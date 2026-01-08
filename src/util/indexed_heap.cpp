/*
 * IndexedHeap Implementation
 * A max heap with O(1) priority updates using node pointers
 * Nodes track their own heap position for efficient updates
 * THIS WAS VIBECODED. 
 */
#include "indexed_heap.hpp"
#include <algorithm>
#include <stdexcept>

void IndexedHeap::bubbleUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap[idx]->priority > heap[parent]->priority) {
            swapNodes(idx, parent);
            idx = parent;
        } else {
            break;
        }
    }
}

void IndexedHeap::bubbleDown(int idx) {
    int n = heap.size();
    while (true) {
        int largest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        if (left < n && heap[left]->priority > heap[largest]->priority) {
            largest = left;
        }
        if (right < n && heap[right]->priority > heap[largest]->priority) {
            largest = right;
        }

        if (largest != idx) {
            swapNodes(idx, largest);
            idx = largest;
        } else {
            break;
        }
    }
}

void IndexedHeap::swapNodes(int i, int j) {
    nodeToIndex[heap[i]] = j;
    nodeToIndex[heap[j]] = i;
    std::swap(heap[i], heap[j]);
}

void IndexedHeap::push(HeapNode* node) {
    if (nodeToIndex.find(node) != nodeToIndex.end()) {
        // Node is already in the heap, just update its position
        updatePriority(node, node->priority);
        return;
    }
    nodeToIndex[node] = heap.size();
    heap.push_back(node);
    bubbleUp(heap.size() - 1);
}

void IndexedHeap::updatePriority(HeapNode* node, int newPriority) {
    auto it = nodeToIndex.find(node);
    if (it == nodeToIndex.end()) {
        return;
    }

    int idx = it->second;
    int oldPriority = node->priority;
    node->priority = newPriority;

    if (newPriority > oldPriority) {
        bubbleUp(idx);
    } else if (newPriority < oldPriority) {
        bubbleDown(idx);
    }
    // If equal, no need to move
}

HeapNode* IndexedHeap::top() const {
    if (heap.empty()) {
        throw std::runtime_error("Heap is empty");
    }
    return heap[0];
}

HeapNode* IndexedHeap::pop() {
    if (heap.empty()) {
        throw std::runtime_error("Heap is empty");
    }

    HeapNode* result = heap[0];
    nodeToIndex.erase(result);  // Remove from index map

    if (heap.size() > 1) {
        heap[0] = heap.back();
        nodeToIndex[heap[0]] = 0;
        heap.pop_back();
        bubbleDown(0);
    } else {
        heap.pop_back();
    }

    return result;
}

void IndexedHeap::remove(HeapNode* node) {
    auto it = nodeToIndex.find(node);
    if (it == nodeToIndex.end()) {
        return;  // Not in heap
    }

    int idx = it->second;
    nodeToIndex.erase(it);

    if (idx == heap.size() - 1) {
        // Last element, just remove it
        heap.pop_back();
        return;
    }

    // Replace with last element and fix heap property
    heap[idx] = heap.back();
    nodeToIndex[heap[idx]] = idx;
    heap.pop_back();

    // Could need to go up or down
    if (idx > 0 && heap[idx]->priority > heap[(idx - 1) / 2]->priority) {
        bubbleUp(idx);
    } else {
        bubbleDown(idx);
    }
}

bool IndexedHeap::empty() const {
    return heap.empty();
}

size_t IndexedHeap::size() const {
    return heap.size();
}

void IndexedHeap::clear() {
    heap.clear();
    nodeToIndex.clear();
}