#pragma once
#include <iostream>

using namespace std;

template <typename T>
class Node {
public:
    T data;
    Node<T>* next; 
    Node<T>* prev; 

    Node(T val = T()) : data(val), next(nullptr), prev(nullptr) {}
};

template <typename T>
class List {
private:
    Node<T>* head; 
    Node<T>* tail; 
    int current_size;

public:
    List() : head(nullptr), tail(nullptr), current_size(0) {}

    void insert(T val) {
        Node<T>* new_node = new Node<T>(val); 
        if (!head) {
            head = tail = new_node;
        }
        else {
            tail->next = new_node;
            new_node->prev = tail;
            tail = new_node;
        }
        current_size++;
    }

    bool search(T val) {
        Node<T>* curr = head; 
        while (curr) {
            if (curr->data == val) return true;
            curr = curr->next;
        }
        return false;
    }

    void remove(T val) {
        Node<T>* curr = head; 
        while (curr) {
            if (curr->data == val) {
                if (curr->prev) curr->prev->next = curr->next;
                else head = curr->next;

                if (curr->next) curr->next->prev = curr->prev;
                else tail = curr->prev;

                delete curr;
                current_size--;
                return;
            }
            curr = curr->next;
        }
    }


    T get_at(int index) const {
        Node<T>* curr = head;
        for (int i = 0; i < index && curr != nullptr; i++) {
            curr = curr->next;
        }
        if (curr == nullptr) return T();
        return curr->data;
    }

    T& get_at_ref(int index) {
        Node<T>* curr = head;
        for (int i = 0; i < index && curr != nullptr; i++) {
            curr = curr->next;
        }
        if (curr == nullptr) {
            static T dummy;
            return dummy;
        }
        return curr->data;
    }

    int size() const { return current_size; }
    bool is_empty() const { return current_size == 0; }
    

    void remove_last() {
        if (!tail) return;
        Node<T>* temp = tail;
        if (head == tail) {
            head = tail = nullptr;
        }
        else {
            tail = tail->prev;
            tail->next = nullptr;
        }
        delete temp;
        current_size--;
    }

    T get_last() const {
        return (tail) ? tail->data : T();
    }
    void display() const {
        if (!head) {
            cout << "List is empty." << endl;
            return;
        }
        Node<T>* curr = head; 
        while (curr) {
            cout << curr->data;
            if (curr->next) cout << "  ";
            curr = curr->next;
        }
        cout << endl;
    }
};