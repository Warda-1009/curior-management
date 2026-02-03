#pragma once
#include <iostream>
using namespace std;
template <typename T>
class Queue {
private:
    T* arr;
    int front;
    int rear;
    int capacity;
    int current_size;

    void resize() {
        int old_capacity = capacity;
        T* old_arr = arr;

        capacity *= 2;
        arr = new T[capacity];

        for (int i = 0; i < current_size; i++) {
            arr[i] = old_arr[(front + i) % old_capacity];
        }

        front = 0;
        rear = current_size - 1;
        delete[] old_arr;

        cout << "Queue resized to capacity: " << capacity << endl;
    }

public:
    Queue(int initial_cap = 10) : capacity(initial_cap), current_size(0), front(0), rear(-1) {
        arr = new T[capacity];
    }

    ~Queue() {
        delete[] arr;
    }

    void enqueue(T item) {
        if (current_size == capacity) {
            resize();
        }

        rear = (rear + 1) % capacity;
        arr[rear] = item;
        current_size++;
    }

    T dequeue() {
        if (is_empty()) {
            return T(); 
        }

        T item = arr[front];
       
        front = (front + 1) % capacity;
        current_size--;
        return item;
    }

    T get_front() const {
        if (is_empty()) return T();
        return arr[front];
    }

    bool is_empty() const {
        return current_size == 0;
    }

    int size() const {
        return current_size;
    }

    void display() const {
        if (is_empty()) {
            cout << "Queue is empty." << endl;
            return;
        }

        for (int i = 0; i < current_size; i++) {
            int index = (front + i) % capacity;
            cout  << arr[index] ;
            if (i < current_size - 1) cout << " - ";
        }
    }
};