#ifndef INDEXED_HEAP_HPP
#define INDEXED_HEAP_HPP

#include <vector>
#include <unordered_map>

struct HeapNode {
    std::pair<int, int> tok_ids;      
    int priority;
    
    HeapNode() : tok_ids(-1, -1), priority(0) {}
    HeapNode(int t, int p) : tok_ids(-1, -1), priority(p) {}
};

class IndexedHeap {
private:
    std::vector<HeapNode*> heap;  // Heap stores pointers to nodes
    std::unordered_map<HeapNode*, int> nodeToIndex;  // Track position of each node

    void bubbleUp(int idx);
    void bubbleDown(int idx);
    void swapNodes(int i, int j);

public:
    /**
     * Insert a node pointer into the heap
     * The node must remain valid for the lifetime of its presence in the heap
     */
    void push(HeapNode* node);

    /**
     * Update the priority of a node already in the heap
     * Call this after modifying the node's priority externally
     */
    void updatePriority(HeapNode* node, int newPriority);

    /**
     * Get pointer to the element with maximum priority
     */
    HeapNode* top() const;

    /**
     * Remove and return pointer to the element with maximum priority
     */
    HeapNode* pop();

    /**
     * Check if heap is empty
     */
    bool empty() const;

    /**
     * Get the current size of the heap
     */
    size_t size() const;

    /**
     * Remove a specific node from the heap
     */
    void remove(HeapNode* node);

    /**
     * Clear all elements from the heap
     */
    void clear();
};

#endif // INDEXED_HEAP_HPP
