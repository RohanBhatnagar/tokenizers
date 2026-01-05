cmake_minimum_required(VERSION 3.20)
project(tokenizers VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tokenizers
    src/bpe/bpe.cpp
)

target_include_directories(tokenizers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(tokenizer src/tokenizer.cpp)
target_link_libraries(tokenizer PRIVATE tokenizers)
