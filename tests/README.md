# BPE Test Suite

This directory contains comprehensive tests for the Byte Pair Encoding (BPE) implementation.

## Test Coverage

The test suite includes 15 tests covering:

### Core Functionality
- **TrainingCompletes**: Verifies training runs without errors
- **VocabularySizeIsCorrect**: Ensures vocabulary size matches expectations
- **TokenizationProducesOutput**: Checks that tokenization generates results
- **CommonWordsMerge**: Validates that frequent words get merged

### Edge Cases
- **TokenizationHandlesEmptyString**: Tests empty input handling
- **SmallVocabSize**: Tests with minimal vocabulary (10 tokens)
- **LargeVocabSize**: Tests with vocabulary larger than possible merges
- **SingleWordCorpus**: Tests training on minimal corpus

### Correctness
- **TokenizationIncludesEndMarkers**: Verifies `</w>` markers are present
- **MultipleWordsTokenized**: Ensures word boundaries are preserved
- **PreservesWordOrder**: Validates token order matches input order
- **HandlesSpecialCharacters**: Tests punctuation and special chars

### Robustness
- **TokenizationIsDeterministic**: Ensures consistent results
- **RepeatedTrainingConsistent**: Validates reproducibility
- **InvalidFileHandling**: Tests error handling for missing files

## Running the Tests

### Build and Run All Tests

```bash
cd /Users/rohanbhatnagar/bpe-cpp
mkdir -p build && cd build
cmake ..
make
ctest --output-on-failure
```

### Run Tests with Verbose Output

```bash
./test_bpe --gtest_verbose
```

### Run Specific Test

```bash
./test_bpe --gtest_filter=BPETest.TrainingCompletes
```

### Run Tests Matching Pattern

```bash
./test_bpe --gtest_filter=BPETest.*Tokenization*
```

## Test Framework

Uses **Google Test** (gtest) framework, automatically downloaded via CMake FetchContent.

## Adding New Tests

To add a new test:

```cpp
TEST_F(BPETest, YourTestName) {
    // Setup
    train(test_corpus_file, 100);
    
    // Execute
    auto result = tokenize("test input");
    
    // Assert
    EXPECT_GT(result.size(), 0);
}
```

## Continuous Integration

These tests should be run:
- Before committing changes
- In CI/CD pipeline
- After modifying BPE algorithm or data structures

## Expected Results

All 15 tests should pass. If any fail:
1. Check the error message from `ctest`
2. Run the specific failing test with verbose output
3. Verify your BPE implementation matches the expected behavior
