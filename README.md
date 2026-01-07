# BPE Tokenizer (C++)

A somewhat efficient C++ implementation of Byte Pair Encoding (BPE) for tokenization. Still short of the optimal algorithm, but implemented most of it. 

## Features

- ✅ **Training**: Learn BPE merges from raw text data
- ✅ **Tokenization**: Convert text into subword tokens using learned vocabulary
- ✅ **Model Persistence**: Save and load trained models (see bpe_model.txt)

## Quick Start

### Build

```bash
cmake -B build -S .
cmake --build build
```

### Train a Model

```cpp
#include <bpe.hpp>

// Train on your text file. I used a 13mb text file, training took about 2 minutes. 
train("training_data.txt", 5000);  // Target vocab size: 5000
```

This will create `bpe_model.txt` containing the learned vocabulary and merges.

### Tokenize Text

```cpp
#include <bpe.hpp>

// Load the trained model
load_model("bpe_model.txt");

// Tokenize text
std::vector<std::string> tokens = tokenize("Your text here");
```

## Example Output

**Input:**
```
Byte Pair Encoding is a data compression technique that iteratively replaces 
the most frequent pair of bytes in a sequence with a single unused byte.
```

**Tokens (189 total):**
```
By_te_</w>_P_air_</w>_En_c_od_ing_</w>_is_</w>_a_</w>_d_ata_</w>_comp_ression_</w>
_techn_ique_</w>_that_</w>_iter_ative_ly_</w>_replac_es_</w>_the_</w>_most_</w>
_frequ_ent_</w>_pair_</w>_of_</w>_by_t_es_</w>_in_</w>_a_</w>_sequence_</w>_with_</w>
_a_</w>_single_</w>_un_used_</w>_by_te_._</w>
```

Notice how:
- Common words like `"the"`, `"is"`, `"that"` are single tokens
- Frequent subwords like `"ing"`, `"tion"`, `"ly"` are learned
- Each word ends with `</w>` (end-of-word marker)
- Rare words are split into smaller subword units

## Project Structure

```
bpe-cpp/
├── include/
│   ├── bpe.hpp              # BPE interface
│   └── indexed_heap.hpp     # Priority queue for merge selection
├── src/
│   ├── bpe/
│   │   └── bpe.cpp          # BPE implementation
│   ├── util/
│   │   └── indexed_heap.cpp # Heap implementation
│   └── tokenizer.cpp        # Example usage
└── tests/                   # Unit tests
```

## Requirements

- C++20 or later
- CMake 3.20+
- AppleClang 14.0+ (or equivalent)
