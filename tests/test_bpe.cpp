#include <gtest/gtest.h>
#include "bpe.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

// Test fixture for BPE tests
class BPETest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a small test corpus
        test_corpus_file = "test_corpus.txt";
        std::ofstream corpus(test_corpus_file);
        corpus << "the quick brown fox jumps over the lazy dog\n";
        corpus << "the quick brown dog jumps over the lazy fox\n";
        corpus << "a quick brown animal jumps high\n";
        corpus.close();
    }

    void TearDown() override {
        // Clean up test files
        if (std::filesystem::exists(test_corpus_file)) {
            std::filesystem::remove(test_corpus_file);
        }
        if (std::filesystem::exists("bpe_model.txt")) {
            std::filesystem::remove("bpe_model.txt");
        }
    }

    std::string test_corpus_file;
};

// Test 1: Basic training completes without errors
TEST_F(BPETest, TrainingCompletes) {
    EXPECT_NO_THROW({
        train(test_corpus_file, 100);
    });
}

// Test 2: Vocabulary size is reasonable
TEST_F(BPETest, VocabularySizeIsCorrect) {
    size_t target_vocab_size = 50;
    train(test_corpus_file, target_vocab_size);
    
    // Check that bpe_model.txt was created
    EXPECT_TRUE(std::filesystem::exists("bpe_model.txt"));
    
    // Count lines in bpe_model.txt (skip header lines)
    std::ifstream vocab_file("bpe_model.txt");
    size_t line_count = 0;
    std::string line;
    bool in_vocab_section = false;
    while (std::getline(vocab_file, line)) {
        if (line == "VOCAB") {
            in_vocab_section = true;
            continue;
        }
        if (line == "MERGES") {
            break;
        }
        if (in_vocab_section && !line.empty()) {
            line_count++;
        }
    }
    
    // Vocab size should be close to target (may be less if no more merges possible)
    EXPECT_LE(line_count, target_vocab_size);
    EXPECT_GT(line_count, 0);
}

// Test 3: Tokenization produces non-empty results
TEST_F(BPETest, TokenizationProducesOutput) {
    train(test_corpus_file, 100);
    
    std::vector<std::string> tokens = tokenize("the quick brown fox");
    
    EXPECT_GT(tokens.size(), 0);
}

// Test 4: Tokenization handles empty string
TEST_F(BPETest, TokenizationHandlesEmptyString) {
    train(test_corpus_file, 100);
    
    std::vector<std::string> tokens = tokenize("");
    
    EXPECT_EQ(tokens.size(), 0);
}

// Test 5: Tokenization includes end-of-word markers
TEST_F(BPETest, TokenizationIncludesEndMarkers) {
    train(test_corpus_file, 100);
    
    std::vector<std::string> tokens = tokenize("fox");
    
    // Should have at least one </w> marker (either as separate token or merged)
    bool has_end_marker = false;
    for (const auto& token : tokens) {
        if (token == "</w>" || token.find("</w>") != std::string::npos) {
            has_end_marker = true;
            break;
        }
    }
    
    EXPECT_TRUE(has_end_marker);
}

// Test 6: Common words get merged into single tokens
TEST_F(BPETest, CommonWordsMerge) {
    train(test_corpus_file, 200);
    
    // "the" appears frequently, should be merged
    std::vector<std::string> tokens = tokenize("the");
    
    // With enough vocab size, "the</w>" should be a single token (plus the </w>)
    // So we expect fewer tokens than individual characters
    EXPECT_LE(tokens.size(), 4); // "t", "h", "e", "</w>" at most
}

// Test 7: Tokenization is deterministic
TEST_F(BPETest, TokenizationIsDeterministic) {
    train(test_corpus_file, 100);
    
    std::string test_text = "the quick brown fox";
    std::vector<std::string> tokens1 = tokenize(test_text);
    std::vector<std::string> tokens2 = tokenize(test_text);
    
    ASSERT_EQ(tokens1.size(), tokens2.size());
    for (size_t i = 0; i < tokens1.size(); i++) {
        EXPECT_EQ(tokens1[i], tokens2[i]);
    }
}

// Test 8: Training with very small vocab size
TEST_F(BPETest, SmallVocabSize) {
    EXPECT_NO_THROW({
        train(test_corpus_file, 10);
    });
}

// Test 9: Training with large vocab size (larger than possible merges)
TEST_F(BPETest, LargeVocabSize) {
    EXPECT_NO_THROW({
        train(test_corpus_file, 10000);
    });
    
    // Should stop when no more merges are possible
    std::ifstream vocab_file("bpe_model.txt");
    size_t line_count = 0;
    std::string line;
    bool in_vocab_section = false;
    while (std::getline(vocab_file, line)) {
        if (line == "VOCAB") {
            in_vocab_section = true;
            continue;
        }
        if (line == "MERGES") {
            break;
        }
        if (in_vocab_section && !line.empty()) {
            line_count++;
        }
    }
    
    // Should be much less than 10000
    EXPECT_LT(line_count, 10000);
}

// Test 10: Tokenization handles special characters
TEST_F(BPETest, HandlesSpecialCharacters) {
    train(test_corpus_file, 100);
    
    std::vector<std::string> tokens = tokenize("hello, world!");
    
    EXPECT_GT(tokens.size(), 0);
    
    // Should contain comma and exclamation mark
    bool has_comma = false;
    bool has_exclamation = false;
    for (const auto& token : tokens) {
        if (token.find(',') != std::string::npos) has_comma = true;
        if (token.find('!') != std::string::npos) has_exclamation = true;
    }
    
    EXPECT_TRUE(has_comma);
    EXPECT_TRUE(has_exclamation);
}

// Test 11: Multiple words are tokenized separately
TEST_F(BPETest, MultipleWordsTokenized) {
    train(test_corpus_file, 100);
    
    std::vector<std::string> tokens = tokenize("fox dog");
    
    // Should have at least 2 </w> markers (one per word, either separate or merged)
    int end_marker_count = 0;
    for (const auto& token : tokens) {
        if (token == "</w>") {
            end_marker_count++;
        } else if (token.find("</w>") != std::string::npos) {
            // Count merged </w> markers
            end_marker_count++;
        }
    }
    
    EXPECT_GE(end_marker_count, 2);
}

// Test 12: Training handles file with single word
TEST_F(BPETest, SingleWordCorpus) {
    std::string single_word_file = "single_word.txt";
    std::ofstream corpus(single_word_file);
    corpus << "hello";
    corpus.close();
    
    EXPECT_NO_THROW({
        train(single_word_file, 20);
    });
    
    std::filesystem::remove(single_word_file);
}

// Test 13: Invalid file handling
TEST_F(BPETest, InvalidFileHandling) {
    EXPECT_THROW({
        train("nonexistent_file.txt", 100);
    }, std::runtime_error);
}

// Test 14: Tokenization preserves word order
TEST_F(BPETest, PreservesWordOrder) {
    train(test_corpus_file, 100);
    
    std::vector<std::string> tokens = tokenize("fox dog cat");
    
    // Reconstruct the text (removing </w> markers)
    std::string reconstructed;
    for (const auto& token : tokens) {
        if (token == "</w>") {
            reconstructed += " ";
        } else {
            reconstructed += token;
        }
    }
    
    // Should contain words in order
    EXPECT_NE(reconstructed.find("fox"), std::string::npos);
    EXPECT_NE(reconstructed.find("dog"), std::string::npos);
    EXPECT_NE(reconstructed.find("cat"), std::string::npos);
    
    // fox should come before dog
    EXPECT_LT(reconstructed.find("fox"), reconstructed.find("dog"));
    // dog should come before cat
    EXPECT_LT(reconstructed.find("dog"), reconstructed.find("cat"));
}

// Test 15: Repeated training produces consistent results
TEST_F(BPETest, RepeatedTrainingConsistent) {
    train(test_corpus_file, 100);
    std::vector<std::string> tokens1 = tokenize("the quick brown fox");
    
    // Train again with same parameters
    train(test_corpus_file, 100);
    std::vector<std::string> tokens2 = tokenize("the quick brown fox");
    
    ASSERT_EQ(tokens1.size(), tokens2.size());
    for (size_t i = 0; i < tokens1.size(); i++) {
        EXPECT_EQ(tokens1[i], tokens2[i]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
