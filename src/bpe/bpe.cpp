#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ranges>
#include <filesystem>
#include <unordered_set>

#include "bpe.hpp"

std::unordered_set<std::string> vocab;
std::map<std::pair<std::string, std::string>, int> freqs; 
std::vector<std::pair<std::string, std::string> > merges; 
std::vector<std::string> training_tokens; 

namespace {

    void preprocess_train(const std::string& train_file) {
        std::ifstream file(train_file); 

        if (!file) { throw std::runtime_error("Unable to open training txt file"); }

        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line); 
            std::string word; 
            while (ss >> word) {
                for (size_t i = 0; i < word.size(); ++i) {
                    training_tokens.emplace_back(std::string(1, word[i])); 
                }
                training_tokens.emplace_back("</w>"); 
            }
        }
        file.close(); 
    }

    std::pair<std::string, std::string> get_merge() {
        std::pair<std::string, std::string> max_pair; 
        int max_count = 0; 
        for (const auto& [pair, count] : freqs) {
            if (count > max_count) {
                max_count = count; 
                max_pair = pair; 
            }
        }
        return max_pair; 
    }
}

void count_freqs() {  
    auto n = training_tokens.size(); 
    for (size_t i = 0; i < n; ++i) {
        if (i + 1 < n && training_tokens[i] != "</w>") {
            freqs[{training_tokens[i], training_tokens[i + 1]}]++; 
        }
    }  
}

void apply_merge_to(std::vector<std::string>& tokens, const std::pair<std::string, std::string>& merge, bool training) {
    auto n = tokens.size();
    size_t write_idx = 0;  
    const std::string new_token = merge.first + merge.second; 

    for (size_t i = 0; i < n; ++i) {
        if (i + 1 < n && tokens[i] == merge.first && tokens[i + 1] == merge.second) {
            tokens[write_idx++] = new_token; 
            ++i; 
        } else {
            tokens[write_idx++] = tokens[i]; 
        }
    }

    tokens.resize(write_idx); 
    if (training) {
        merges.emplace_back(merge); 
        vocab.insert(new_token); 
    }
}

void train(const std::string& raw_data, size_t vocab_size) {
    preprocess_train(raw_data); 
    freqs.clear(); 
    merges.clear(); 
    vocab.clear(); 
    std::string word;
    std::ifstream corpus(raw_data); 
    while (corpus >> word) {
        for (char c : word) {
            vocab.insert(std::string(1, c));
        }
        vocab.insert("</w>");
    }
    while (vocab.size() < vocab_size) {
        std::cout << "Vocab size: " << vocab.size() << std::endl; 
        freqs.clear(); 
        count_freqs(); 
        auto m = get_merge();
        if (m.first.empty()) break;     
        apply_merge_to(training_tokens, m, true); 
    }
    std::cout << "Training complete" << std::endl; 
    std::ofstream vocab_file("vocab.txt"); 
    for (const auto& token : vocab) {
        vocab_file << token << "\n"; 
    }
    vocab_file.close(); 
}

std::vector<std::string> tokenize(const std::string& text) {

    std::stringstream ss(text); 
    std::vector<std::string> tokenized; 
    std::string word; 

    while (ss >> word) {
        for (auto c : word) {
            tokenized.emplace_back(std::string(1, c)); 
        }
        tokenized.emplace_back("</w>"); 
    }

    // very inefficient, but okay for now. 
    for (const auto& merge : merges) {
        apply_merge_to(tokenized, merge, false); 
    }
    return tokenized; 
}