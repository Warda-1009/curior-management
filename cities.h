#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "hashtable.h"

using namespace std;

class CityRegistry {
private:
    HashTable<string, int> name_to_id;
    string* id_to_name;
    int capacity;
    int city_count;
    static int next_id;


public:
  

    CityRegistry();
    ~CityRegistry();
    int add_city(string name);
    void resize(int new_cap);
    int add_city(int id, string name);
    int get_id(string name);
    string get_name(int id) const;

    friend ostream& operator<<(ostream& os, const CityRegistry& reg);
    friend istream& operator>>(istream& is, CityRegistry& reg);
};

extern CityRegistry global_cities;