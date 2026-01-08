#include "../include/bpe.hpp"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    bool train_model = false; 
    int vocab_size = 0; 

    // not checking for invalid args
    if (argc > 1) {
        if (std::string(argv[1]) == "--train") {
            train_model = true; 
        }
        vocab_size = std::stoi(argv[2]);
    }
    
    if (train_model) {
        train("wikitext2.txt", vocab_size);
    }

    load_model("bpe_model.txt");
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
