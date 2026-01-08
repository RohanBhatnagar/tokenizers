#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

// this was vibe coded. 

#include <cstddef>
#include <stdexcept>
#include <iterator>

template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        
        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
    };
    
    Node* head;
    Node* tail;
    size_t size_;
    
public:
    // Iterator class for range-based for loops
    class Iterator {
    private:
        Node* current;
        friend class DoublyLinkedList;
        
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator(Node* node) : current(node) {}
        
        reference operator*() const { return current->data; }
        pointer operator->() const { return &(current->data); }
        
        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        Iterator& operator--() {
            if (current) current = current->prev;
            return *this;
        }
        
        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
    };
    
    // Constructor
    DoublyLinkedList();
    
    // Destructor
    ~DoublyLinkedList();
    
    // Copy constructor
    DoublyLinkedList(const DoublyLinkedList& other);
    
    // Copy assignment operator
    DoublyLinkedList& operator=(const DoublyLinkedList& other);
    
    // Move constructor
    DoublyLinkedList(DoublyLinkedList&& other) noexcept;
    
    // Move assignment operator
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept;
    
    // Capacity
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    
    // Element access
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    
    // Modifiers
    void push_front(const T& value);
    void push_back(const T& value);
    void pop_front();
    void pop_back();
    void clear();
    
    // Insert/erase at position
    Iterator insert(Iterator pos, const T& value);
    Iterator erase(Iterator pos);
    
    // Iterators
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); }
};

#endif // DOUBLY_LINKED_LIST_HPP
