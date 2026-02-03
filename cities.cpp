#include "cities.h"
#include <iostream>
using namespace std;

int CityRegistry::next_id = 1;

CityRegistry::CityRegistry() : capacity(100), city_count(0) {

    id_to_name = new string[capacity];

    for (int i = 0; i < capacity; i++) {
        id_to_name[i] = "Invalid City";
    }
}

CityRegistry:: ~CityRegistry() {
    delete[] id_to_name;
}

int  CityRegistry::add_city(string name) {
    if (next_id >= capacity) {
        resize(capacity * 2);
    }

    int assigned_id = next_id++;
    name_to_id.insert(name, assigned_id);
    id_to_name[assigned_id] = name;
    city_count++;
    return assigned_id;
}


void CityRegistry:: resize(int new_cap) {
    if (new_cap <= capacity || new_cap > 5000) return;

    string* new_arr = new string[new_cap];
    for (int i = 0; i < capacity; i++) new_arr[i] = id_to_name[i];
    for (int i = capacity; i < new_cap; i++) new_arr[i] = "Invalid City";

    delete[] id_to_name;
    id_to_name = new_arr;
    capacity = new_cap;
}
int CityRegistry:: add_city(int id, string name) {
    if (id < 0 || id > 1000) return -1;
    if (get_id(name) != -1) {
        cout << "\t ! Error: City '" << name << "' is already registered with ID " << get_id(name) << "." << endl;
        return -1; 
    }
    while (id >= capacity) {
        resize(capacity * 2);
    }

    name_to_id.insert(name, id);
    id_to_name[id] = name;
    if (id >= next_id) next_id = id + 1;
}

int CityRegistry:: get_id(string name) {
    int id = name_to_id.search(name);
    if (id == 0 && name != id_to_name[0]) return -1;
    return id;
}

string CityRegistry:: get_name(int id) const {
    if (id >= 0 && id < 100) return id_to_name[id];
    return "Invalid ID";
}


 ostream& operator<<(ostream& os, const CityRegistry& reg) {
    for (int i = 0; i < 100; i++) {
        if (reg.id_to_name[i] != "Invalid City") {
            os << i << "," << reg.id_to_name[i] << endl;
        }
    }
    return os;
}

istream& operator>>(istream& is, CityRegistry& reg) {
    string line;
    while (getline(is, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id_str, name;
        if (getline(ss, id_str, ',') && getline(ss, name)) {
            reg.add_city(stoi(id_str), name);
        }
    }
    return is;
}
CityRegistry global_cities;

