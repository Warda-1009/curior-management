#pragma once
#include "list.h"
#include "queue.h"
#include "maxHeap.h"
#include "stack.h"
#include <string>
#include <fstream>

class Rider; class Parcel;

struct Person { string name, contact, address; };

struct ActionLog {
    Parcel* p_ref;
    Rider* r_ref;
    int prev_city_id;
    string prev_status;
    int temp_r_id;

    ActionLog(Parcel* p = nullptr, Rider* r = nullptr, int city = -1, string status = "");


    void save(ofstream& os) const;
    void load(string line);
    friend ostream& operator<<(ostream& os, const ActionLog& node);
};

class Parcel {
public:
    static int next_id;
    int id, priority, source_id, dest_id, current_city_id, attempts, last_tick, temp_rider_id;
    double weight; string status; Person sender, receiver;
    Rider* assigned_rider; 
    List<int> planned_route;     
    List<ActionLog> history_log;

    void load(string line);
    Parcel(double w = 0, int p = 1);
    bool operator>(const Parcel& other) const;
    bool operator<(const Parcel& other) const;
    void save(ofstream& os) const;
    
    friend ostream& operator<<(ostream& os, const Parcel& p);
    friend istream& operator>>(istream& is, Parcel& p);
};

class Rider {
public:
    static int next_id;
    int id, current_city_id; double weight_capacity, current_load;
    int target_destination_id;
    List<int> planned_route;
    List<Parcel*> parcels;
    string name;    
    string contact;
    int get_next_city();
    Rider(double cap = 50.0, int city = 1);
    void save(ofstream& os) const;
    void load(string line);
    friend ostream& operator<<(ostream& out, const Rider& r);
    friend istream& operator>>(istream& is, Rider& r);
};

class Warehouse {
public:
    int city_id;
    Queue<Parcel*> pickup_queue; 
    MaxHeap<Parcel*> warehouse_sorting_heap; 
    Queue<Parcel*> transit_queue;
    Queue<Rider*> idle_riders_queue;      

    Warehouse(int id = -1);
};