#pragma once
#include <iostream>
#include <algorithm>

using namespace std;

template <typename T>
class Heap {
protected:
    T* arr;
    int capacity, current_size;


    virtual bool compare(T a, T b) = 0;

    void heapify_up(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (compare(arr[idx], arr[parent])) {
                swap(arr[idx], arr[parent]);
                idx = parent;
            }
            else break;
        }
    }

    void heapify_down(int idx) {
        int best = idx;
        int l = 2 * idx + 1;
        int r = 2 * idx + 2;

        if (l < current_size && compare(arr[l], arr[best])) best = l;
        if (r < current_size && compare(arr[r], arr[best])) best = r;

        if (best != idx) {
            swap(arr[idx], arr[best]);
            heapify_down(best);
        }
    }

    void resize() {
        capacity *= 2;
        T* new_arr = new T[capacity];
        for (int i = 0; i < current_size; i++) new_arr[i] = arr[i];
        delete[] arr;
        arr = new_arr;
    }

public:
    Heap(int cap = 50) : capacity(cap), current_size(0) {
        arr = new T[capacity];
    }

    virtual ~Heap() { delete[] arr; }

    void insert(T val) {
        if (current_size >= capacity) resize();
        arr[current_size] = val;
        heapify_up(current_size++);
    }

    T extract_root() {
        if (current_size <= 0) return T();
        T root = arr[0];
        arr[0] = arr[--current_size];
        if (current_size > 0) heapify_down(0);
        return root;
    }

    bool is_empty() const { return current_size == 0; }
    int size() const { return current_size; }
};