cmake_minimum_required(VERSION 3.20)
project(tokenizers VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tokenizers
    src/bpe.cpp
    src/util/indexed_heap.cpp
    src/util/doubly_linked_list.cpp
)

target_include_directories(tokenizers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(tokenizer src/run_tokenizer.cpp)
target_link_libraries(tokenizer PRIVATE tokenizers)


# Testing
enable_testing()

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(test_bpe tests/test_bpe.cpp)
target_link_libraries(test_bpe PRIVATE tokenizers GTest::gtest_main)

add_executable(test_indexed_heap tests/test_indexed_heap.cpp)
target_link_libraries(test_indexed_heap PRIVATE tokenizers GTest::gtest_main)

add_executable(test_doubly_linked_list tests/test_doubly_linked_list.cpp)
target_link_libraries(test_doubly_linked_list PRIVATE tokenizers GTest::gtest_main)

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_bpe)
gtest_discover_tests(test_indexed_heap)
gtest_discover_tests(test_doubly_linked_list)

