#include "../include/bpe.hpp"
#include <iostream>
#include <vector>
#include <string>

int main() {
    load_model("bpe_model.txt");
    // Test tokenization
    std::string test_text = "Byte Pair Encoding is a data compression technique that iteratively replaces the most frequent pair of bytes in a sequence with a single unused byte. In natural language processing, BPE has become a popular subword tokenization algorithm used in many modern language models including GPT and BERT. The algorithm starts with a vocabulary of individual characters and gradually builds up larger subword units by merging the most frequently occurring pairs.";
    std::cout << "Input text: " << test_text << std::endl;
    std::cout << std::endl;
    std::vector<std::string> tokens = tokenize(test_text);    
    std::cout << "Tokens (" << tokens.size() << "):" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i];
        if (i < tokens.size() - 1) {
            std::cout << "_";
        }
    }
    std::cout << std::endl;
    
    return 0;
}
