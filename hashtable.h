#pragma once
#include <iostream>
#include <string>

using namespace std;

enum BUCKET { EMPTY, OCCUPIED, DELETED };

template <typename K, typename V>
struct Entry {
    K key;
    V value;
    BUCKET state;
    // Explicitly set to empty to prevent memory corruption
    Entry() : state(EMPTY), value(V()) {}
};


 inline int get_raw_hash(int key) {
    return key;
}

 inline int get_raw_hash(const string& key) {
    unsigned long hash = 5381; // DJB2 Algorithm
    for (char c : key) {
        hash = ((hash << 5) + hash) + c;
    }
    return (int)hash;
}

template <typename K, typename V>
class HashTable {
private:
    Entry<K, V>* table;
    int capacity;
    int current_size;

    int hashFunction(K key) const {
        int h = get_raw_hash(key);
        int idx = h % capacity;
        return (idx < 0) ? (idx + capacity) : idx;
    }

    void insert_internal(K key, V value) {
        int index = hashFunction(key);
        while (table[index].state == OCCUPIED) {
            if (table[index].key == key) {
                table[index].value = value;
                return;
            }
            index = (index + 1) % capacity;
        }
        table[index].key = key;
        table[index].value = value;
        table[index].state = OCCUPIED;
        current_size++;
    }

    void resize() {
        int old_capacity = capacity;
        Entry<K, V>* old_table = table;

        capacity *= 2;
        table = new Entry<K, V>[capacity];
        current_size = 0;

        for (int i = 0; i < old_capacity; i++) {
            if (old_table[i].state == OCCUPIED) {
                insert_internal(old_table[i].key, old_table[i].value);
            }
        }
        delete[] old_table;
    }

public:
    HashTable(int initial_cap = 11) : capacity(initial_cap), current_size(0) {
        table = new Entry<K, V>[capacity];
    }

    ~HashTable() { delete[] table; }

    void insert(K key, V value) {

        if ((double)current_size / capacity >= 0.75) {
            resize();
        }
        insert_internal(key, value);
    }

    V search(K key) {
        int index = hashFunction(key);
        int start = index;
        while (table[index].state != EMPTY) {
            if (table[index].state == OCCUPIED && table[index].key == key)
                return table[index].value;
            index = (index + 1) % capacity;
            if (index == start) break;
        }
        return V();
    }

    void remove(K key) {
        int index = hashFunction(key);
        int start = index;
        while (table[index].state != EMPTY) {
            if (table[index].state == OCCUPIED && table[index].key == key) {
                table[index].state = DELETED;
                current_size--;
                return; 
            }
            index = (index + 1) % capacity;
            if (index == start) break;
        }
    }

    
    V get_value_at(int i) const {
        if (i >= 0 && i < capacity && table[i].state == OCCUPIED) {
            return table[i].value;
        }
        return V();
    }

    int get_capacity() const { return capacity; }
    int size() const { return current_size; }

    void display() {
        for (int i = 0; i < capacity; i++) {
            if (table[i].state == OCCUPIED) {
                cout << "Index [" << i << "]: Key=" << table[i].key << endl;
            }
        }
    }
};