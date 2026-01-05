#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ranges>
#include <filesystem>

#include "bpe.hpp"

std::vector<std::string> vocab; 
std::map<std::pair<std::string, std::string>, int> freqs; 
std::vector<std::pair<std::string, std::string> > merges; 

// helpers 
namespace {

    // preprocess each word into [c1 c2 c3 ... </w> ] and write to a file. 
    void preprocess(const std::string& train_file, const std::string& preproc_file) {
        std::ifstream file(train_file); 
        std::ofstream outfile(preproc_file); 

        if (!file) {
            throw std::runtime_error("Unable to open training txt file");
        }

        if (!outfile) {
            throw std::runtime_error("Unable to open preprocessed txt file");
        }

        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line); 
            std::string word; 
            while (ss >> word) {
                for (size_t i = 0; i < word.size(); ++i) {
                    outfile << word[i] << " "; 
                }
                outfile << "</w>\n"; 
            }
        }
        outfile.close(); 
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

void count_freqs(const std::string& raw_data) {
    std::ifstream raw(raw_data); 
    if (!raw) {
        throw std::runtime_error("Unable to open raw data file");
    }
    std::string line; 
    while (std::getline(raw, line)) {
        std::stringstream ss(line); 
        std::string t1; 
        ss >> t1; 
        std::string t2;
        while (ss >> t2) {
            freqs[{t1, t2}]++;
            t1 = t2; 
        }
    }
    
}

// Streams new tokens into tmp file
void apply_merge(const std::string& old) {
    std::pair<std::string, std::string> merge = get_merge(); 

    std::filesystem::path temp = old + ".tmp"; 

    std::ifstream file(old); 
    std::ofstream outfile(temp); 
    
    if (!file) {
        throw std::runtime_error("Unable to open old txt file");
    }
    if (!outfile) {
        throw std::runtime_error("Unable to open new txt file");
    }

    std::string line; 
    while (std::getline(file, line)) {
        std::stringstream ss(line); 
        std::string t1; 
        ss >> t1; 
        std::string t2; 
        while (ss >> t2) {
            if (t1 == merge.first && t2 == merge.second) {
                outfile << merge.first << merge.second << " "; 
            } else {
                outfile << t1 << " "; 
            }
            t1 = t2; 
        }
        outfile << "</w> \n"; 
    }
    outfile.close(); 
    file.close(); 

    merges.emplace_back(merge);
    vocab.push_back(merge.first + merge.second);
    std::filesystem::rename(temp, old); 
}

void train(const std::string& raw_data, size_t vocab_size) {
    std::string preproc = "preproc.txt"; 
    preprocess(raw_data, preproc); 
    while (vocab.size() < vocab_size) {
        count_freqs(preproc); 
        apply_merge(preproc); 
    }
}

int main() {
    train("raw_data.txt", 10000); 
}