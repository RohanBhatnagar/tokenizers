#include <doubly_linked_list.hpp>
#include <string>
#include <stdexcept>

// Constructor
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

// Destructor
template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    clear();
}

// Copy constructor
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList& other) 
    : head(nullptr), tail(nullptr), size_(0) {
    Node* current = other.head;
    while (current) {
        push_back(current->data);
        current = current->next;
    }
}

// Copy assignment operator
template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList& other) {
    if (this != &other) {
        clear();
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }
    return *this;
}

// Move constructor
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList&& other) noexcept
    : head(other.head), tail(other.tail), size_(other.size_) {
    other.head = nullptr;
    other.tail = nullptr;
    other.size_ = 0;
}

// Move assignment operator
template <typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(DoublyLinkedList&& other) noexcept {
    if (this != &other) {
        clear();
        head = other.head;
        tail = other.tail;
        size_ = other.size_;
        other.head = nullptr;
        other.tail = nullptr;
        other.size_ = 0;
    }
    return *this;
}

// Element access
template <typename T>
T& DoublyLinkedList<T>::front() {
    if (empty()) {
        throw std::out_of_range("DoublyLinkedList::front() called on empty list");
    }
    return head->data;
}

template <typename T>
const T& DoublyLinkedList<T>::front() const {
    if (empty()) {
        throw std::out_of_range("DoublyLinkedList::front() called on empty list");
    }
    return head->data;
}

template <typename T>
T& DoublyLinkedList<T>::back() {
    if (empty()) {
        throw std::out_of_range("DoublyLinkedList::back() called on empty list");
    }
    return tail->data;
}

template <typename T>
const T& DoublyLinkedList<T>::back() const {
    if (empty()) {
        throw std::out_of_range("DoublyLinkedList::back() called on empty list");
    }
    return tail->data;
}

// Modifiers
template <typename T>
void DoublyLinkedList<T>::push_front(const T& value) {
    Node* new_node = new Node(value);
    
    if (empty()) {
        head = tail = new_node;
    } else {
        new_node->next = head;
        head->prev = new_node;
        head = new_node;
    }
    
    ++size_;
}

template <typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
    Node* new_node = new Node(value);
    
    if (empty()) {
        head = tail = new_node;
    } else {
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;
    }
    
    ++size_;
}

template <typename T>
void DoublyLinkedList<T>::pop_front() {
    if (empty()) {
        throw std::out_of_range("DoublyLinkedList::pop_front() called on empty list");
    }
    
    Node* old_head = head;
    head = head->next;
    
    if (head) {
        head->prev = nullptr;
    } else {
        tail = nullptr;
    }
    
    delete old_head;
    --size_;
}

template <typename T>
void DoublyLinkedList<T>::pop_back() {
    if (empty()) {
        throw std::out_of_range("DoublyLinkedList::pop_back() called on empty list");
    }
    
    Node* old_tail = tail;
    tail = tail->prev;
    
    if (tail) {
        tail->next = nullptr;
    } else {
        head = nullptr;
    }
    
    delete old_tail;
    --size_;
}

template <typename T>
void DoublyLinkedList<T>::clear() {
    while (!empty()) {
        pop_front();
    }
}

// Insert at position
template <typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::insert(Iterator pos, const T& value) {
    // If inserting at the beginning
    if (pos == begin()) {
        push_front(value);
        return begin();
    }
    
    // If inserting at the end
    if (pos == end()) {
        push_back(value);
        return Iterator(tail);
    }
    
    // Insert in the middle
    Node* new_node = new Node(value);
    Node* current = pos.current;
    
    new_node->next = current;
    new_node->prev = current->prev;
    
    if (current->prev) {
        current->prev->next = new_node;
    }
    current->prev = new_node;
    
    ++size_;
    return Iterator(new_node);
}

// Erase at position
template <typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::erase(Iterator pos) {
    if (pos == end()) {
        throw std::out_of_range("DoublyLinkedList::erase() called with end iterator");
    }
    
    Node* to_delete = pos.current;
    Node* next_node = to_delete->next;
    
    // Update links
    if (to_delete->prev) {
        to_delete->prev->next = to_delete->next;
    } else {
        head = to_delete->next;
    }
    
    if (to_delete->next) {
        to_delete->next->prev = to_delete->prev;
    } else {
        tail = to_delete->prev;
    }
    
    delete to_delete;
    --size_;
    
    return Iterator(next_node);
}

// Explicit template instantiations for common types
template class DoublyLinkedList<int>;
template class DoublyLinkedList<double>;
template class DoublyLinkedList<float>;
template class DoublyLinkedList<char>;
template class DoublyLinkedList<std::string>;
