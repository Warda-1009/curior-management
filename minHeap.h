#pragma once
#include "Heap.h"
using namespace std;

template <typename T>
class MinHeap : public Heap<T> {
protected:

    bool compare(T a, T b) override {
        return a < b;
    }

public:
    MinHeap(int cap = 50) : Heap<T>(cap) {}

    T extract_min() {
        return this->extract_root();
    }
};