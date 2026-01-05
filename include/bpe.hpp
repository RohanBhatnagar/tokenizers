#ifndef BPE_HPP
#define BPE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>

void load_training_txt(const std::string& training_txt_file);

void train(const std::string& vocab_file);

void load_merges(const std::string& merges_file);

std::vector<std::pair<std::string, std::string> > get_pairs(const std::vector<std::string>& tokens);

std::vector<std::string> encode_word(const std::string& word);

std::vector<std::string> encode(const std::string& text);

std::vector<int> tokens_to_ids(const std::vector<std::string>& tokens);

#endif 