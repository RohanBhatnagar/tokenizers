#include "bpe.hpp"
#include <iostream>
#include <vector>
#include <string>

int main() {
    train("wikitext2_1mb.txt", 1000); 

    std::vector<std::string> tokens = tokenize("My name is Rohan, I am a senior at University of Maryland, College Park."); 
    for (const auto& token : tokens) {
        std::cout << token << " "; 
    }
    std::cout << std::endl; 
    return 0; 
}
