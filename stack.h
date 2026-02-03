#pragma once
#include <iostream>
using namespace std;
template <typename T>
class Stack {
private:
    T* arr;
    int top; 
    int capacity;

    void resize(int new_cap) {
        T* new_arr = new T[new_cap];

        int copy = top + 1;
        for (int i = 0; i < copy; i++) {
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr;
        capacity = new_cap;
    }

public:
    Stack(int initial_cap = 10) : capacity(initial_cap), top(-1) {
        arr = new T[capacity];
    }

    ~Stack() {
        delete[] arr;
    }

    void push(T val) {
       
        if (top == capacity - 1) {
            resize(capacity * 2);
        }
        arr[++top] = val;
    }

    T pop() {
        if (is_empty()) return T();

        T val = arr[top--];

       
        return val;
    }

    T get_top() const {
        if (is_empty()) return T();
        return arr[top];
    }

    bool is_empty() const {
        return top == -1;
    }

    int size() const {
        return top + 1;
    }

    void display() const {
        if (is_empty()) {
            cout << "Stack is Empty" << endl;
            return;
        }
        for (int i = top; i >= 0; i--) {
            cout << "[" << arr[i] << "]" << endl;
            if (i > 0) 
                cout << "       ";
        }
    }
};