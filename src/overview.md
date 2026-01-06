# Tokenizers in C++

## Project Overview

The goal of this project is to implement tokenizers in C++ that match or exceed the performance of popular implementations (e.g., HuggingFace tokenizers, SentencePiece). The goal is to build up to **hierarchical modeling** ([Cartesia's approach](https://cartesia.ai/blog/hierarchical-modeling)) to create a dynamic tokenizing network. I also want to get better at C++.

## Tokenization Algorithms

### 1. Byte Pair Encoding (BPE)

**Algorithm Overview:**
- Begin with a corpus of text split into individual characters
- Iteratively merge the most common bigram (pair of adjacent tokens) in the corpus
- Repeat merging until reaching a desired vocabulary size
- The merge sequence becomes the tokenization rules

**Training Process:**
1. Initialize vocabulary with all unique characters
2. Count all adjacent token pairs in the corpus
3. Merge the most frequent pair into a new token
4. Update the corpus with the new token
5. Repeat steps 2-4 until vocab size is reached

**Encoding Process:**
1. Split input text into characters
2. Apply learned merges in order
3. Return the final token sequence

**Key Characteristics:**
- Deterministic and greedy
- Balances vocabulary size with sequence length
- Used in GPT-2, GPT-3, and many other models

**Performance:**
Only trained on 1mb of data, since it takes some time to train. I will revisit efficiency later. Currently it is O(n^2) for training and O(n) for encoding. After training to a vocabulary of 1000, I got the following results: 

```
My name is Rohan, I am a senior at University of Maryland, College Park.
```

```
M y</w> n ame</w> is</w> R oh an ,</w> I</w> am</w> a</w> sen i or</w> at</w> Un iv er s ity</w> of</w> Mar y l and ,</w> Col le ge</w> P ark .</w>
```

### 2. SentencePiece

**Algorithm Overview:**
SentencePiece is a language-independent tokenizer that treats text as a sequence of Unicode characters. It can use either BPE or Unigram algorithm under the hood.

**Two Main Algorithms:**

#### BPE Mode
- Similar to standard BPE but operates on raw text (no pre-tokenization)
- Handles whitespace as a special character (▁)
- Language-agnostic approach

#### Unigram Mode
- **Training:** Start with a large vocabulary, iteratively prune tokens based on likelihood
- Uses a probabilistic language model
- Keeps tokens that maximize the likelihood of the training corpus
- More flexible than BPE (multiple tokenization options per word)

**Key Characteristics:**
- No pre-tokenization required
- Reversible (can perfectly reconstruct original text including spaces)
- Used in T5, ALBERT, XLNet, Llama, and most modern LLMs
- Supports multiple languages without modification

### 3. WordPiece (Future)
- Used in BERT
- Similar to BPE but uses likelihood-based scoring instead of frequency

## Performance Goals

### Benchmarks to Match/Exceed:
- **HuggingFace Tokenizers** (Rust-based)
- **Google SentencePiece** (C++ reference implementation)
- **tiktoken** (OpenAI's BPE implementation)

### Optimization Strategies:
1. **Efficient data structures:**
   - Hash maps for vocabulary lookups
   - Priority queues for merge selection
   - Trie structures for prefix matching

2. **Memory optimization:**
   - Minimize allocations during encoding
   - Reuse buffers where possible
   - Compact representation of merge rules

3. **Parallelization:**
   - Multi-threaded training for large corpora
   - Batch encoding support
   - SIMD optimizations for character operations

4. **Algorithmic improvements:**
   - Fast pair counting using suffix arrays
   - Incremental updates during training
   - Efficient merge application

## Hierarchical Modeling Roadmap

### What is Hierarchical Modeling?

Hierarchical modeling (as described by [Cartesia](https://cartesia.ai/blog/hierarchical-modeling)) involves:
- **Multi-scale tokenization:** Different granularities (characters → subwords → words → phrases)
- **Dynamic token boundaries:** Learned, context-dependent segmentation
- **Hierarchical representations:** Tokens at different levels inform each other

### Implementation Path:

#### Phase 1: Foundation (Current)
- ✅ Implement standard BPE
- ✅ Implement SentencePiece (BPE + Unigram)
- Achieve competitive performance

#### Phase 2: Multi-Scale Tokenization
- Implement multiple tokenizers at different granularities
- Character-level, subword-level, word-level
- Efficient encoding/decoding across scales

#### Phase 3: Dynamic Boundaries
- Neural network to predict optimal token boundaries
- Context-aware segmentation
- Learned merge decisions (vs. fixed merge table)

#### Phase 4: Hierarchical Network
- Multi-level token embeddings
- Cross-scale attention mechanisms
- End-to-end trainable tokenization + modeling
- Integration with transformer architectures

### Benefits of Hierarchical Approach:
- **Better generalization:** Handle rare words and OOV tokens
- **Efficiency:** Adaptive granularity based on context
- **Interpretability:** Explicit multi-scale structure
- **Performance:** Potential improvements in downstream tasks

## Project Structure

```
bpe-cpp/
├── src/
│   ├── bpe.cpp              # BPE implementation
│   ├── sentencepiece.cpp    # SentencePiece implementation
│   └── overview.md          # This file
├── in.txt                   # Input corpus for training
└── out.txt                  # Output (tokenized results)
```

## References

- [BPE Paper](https://arxiv.org/abs/1508.07909) - Neural Machine Translation of Rare Words with Subword Units
- [SentencePiece Paper](https://arxiv.org/abs/1808.06226) - SentencePiece: A simple and language independent approach
- [Cartesia Hierarchical Modeling](https://cartesia.ai/blog/hierarchical-modeling)
- [HuggingFace Tokenizers](https://github.com/huggingface/tokenizers)
- [Google SentencePiece](https://github.com/google/sentencepiece)

## Next Steps

1. Complete BPE implementation with training + encoding
2. Implement SentencePiece (both BPE and Unigram modes)
3. Add comprehensive benchmarking suite
4. Optimize for performance (profiling, SIMD, multi-threading)
5. Begin hierarchical modeling experiments