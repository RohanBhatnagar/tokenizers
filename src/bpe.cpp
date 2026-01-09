#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ranges>
#include <filesystem>
#include <set>
#include <unordered_set>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>

#include <bpe.hpp>
#include <indexed_heap.hpp>

struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        auto h1 = std::hash<int>{}(p.first);
        auto h2 = std::hash<int>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

std::unordered_map<std::string, int> vocab_to_id; 
std::unordered_map<int, std::string> id_to_vocab; 
int vocab_size = 0; 

std::unordered_map<std::pair<int, int>, std::list<int>, PairHash> occurrences; 
std::vector<std::pair<int, int> > merges;

IndexedHeap frequency_heap;
std::unordered_map<std::pair<int, int>, std::unique_ptr<HeapNode>, PairHash> pair_frequencies; 

// default tokens. 
const std::string EOW = "</w>";
const std::string EOS = "<|endoftext|>";

namespace {

    // Profiling data
    struct ProfileData {
        std::chrono::duration<double> bump_priority_time{0};
        std::chrono::duration<double> count_freqs_time{0};
        std::chrono::duration<double> apply_merge_time{0};
        std::chrono::duration<double> get_merge_time{0};
        long long bump_priority_calls = 0;
        long long apply_merge_calls = 0;
    } profile_data;

    void print_profile_stats() {
        std::cout << "\n=== Performance Profile ===\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "count_freqs:    " << profile_data.count_freqs_time.count() << "s\n";
        std::cout << "get_merge:      " << profile_data.get_merge_time.count() << "s\n";
        std::cout << "apply_merge:    " << profile_data.apply_merge_time.count() << "s (" 
                  << profile_data.apply_merge_calls << " calls)\n";
        std::cout << "bump_priority:  " << profile_data.bump_priority_time.count() << "s (" 
                  << profile_data.bump_priority_calls << " calls)\n";
        std::cout << "===========================\n\n";
    }

    void add_def_tokens() {
        vocab_to_id[EOW] = vocab_size; 
        id_to_vocab[vocab_size++] = EOW; 
        vocab_to_id[EOS] = vocab_size; 
        id_to_vocab[vocab_size++] = EOS; 
    }

    // DLL within a fixed size vector. 
    struct DLLNode {
        int tok; 
        int prev; 
        int next; 
        bool active; 
    };

    void bump_priority(const std::pair<int,int>& p, int delta) {
        auto it = pair_frequencies.find(p);
        if (it == pair_frequencies.end()) {
            auto node_ptr = std::make_unique<HeapNode>();
            node_ptr->tok_ids = p;
            node_ptr->priority = 0;
            frequency_heap.push(node_ptr.get()); // inserts into heap
            auto insert_result = pair_frequencies.insert({p, std::move(node_ptr)});
            it = insert_result.first;  // Use iterator from insert, not redundant find
        }
        HeapNode* node = it->second.get();
        int newPri = node->priority + delta;
        if (newPri < 0) newPri = 0;
        frequency_heap.updatePriority(node, newPri);
    }


    std::vector<DLLNode> train_tokens; 

    void preprocess_train(const std::string& train_file) {
        add_def_tokens(); 
        std::ifstream file(train_file);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open training file: " + train_file);
        }
        std::string line; 
        while (std::getline(file, line)) {
            std::stringstream ss(line); 
            std::string word; 
            while (ss >> word) {
                size_t n = word.size(); 
                for (size_t i = 0; i < n; ++i) {
                    std::string tok = word.substr(i, 1); 
                    if (vocab_to_id.find(tok) == vocab_to_id.end()) {
                        vocab_to_id[tok] = vocab_size; 
                        id_to_vocab[vocab_size++] = tok; 
                    }
                    DLLNode node = {vocab_to_id[tok], -1, -1, true};
                    train_tokens.push_back(node);
                }
                DLLNode eow_node = {vocab_to_id[EOW], -1, -1, true};
                train_tokens.push_back(eow_node);
            }
        }
        // Set prev and next ptrs. 
        size_t n = train_tokens.size(); 
        for (size_t i = 0; i < n; ++i) {
                DLLNode& node = train_tokens[i]; 
            if (i > 0) {
                node.prev = i - 1; 
            }
            if (i + 1 < n) {
                node.next = i + 1; 
            }
        }
    }

    std::pair<int,int> get_merge() {
        while (!frequency_heap.empty()) {
            HeapNode* node = frequency_heap.pop();
            if (node->priority <= 0) continue;

            auto itOcc = occurrences.find(node->tok_ids);
            if (itOcc == occurrences.end() || itOcc->second.empty()) continue;

            return node->tok_ids;
        }
        throw std::runtime_error("No more merges available");
    }

    void count_freqs(const std::vector<DLLNode>& tokens) {
        const int EOW_ID = vocab_to_id[EOW];
        size_t n = tokens.size(); 
        for (size_t i = 0; i < n; ++i) {
            const DLLNode& token = tokens[i];
            if (token.next == -1) continue;
            if (token.tok == EOW_ID) continue;
            if (tokens[token.next].tok == EOW_ID) continue;
            
            std::pair<int, int> tok_pair(token.tok, tokens[token.next].tok);
            auto it = pair_frequencies.find(tok_pair);
            if (it == pair_frequencies.end()) {
                auto node_ptr = std::make_unique<HeapNode>();
                node_ptr->tok_ids = std::make_pair(tok_pair.first, tok_pair.second);
                node_ptr->priority = 1;
                frequency_heap.push(node_ptr.get());
                pair_frequencies[tok_pair] = std::move(node_ptr);
            } else {
                HeapNode* node = it->second.get();
                frequency_heap.updatePriority(node, node->priority + 1);
            }
            occurrences[tok_pair].push_back(i);
        }
    }

    void apply_merge_to(std::vector<DLLNode>& tokens, const std::pair<int,int>& merge) {
        auto start = std::chrono::high_resolution_clock::now();
        profile_data.apply_merge_calls++;
        
        auto itOcc = occurrences.find(merge);
        if (itOcc == occurrences.end()) {
            auto end = std::chrono::high_resolution_clock::now();
            profile_data.apply_merge_time += (end - start);
            return;
        }

        const std::list<int>& indices = itOcc->second;
        
        int new_id = vocab_size;
        bool did_merge = false;

        for (int idx : indices) {

            DLLNode& token = tokens[idx];
            if (!token.active || token.next == -1 || token.tok != merge.first) continue;

            DLLNode& next_token = tokens[token.next];
            if (!next_token.active || next_token.tok != merge.second) continue;

            did_merge = true;

            // remove occurrences (prev, token), (token, next), (next, next.next)
            if (token.prev != -1 && tokens[token.prev].active) {
                std::pair<int,int> prev_pair(tokens[token.prev].tok, token.tok);
                auto it = occurrences.find(prev_pair);
                if (it != occurrences.end()) {
                    auto& occ_list = it->second;
                    occ_list.remove(token.prev);
                    if (occ_list.empty()) {
                        occurrences.erase(it);
                    }
                }
                bump_priority(prev_pair, -1);
            }
        
            bump_priority(std::make_pair(token.tok, next_token.tok), -1);
            
            // (next_token, next_token.next) 
            if (next_token.next != -1 && tokens[next_token.next].active) {
                std::pair<int,int> next_pair(next_token.tok, tokens[next_token.next].tok);
                auto it = occurrences.find(next_pair);
                if (it != occurrences.end()) {
                    auto& occ_list = it->second;
                    occ_list.remove(token.next);
                    if (occ_list.empty()) {
                        occurrences.erase(it);
                    }
                }
                bump_priority(next_pair, -1);
            }

            // merge
            int next_neighbor_idx = next_token.next;
            token.tok = new_id;
            token.next = next_neighbor_idx;
            if (next_neighbor_idx != -1) tokens[next_neighbor_idx].prev = idx;
            next_token.active = false;

            // add new occs. 
            const int EOW_ID = vocab_to_id[EOW];  // Cache for fast comparison
            if (token.prev != -1 && tokens[token.prev].active) {
                // Only check if tokens are NOT the EOW token (fast integer comparison)
                if (tokens[token.prev].tok != EOW_ID && token.tok != EOW_ID) {
                    std::pair<int,int> new_prev_pair(tokens[token.prev].tok, token.tok);
                    bump_priority(new_prev_pair, +1);
                    occurrences[new_prev_pair].push_back(token.prev);
                }
            }
            if (token.next != -1 && tokens[token.next].active) {
                if (token.tok != EOW_ID && tokens[token.next].tok != EOW_ID) {
                
                    std::pair<int,int> new_next_pair(token.tok, tokens[token.next].tok);
                    bump_priority(new_next_pair, +1);
                    occurrences[new_next_pair].push_back(idx);
                }
            }
        }

        if (!did_merge) {
            auto end = std::chrono::high_resolution_clock::now();
            profile_data.apply_merge_time += (end - start);
            return;
        }

        // update vocab
        std::string new_token_str = id_to_vocab[merge.first] + id_to_vocab[merge.second];
        vocab_to_id[new_token_str] = new_id;
        id_to_vocab[new_id] = new_token_str;
        vocab_size++;

        merges.push_back(merge);
        occurrences.erase(merge);
        
        auto end = std::chrono::high_resolution_clock::now();
        profile_data.apply_merge_time += (end - start);
    }


    void clear() {
        vocab_to_id.clear(); 
        id_to_vocab.clear(); 
        occurrences.clear(); 
        merges.clear(); 
        pair_frequencies.clear(); 
        frequency_heap.clear(); 
        train_tokens.clear();
        vocab_size = 0;
    }
}

void save_model(const std::string& output_file) {
    std::ofstream out(output_file);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file: " + output_file);
    }
    
    out << "VOCAB_SIZE " << vocab_size << "\n";
    out << "VOCAB\n";
    for (const auto& pair : vocab_to_id) {
        out << pair.first << "\t" << pair.second << "\n";
    }
    
    out << "MERGES\n";
    for (const auto& merge : merges) {
        std::string first_tok = id_to_vocab[merge.first];
        std::string second_tok = id_to_vocab[merge.second];
        out << first_tok << " " << second_tok << "\n";
    }
    
    out.close();
    std::cout << "Model saved to " << output_file << "\n";
    std::cout << "Final vocabulary size: " << vocab_size << "\n";
    std::cout << "Number of merges learned: " << merges.size() << "\n";
}

void train(const std::string& raw_data, size_t target_vocab_size) {    
    clear();  
    std::cout << "Preprocessing training data..." << std::endl;
    preprocess_train(raw_data);
    std::cout << " Initial vocabulary size: " << vocab_size << std::endl;
    count_freqs(train_tokens);    
    int merge_count = 0;
    while (vocab_size < target_vocab_size && !frequency_heap.empty()) {
        try {
            std::pair<int, int> merge = get_merge(); 
            
                std::cout << "  Merge " << merge_count << ": " << id_to_vocab[merge.first] 
                        << " + " << id_to_vocab[merge.second] << std::endl;
            
            apply_merge_to(train_tokens, merge); 
            merge_count++;
        } catch (const std::runtime_error& e) {
            std::cout << "  No more valid merges available. Stopping at vocab size: " << vocab_size << std::endl;
            break;
        }
    }
    
    print_profile_stats();
    save_model("bpe_model.txt");
    std::cout << std::endl;
    std::cout << "=== Training Complete ===" << std::endl;
}

void load_model(const std::string& model_file) {
    std::cout << "Loading BPE model from " << model_file << "..." << std::endl;
    
    clear();
    
    std::ifstream in(model_file);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open model file: " + model_file);
    }
    
    std::string line;
    
    // Read vocab size
    std::getline(in, line);
    if (line.substr(0, 11) != "VOCAB_SIZE ") {
        throw std::runtime_error("Invalid model file format: expected VOCAB_SIZE");
    }
    int expected_vocab_size = std::stoi(line.substr(11));
    
    // Read VOCAB header
    std::getline(in, line);
    if (line != "VOCAB") {
        throw std::runtime_error("Invalid model file format: expected VOCAB");
    }
    
    // Read vocabulary
    while (std::getline(in, line) && line != "MERGES") {
        size_t tab_pos = line.find('\t');
        if (tab_pos == std::string::npos) continue;
        
        std::string token = line.substr(0, tab_pos);
        int id = std::stoi(line.substr(tab_pos + 1));
        
        vocab_to_id[token] = id;
        id_to_vocab[id] = token;
        if (id >= vocab_size) vocab_size = id + 1;
    }
    
    // Read merges (line already contains "MERGES" or next merge)
    if (line == "MERGES") {
        std::getline(in, line);
    }
    
    do {
        if (line.empty()) continue;
        
        size_t space_pos = line.find(' ');
        if (space_pos == std::string::npos) continue;
        
        std::string first = line.substr(0, space_pos);
        std::string second = line.substr(space_pos + 1);
        
        // Convert to IDs and store
        if (vocab_to_id.find(first) != vocab_to_id.end() && 
            vocab_to_id.find(second) != vocab_to_id.end()) {
            merges.push_back(std::make_pair(vocab_to_id[first], vocab_to_id[second]));
        }
    } while (std::getline(in, line));
    
    in.close();
    
    std::cout << "  Loaded vocabulary size: " << vocab_size << std::endl;
    std::cout << "  Loaded merges: " << merges.size() << std::endl;
    std::cout << "Model loaded successfully!" << std::endl << std::endl;
}

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::vector<int> > words;
    std::stringstream ss(text);
    std::string word;
    while (ss >> word) {
        std::vector<int> token_ids;
        for (size_t i = 0; i < word.size(); ++i) {
            std::string ch = word.substr(i, 1);
            // If character not in vocab, add it
            if (vocab_to_id.find(ch) == vocab_to_id.end()) {
                vocab_to_id[ch] = vocab_size;
                id_to_vocab[vocab_size++] = ch;
            }
            token_ids.push_back(vocab_to_id[ch]);
        }
        if (vocab_to_id.find(EOW) != vocab_to_id.end()) {
            token_ids.push_back(vocab_to_id[EOW]);
        }
        words.push_back(token_ids);
    }
    for (auto& token_ids : words) {
        for (const auto& merge : merges) {
            bool changed = true;
            while (changed) {
                changed = false;
                for (size_t i = 0; i + 1 < token_ids.size(); ++i) {
                    if (token_ids[i] == merge.first && token_ids[i + 1] == merge.second) {
                        std::string merged_token = id_to_vocab[merge.first] + id_to_vocab[merge.second];
                        int merged_id;
                        if (vocab_to_id.find(merged_token) != vocab_to_id.end()) {
                            merged_id = vocab_to_id[merged_token];
                        } else {
                            continue;
                        }
                        token_ids[i] = merged_id;
                        token_ids.erase(token_ids.begin() + i + 1);
                        changed = true;
                        break;
                    }
                }
            }
        }
    }
    std::vector<std::string> result;
    for (const auto& token_ids : words) {
        for (int id : token_ids) {
            result.push_back(id_to_vocab[id]);
        }
    }
    
    return result;
}