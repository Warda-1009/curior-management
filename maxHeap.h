#pragma once
#include "Heap.h"
using namespace std;

template <typename T>
class MaxHeap : public Heap<T> {
protected:

    bool compare(T a, T b) override {
        return *a > *b;
    }

public:
    MaxHeap(int cap = 50) : Heap<T>(cap) {}

    T extract_max() {
        return this->extract_root();
    }
};