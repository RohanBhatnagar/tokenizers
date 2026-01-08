#include <gtest/gtest.h>
#include <doubly_linked_list.hpp>
#include <string>

// Test basic construction and empty state
TEST(DoublyLinkedListTest, ConstructorAndEmpty) {
    DoublyLinkedList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

// Test push_back and push_front
TEST(DoublyLinkedListTest, PushOperations) {
    DoublyLinkedList<int> list;
    
    list.push_back(1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 1);
    
    list.push_back(2);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(list.back(), 2);
    
    list.push_front(0);
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.front(), 0);
    EXPECT_EQ(list.back(), 2);
}

// Test pop operations
TEST(DoublyLinkedListTest, PopOperations) {
    DoublyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.pop_front();
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 2);
    
    list.pop_back();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.back(), 2);
    
    list.pop_front();
    EXPECT_TRUE(list.empty());
}

// Test pop on empty list throws exception
TEST(DoublyLinkedListTest, PopEmptyThrows) {
    DoublyLinkedList<int> list;
    EXPECT_THROW(list.pop_front(), std::out_of_range);
    EXPECT_THROW(list.pop_back(), std::out_of_range);
}

// Test front/back on empty list throws exception
TEST(DoublyLinkedListTest, AccessEmptyThrows) {
    DoublyLinkedList<int> list;
    EXPECT_THROW(list.front(), std::out_of_range);
    EXPECT_THROW(list.back(), std::out_of_range);
}

// Test iterator traversal
TEST(DoublyLinkedListTest, IteratorTraversal) {
    DoublyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    std::vector<int> values;
    for (auto it = list.begin(); it != list.end(); ++it) {
        values.push_back(*it);
    }
    
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 3);
}

// Test range-based for loop
TEST(DoublyLinkedListTest, RangeBasedFor) {
    DoublyLinkedList<int> list;
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    
    std::vector<int> values;
    for (int val : list) {
        values.push_back(val);
    }
    
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 10);
    EXPECT_EQ(values[1], 20);
    EXPECT_EQ(values[2], 30);
}

// Test copy constructor
TEST(DoublyLinkedListTest, CopyConstructor) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    DoublyLinkedList<int> list2(list1);
    
    EXPECT_EQ(list2.size(), 3);
    EXPECT_EQ(list2.front(), 1);
    EXPECT_EQ(list2.back(), 3);
    
    // Verify deep copy
    list1.pop_front();
    EXPECT_EQ(list1.size(), 2);
    EXPECT_EQ(list2.size(), 3);
}

// Test copy assignment
TEST(DoublyLinkedListTest, CopyAssignment) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    
    DoublyLinkedList<int> list2;
    list2.push_back(99);
    
    list2 = list1;
    
    EXPECT_EQ(list2.size(), 2);
    EXPECT_EQ(list2.front(), 1);
    EXPECT_EQ(list2.back(), 2);
}

// Test move constructor
TEST(DoublyLinkedListTest, MoveConstructor) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    
    DoublyLinkedList<int> list2(std::move(list1));
    
    EXPECT_EQ(list2.size(), 3);
    EXPECT_EQ(list2.front(), 1);
    EXPECT_EQ(list2.back(), 3);
    EXPECT_TRUE(list1.empty());
}

// Test move assignment
TEST(DoublyLinkedListTest, MoveAssignment) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    
    DoublyLinkedList<int> list2;
    list2.push_back(99);
    
    list2 = std::move(list1);
    
    EXPECT_EQ(list2.size(), 2);
    EXPECT_EQ(list2.front(), 1);
    EXPECT_EQ(list2.back(), 2);
    EXPECT_TRUE(list1.empty());
}

// Test clear
TEST(DoublyLinkedListTest, Clear) {
    DoublyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    list.clear();
    
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

// Test with strings
TEST(DoublyLinkedListTest, StringType) {
    DoublyLinkedList<std::string> list;
    list.push_back("hello");
    list.push_back("world");
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), "hello");
    EXPECT_EQ(list.back(), "world");
}

// Test insert at beginning
TEST(DoublyLinkedListTest, InsertAtBeginning) {
    DoublyLinkedList<int> list;
    list.push_back(2);
    list.push_back(3);
    
    auto it = list.insert(list.begin(), 1);
    
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(*it, 1);
}

// Test insert at end
TEST(DoublyLinkedListTest, InsertAtEnd) {
    DoublyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    
    auto it = list.insert(list.end(), 3);
    
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.back(), 3);
    EXPECT_EQ(*it, 3);
}

// Test erase
TEST(DoublyLinkedListTest, Erase) {
    DoublyLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    auto it = list.begin();
    ++it; // Point to 2
    
    auto next_it = list.erase(it);
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(*next_it, 3);
    
    // Verify list contains 1, 3
    std::vector<int> values;
    for (int val : list) {
        values.push_back(val);
    }
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 3);
}

// Test erase at end throws
TEST(DoublyLinkedListTest, EraseEndThrows) {
    DoublyLinkedList<int> list;
    list.push_back(1);
    
    EXPECT_THROW(list.erase(list.end()), std::out_of_range);
}
