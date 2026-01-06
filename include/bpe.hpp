#ifndef BPE_HPP
#define BPE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>

// Count frequencies of adjacent token pairs in the training data
void count_freqs();

// Apply a merge operation to a token vector
void apply_merge_to(std::vector<std::string>& tokens, 
                    const std::pair<std::string, std::string>& merge, 
                    bool training);

// Train BPE on the given raw data file, building a vocabulary of the specified size
void train(const std::string& raw_data, size_t vocab_size);

// Tokenize text using the learned BPE merges
std::vector<std::string> tokenize(const std::string& text);

#endif