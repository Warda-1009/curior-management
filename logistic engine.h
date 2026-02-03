#pragma once
#include "hashtable.h"
#include "entities.h"
#include "graph.h"
#include <chrono>
#include <thread>

class LogisticsEngine {
private:
    HashTable<int, Parcel*> parcel_reg;
    HashTable<int, Rider*> rider_reg;
    Graph Map;
    Warehouse** warehouses;
    int warehouse_capacity;
    int current_tick;
    Stack<int> undo_stack;
    Stack<ActionLog> redo_buffer;

public:
    LogisticsEngine(int init_cap = 50);
    ~LogisticsEngine();
    void ensure_warehouse(int id);
    void add_city(string n);
    void add_road(string a, string b, int d);
    void simulate_road_overload(string a, string b);
    void save_map(); void load_map();
    void save_all(); void load_all();
    void link_pointers();
    void process_warehouse();
    void register_new_parcel();
    void register_new_rider();
    void process_assignments();
    void process_tick();
    void run_auto(int ticks, int delay);
    void recalculate_rider_routes(Rider* r);
    void handle_parcel_delivery(Parcel* p, Rider* r);
    void process_transit();
    void detect_missing_parcels();
    void undo_last_action();
    void undo_parcel_action(int p_id);
    void redo_last_undone_action();
    void parcel_forensics(int p_id);
    Warehouse* get_warehouse(int id);
    HashTable<int, Parcel*>& get_parcels();
    HashTable<int, Rider*>& get_riders();
    Graph& get_map();
    int get_tick() const;
};