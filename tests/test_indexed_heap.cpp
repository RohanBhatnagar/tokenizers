#include <gtest/gtest.h>
#include "indexed_heap.hpp"
#include <unordered_map>

// Test fixture for IndexedHeap tests
class IndexedHeapTest : public ::testing::Test {
protected:
    IndexedHeap heap;
    std::unordered_map<int, HeapNode> nodes;  // Store nodes here
};

// Test 1: Basic push and top
TEST_F(IndexedHeapTest, PushAndTop) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    nodes[3] = HeapNode();
    nodes[3].tok_ids = {5, 6};
    nodes[3].priority = 15;
    
    heap.push(&nodes[1]);
    heap.push(&nodes[2]);
    heap.push(&nodes[3]);
    
    HeapNode* top = heap.top();
    EXPECT_EQ(top->tok_ids.first, 3);
    EXPECT_EQ(top->tok_ids.second, 4);
    EXPECT_EQ(top->priority, 20);
}

// Test 2: Pop returns max and maintains heap property
TEST_F(IndexedHeapTest, PopMaintainsHeapProperty) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    nodes[3] = HeapNode();
    nodes[3].tok_ids = {5, 6};
    nodes[3].priority = 15;
    
    heap.push(&nodes[1]);
    heap.push(&nodes[2]);
    heap.push(&nodes[3]);
    
    HeapNode* first = heap.pop();
    EXPECT_EQ(first->priority, 20);
    
    HeapNode* second = heap.pop();
    EXPECT_EQ(second->priority, 15);
    
    HeapNode* third = heap.pop();
    EXPECT_EQ(third->priority, 10);
}

// Test 3: Update priority upward
TEST_F(IndexedHeapTest, UpdatePriorityUpward) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    nodes[3] = HeapNode();
    nodes[3].tok_ids = {5, 6};
    nodes[3].priority = 15;
    
    heap.push(&nodes[1]);
    heap.push(&nodes[2]);
    heap.push(&nodes[3]);
    
    heap.updatePriority(&nodes[1], 25);  // Increase 10 -> 25
    
    HeapNode* top = heap.top();
    EXPECT_EQ(top->tok_ids.first, 1);
    EXPECT_EQ(top->tok_ids.second, 2);
    EXPECT_EQ(top->priority, 25);
}

// Test 4: Update priority downward
TEST_F(IndexedHeapTest, UpdatePriorityDownward) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    nodes[3] = HeapNode();
    nodes[3].tok_ids = {5, 6};
    nodes[3].priority = 15;
    
    heap.push(&nodes[1]);
    heap.push(&nodes[2]);
    heap.push(&nodes[3]);
    
    heap.updatePriority(&nodes[2], 5);  // Decrease 20 -> 5
    
    HeapNode* top = heap.top();
    EXPECT_EQ(top->tok_ids.first, 5);
    EXPECT_EQ(top->tok_ids.second, 6);
    EXPECT_EQ(top->priority, 15);
}

// Test 5: Empty and size operations
TEST_F(IndexedHeapTest, EmptyAndSize) {
    EXPECT_TRUE(heap.empty());
    EXPECT_EQ(heap.size(), 0);
    
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    heap.push(&nodes[1]);
    EXPECT_FALSE(heap.empty());
    EXPECT_EQ(heap.size(), 1);
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    heap.push(&nodes[2]);
    EXPECT_EQ(heap.size(), 2);
    
    heap.pop();
    EXPECT_EQ(heap.size(), 1);
}

// Test 6: Update non-existent element (should be no-op)
TEST_F(IndexedHeapTest, UpdateNonExistent) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[999] = HeapNode();
    nodes[999].tok_ids = {99, 100};
    nodes[999].priority = 50;
    
    heap.push(&nodes[1]);
    
    EXPECT_NO_THROW(heap.updatePriority(&nodes[999], 50));
    EXPECT_EQ(heap.size(), 1);
}

// Test 7: Multiple updates on same element
TEST_F(IndexedHeapTest, MultipleUpdates) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    heap.push(&nodes[1]);
    heap.push(&nodes[2]);
    
    heap.updatePriority(&nodes[1], 15);
    heap.updatePriority(&nodes[1], 25);
    heap.updatePriority(&nodes[1], 5);
    
    HeapNode* top = heap.top();
    EXPECT_EQ(top->tok_ids.first, 3);
    EXPECT_EQ(top->tok_ids.second, 4);
    EXPECT_EQ(top->priority, 20);
}

// Test 8: Remove specific node
TEST_F(IndexedHeapTest, RemoveNode) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    nodes[2] = HeapNode();
    nodes[2].tok_ids = {3, 4};
    nodes[2].priority = 20;
    
    nodes[3] = HeapNode();
    nodes[3].tok_ids = {5, 6};
    nodes[3].priority = 15;
    
    heap.push(&nodes[1]);
    heap.push(&nodes[2]);
    heap.push(&nodes[3]);
    
    heap.remove(&nodes[2]);  // Remove the max
    EXPECT_EQ(heap.size(), 2);
    
    HeapNode* top = heap.top();
    EXPECT_EQ(top->tok_ids.first, 5);
    EXPECT_EQ(top->tok_ids.second, 6);
    EXPECT_EQ(top->priority, 15);
}

// Test 9: Push same node twice (should update, not duplicate)
TEST_F(IndexedHeapTest, PushDuplicate) {
    nodes[1] = HeapNode();
    nodes[1].tok_ids = {1, 2};
    nodes[1].priority = 10;
    
    heap.push(&nodes[1]);
    EXPECT_EQ(heap.size(), 1);
    
    nodes[1].priority = 20;
    heap.push(&nodes[1]);  // Should update position, not add new
    EXPECT_EQ(heap.size(), 1);
    
    HeapNode* top = heap.top();
    EXPECT_EQ(top->priority, 20);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
