#include "logistic engine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

using namespace std;



HashTable<int, Parcel*>& LogisticsEngine::get_parcels() { return parcel_reg; }
HashTable<int, Rider*>& LogisticsEngine::get_riders() { return rider_reg; }
Graph& LogisticsEngine::get_map() { return Map; }
int LogisticsEngine::get_tick() const { return current_tick; }


void LogisticsEngine::add_city(string name) {
    int id = global_cities.add_city(name);

    if (id == -1) {
        return;
    }

    if (id >= 0 && id < 100) {
        if (warehouses[id] != nullptr) {
            delete warehouses[id];
        }

        // Build the new warehouse
        warehouses[id] = new Warehouse(id);

        cout << "\t>> Warehouse established in " << name << " (ID: " << id << ")" << endl;
    }
    else {
        cout << "\t ! Error: City '" << name << "' registered, but the system has no more warehouse slots (Limit 100)." << endl;
    }
}

void LogisticsEngine::add_road(string a, string b, int d) {
   
    int u = global_cities.get_id(a);
    int v = global_cities.get_id(b);

    if (u == -1 || v == -1) {
        cout << "\n\t\t\t! ERROR: Road creation failed." << endl;
        if (u == -1) cout << "\n\n\t\t\t  City '" << a << "' not found in Table" << endl;
        if (v == -1) cout << "\n\n\t\t\t  City '" << b << "' not found in Table" << endl;
        return; 
    }

    Map.add_road(u, v, d);
    cout << "\t>> Road established between " << a << " and " << b << endl;
}

Warehouse* LogisticsEngine::get_warehouse(int id) {
    return (id >= 0 && id < warehouse_capacity) ? warehouses[id] : nullptr;
}

void LogisticsEngine::save_map() {
    ofstream c_out("cities.csv");
    if (c_out) { c_out << global_cities; c_out.close(); }
    ofstream r_out("roads.csv");
    if (r_out) { r_out << Map; r_out.close(); }
    cout << ">> Map saved successfully." << endl;
}
void LogisticsEngine::load_map() {
    
    ifstream c_in("cities.csv");
    if (c_in.is_open()) {
        string line;
        while (getline(c_in, line)) {
            if (line.empty()) continue; 
            stringstream ss(line);
            string id_s, name;
            if (getline(ss, id_s, ',') && getline(ss, name)) {
                try {
                    int id = stoi(id_s);
                    if (id >= 0 && id < 2000) global_cities.add_city(id, name); 
                }
                catch (...) { continue; } 
            }
        }
        c_in.close();
    }
    else {
        cout << ">>  No city database found. Starting with empty map." << endl;
    }

    ifstream r_in("roads.csv");
    if (r_in.is_open()) {
        string line;
        while (getline(r_in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string u_s, v_s, w_s, b_s;
            try {
                if (getline(ss, u_s, ',') && getline(ss, v_s, ',') &&
                    getline(ss, w_s, ',') && getline(ss, b_s)) {
                    int u = stoi(u_s);
                    int v = stoi(v_s);
                    int w = stoi(w_s);
                    bool b = (stoi(b_s) == 1);

                    if (u >= 0 && v >= 0 && u < 1000 && v < 1000) {
                        Map.add_road(u, v, w);
                        if (b) Map.toggle_road_block(u, v, true);
                    }
                }
            }
            catch (...) { continue; }
        }
        r_in.close();
    }

    for (int i = 0; i < 100; i++) {
        string cityName = global_cities.get_name(i);

        if (cityName != "Invalid ID" && cityName != "Invalid City" && cityName != "") {
            if (i < warehouse_capacity && warehouses[i] == nullptr) {
                warehouses[i] = new Warehouse(i);
            }
        }
    }
}

void LogisticsEngine::process_warehouse() {
    cout << "\n\t>>WAREHOUSE: Sorting incoming parcels from pickup queues..." << endl;

    for (int i = 0; i < 100; i++) {
        Warehouse* wh = get_warehouse(i);
        if (!wh || wh->pickup_queue.is_empty()) continue;

        int count = 0;
        while (!wh->pickup_queue.is_empty()) {
            Parcel* p = wh->pickup_queue.dequeue();

            // Move to Priority Heap
            wh->warehouse_sorting_heap.insert(p);
            p->status = "In Warehouse";
            p->history_log.insert(ActionLog(p, nullptr, i, "SORTED"));
            count++;
        }
        cout << "\t City " << global_cities.get_name(i) << ": Processed " << count << " items." << endl;
    }
}
void LogisticsEngine::register_new_parcel() {
    Parcel* new_p = new Parcel();

    if (!(cin >> *new_p)) {
        cout << "\t! Parcel Registration for Shipment failed." << endl;
        delete new_p;
        return;
    }
    new_p->last_tick = this->current_tick;
        parcel_reg.insert(new_p->id, new_p);
        Warehouse* wh = get_warehouse(new_p->source_id);

        if (wh) {
            wh->pickup_queue.enqueue(new_p);
            new_p->status = " Loading AT Warehouse";
            new_p->history_log.insert(ActionLog(new_p, nullptr, new_p->source_id, "ARRIVED_AT_WAREHOUSE"));
            undo_stack.push(new_p->id);
            cout << "\t Parcel " << new_p->id << " is waiting in the Pickup Queue." << endl;
        }
   
    else {
        cout << "\t! ERROR: WareHouse missing in city " << new_p->source_id << endl;
    }
}

void LogisticsEngine::register_new_rider() {
    Rider* new_r = new Rider();

    if (cin >> *new_r) {
        rider_reg.insert(new_r->id, new_r);

        Warehouse* wh = get_warehouse(new_r->current_city_id);
        if (wh != nullptr) {
            wh->idle_riders_queue.enqueue(new_r);
            cout << "\tSUCCESS: Rider " << new_r->id << " deployed to "
                << global_cities.get_name(new_r->current_city_id) << "." << endl;
        }
    }
    else {
        delete new_r;
    }
}


void LogisticsEngine::process_tick() {

    process_transit();
    current_tick++;
    cout << "\n>>>>> \t\t\t TICK: " << current_tick << " <<<" << endl;
    for (int i = 0; i < rider_reg.get_capacity(); i++) {
        Rider* r = rider_reg.get_value_at(i);
        if (!r || r->parcels.is_empty()) continue;

        int next = r->get_next_city();
        if (next == -1) continue;

        if (Map.is_road_blocked(r->current_city_id, next)) {
            cout << "\tDETOUR: Road Blocked! Recalculating path for Rider " << r->id << endl;
            recalculate_rider_routes(r);
            continue;
        }

        int old_city = r->current_city_id;
        r->current_city_id = next;
        cout << "\t>> MOVING: Rider " << r->id << ": " << global_cities.get_name(old_city) << " ---> " << global_cities.get_name(next) << endl;

        for (int j = 0; j < r->parcels.size(); j++) {
            Parcel* p = r->parcels.get_at(j);
            p->current_city_id = r->current_city_id;
            p->history_log.insert(ActionLog(p, r, old_city, "IN_TRANSIT"));
            undo_stack.push(p->id);
            if (p->current_city_id == p->dest_id) { handle_parcel_delivery(p, r); j--; }
        }

        if (r->parcels.is_empty()) {
            cout << "\t>> Rider " << r->id << " has delivered all the parcels" << endl;
            r->target_destination_id = -1;
            Warehouse* wh = get_warehouse(r->current_city_id);
            if (wh) wh->idle_riders_queue.enqueue(r);
        }
    }
    detect_missing_parcels();
}

void LogisticsEngine::recalculate_rider_routes(Rider* r) {
    r->planned_route = List<int>();
    Map.calculate_shortest_path(r->current_city_id, r->target_destination_id, r->planned_route);
    for (int i = 0; i < r->parcels.size(); i++) {
        r->parcels.get_at(i)->planned_route = r->planned_route;
        r->parcels.get_at(i)->history_log.insert(ActionLog(r->parcels.get_at(i), r, r->current_city_id, "DETOUR_APPLIED"));
    }
}

void LogisticsEngine::handle_parcel_delivery(Parcel* p, Rider* r) {
    if (rand() % 100 < 80) {
        p->status = "Delivered";
        p->history_log.insert(ActionLog(p, r, p->current_city_id, "DELIVERED"));
        undo_stack.push(p->id);
        cout << "\n\t*************************************************" << endl;
        cout << "\t   PARCEL " << p->id << " SUCCESSFULLY DELIVERED!" << endl;
        cout << "\t   To Receiver: \"" << p->receiver.name << "\"" << endl;
        cout << "\t   At Address:  \"" << p->receiver.address << "\"" << endl;
        cout << "\t   By Sender:   \"" << p->sender.name << "\"" << endl;
        cout << "\t*************************************************\n" << endl;
        r->parcels.remove(p);
        r->current_load -= p->weight;
    }
    else {
        p->attempts++;
        cout << "\t! Delivery attempt " << p->attempts << " for Parcel " << p->id << endl;
        if (p->attempts >= 3) {
            p->status = "Return to Sender";
            r->target_destination_id = p->source_id;
            Map.calculate_shortest_path(p->current_city_id, p->source_id, r->planned_route);
        }
    }
}


LogisticsEngine::LogisticsEngine(int cap)
    : parcel_reg(101),
    rider_reg(53),
    Map(50),           
    warehouse_capacity(cap > 0 ? cap : 100),
    current_tick(0)
{
    this->warehouses = new Warehouse * [this->warehouse_capacity];

    for (int i = 0; i < this->warehouse_capacity; i++) {
        this->warehouses[i] = nullptr;
    }
}
LogisticsEngine::~LogisticsEngine() {
    for (int i = 0; i < warehouse_capacity; i++) if (warehouses[i]) delete warehouses[i];
    delete[] warehouses;
}

void LogisticsEngine::ensure_warehouse(int id) {
    if (id >= warehouse_capacity) {
        int old_cap = warehouse_capacity;
        while (id >= warehouse_capacity) warehouse_capacity *= 2;
        Warehouse** new_w = new Warehouse * [warehouse_capacity];
        for (int i = 0; i < old_cap; i++) new_w[i] = warehouses[i];
        for (int i = old_cap; i < warehouse_capacity; i++) new_w[i] = nullptr;
        delete[] warehouses;
        warehouses = new_w;
    }
    if (!warehouses[id]) warehouses[id] = new Warehouse(id);
}


void LogisticsEngine::process_assignments() {
    process_warehouse();
    for (int i = 0; i < 100; i++) {
        Warehouse* wh = get_warehouse(i);
        if (!wh || wh->warehouse_sorting_heap.is_empty() || wh->idle_riders_queue.is_empty()) continue;

        while (!wh->idle_riders_queue.is_empty()) {
            Rider* r = wh->idle_riders_queue.get_front();
            bool loaded_anything = false;
            List<Parcel*> rejected;

            while (!wh->warehouse_sorting_heap.is_empty()) {
                Parcel* p = wh->warehouse_sorting_heap.extract_max();

                if (r->parcels.is_empty()) {
                    r->target_destination_id = p->dest_id;
                    Map.calculate_shortest_path(r->current_city_id, r->target_destination_id, r->planned_route);
                }

                if ((r->current_load + p->weight <= r->weight_capacity) && (p->dest_id == r->target_destination_id)) {
                    
                    p->assigned_rider = r;
                    p->status = "In Transit Queue"; 
                    wh->transit_queue.enqueue(p);  

                    r->current_load += p->weight;
                    loaded_anything = true;
                    cout << "\tASSIGNED: Parcel " << p->id << " moved to Transit Queue for Rider " << r->id << endl;
                }
                else {
                    rejected.insert(p);
                }
            }
            for (int k = 0; k < rejected.size(); k++) wh->warehouse_sorting_heap.insert(rejected.get_at(k));

            if (loaded_anything) {
                wh->idle_riders_queue.dequeue(); 
            }
            else break;
        }
    }
}
void LogisticsEngine::process_transit() {
    for (int i = 0; i < 100; i++) {
        Warehouse* wh = get_warehouse(i);
        if (!wh || wh->transit_queue.is_empty()) continue;

        while (!wh->transit_queue.is_empty()) {
            Parcel* p = wh->transit_queue.dequeue();
            Rider* r = p->assigned_rider;

            if (r) {
                p->status = "En Route"; 
                r->parcels.insert(p);
                p->history_log.insert(ActionLog(p, r, i, "DEPARTED"));
                cout << "\tDEPARTED: Parcel " << p->id << " has left the Transit Queue on Rider " << r->id << endl;
                undo_stack.push(p->id);
            }
        }
    }
}

void LogisticsEngine::detect_missing_parcels() {
    bool found = false;
    int threshold = 5; 

    for (int i = 0; i < parcel_reg.get_capacity(); i++) {
        Parcel* p = parcel_reg.get_value_at(i);

        if (p && p->status != "Delivered") {
            int tick = current_tick - p->last_tick;

            if (tick >= threshold) {
                found = true;
                cout << "\t ! ALERT: Parcel " << p->id << " is STAGNANT." << endl;
                cout << "\t     Current Status: " << p->status << endl;
                cout << "\t     Last Activity:  " << tick << " ticks ago at "<< global_cities.get_name(p->current_city_id) << endl;
                cout << "\t     Action needed: Assign Rider or Unblock Road." << endl;
                
            }
        }
    }

    if (!found) {
        cout << "\t >> No delays detected. All Parcels are on schedule." << endl;
    }
}

void LogisticsEngine::simulate_road_overload(string a, string b) {
    int u = global_cities.get_id(a); int v = global_cities.get_id(b);
    for (int i = 0; i < Map.adj_list[u].size(); i++) {
        Edge& e = Map.adj_list[u].get_at_ref(i);
        if (e.to_city == v) { e.weight *= 5; cout << ">> Road " << a << "-" << b << " Overloaded." << endl; }
    }
}

void LogisticsEngine::run_auto(int num_ticks, int delay) {
    for (int i = 0; i < num_ticks; i++) {
        process_assignments();
        process_tick();
        this_thread::sleep_for(chrono::seconds(delay));
    }
}

void LogisticsEngine::save_all() {
    ofstream rd("riders.csv"), pr("parcels.csv");
    for (int i = 0; i < rider_reg.get_capacity(); i++) { Rider* r = rider_reg.get_value_at(i); if (r) r->save(rd); }
    for (int i = 0; i < parcel_reg.get_capacity(); i++) { Parcel* p = parcel_reg.get_value_at(i); if (p) p->save(pr); }
}
void LogisticsEngine::load_all() {
    string line;
    int mr = 5000; 
    int mp = 1000; 

    ifstream rd_file("riders.csv");
    if (rd_file.is_open()) {
        while (getline(rd_file, line)) {
            if (line.empty()) continue;

            Rider* r = new Rider();
            try {

                rider_reg.insert(r->id, r);

                Warehouse* wh = get_warehouse(r->current_city_id);
                if (wh != nullptr) {
                    wh->idle_riders_queue.enqueue(r);
                }

                if (r->id > mr) mr = r->id;
            }
            catch (...) {
                delete r;
            }
        }
        rd_file.close();
    }
    Rider::next_id = mr + 1;

    ifstream pr_file("parcels.csv");
    if (pr_file.is_open()) {
        while (getline(pr_file, line)) {
            if (line.empty()) continue;

            Parcel* p = new Parcel();
            try {
                p->load(line); 

                
                parcel_reg.insert(p->id, p);

              
                if (p->status == "In Warehouse" || p->status == "Accepted at Warehouse" || p->status == "Pending") {
                    Warehouse* wh = get_warehouse(p->current_city_id);
                    if (wh != nullptr) {
                        wh->warehouse_sorting_heap.insert(p);
                    }
                }

              
                if (p->id > mp) mp = p->id;
            }
            catch (...) {
                delete p;
            }
        }
        pr_file.close();
    }
   
    Parcel::next_id = mp + 1;

    this->link_pointers();

    cout << ">> SYSTEM DATA RESTORED. >> Next IDs: Parcel " << Parcel::next_id
        << ", Rider " << Rider::next_id << "" << endl;
}

void LogisticsEngine::redo_last_undone_action() {
    if (redo_buffer.is_empty()) { cout << "\t! Nothing to redo." << endl; return; }
    ActionLog snap = redo_buffer.pop();
    Parcel* p = snap.p_ref;
    if (p) {
        if (snap.r_ref) {
            Rider* r = snap.r_ref;
            p->assigned_rider = r; r->parcels.insert(p); r->current_load += p->weight;
            Warehouse* wh = get_warehouse(r->current_city_id);
            if (wh) {
                int sz = wh->idle_riders_queue.size();
                for (int i = 0; i < sz; i++) {
                    Rider* tr = wh->idle_riders_queue.dequeue();
                    if (tr->id != r->id) wh->idle_riders_queue.enqueue(tr);
                }
            }
        }
        p->status = snap.prev_status;
        p->current_city_id = snap.prev_city_id;
        p->history_log.insert(snap);
        undo_stack.push(p->id);
        cout << "\t>> Redo completed: Parcel #" << p->id << " restored." << endl;
    }
}
void LogisticsEngine::parcel_forensics(int p_id) {
   
    Parcel* p = parcel_reg.search(p_id);

    if (p == nullptr) {
        cout << "! Error: Parcel ID " << p_id << " does not exist in the database" << endl;
        return;
    }

    cout << "\n\t/---------------------------------------\\" << endl;
    cout << "\t|    TRACKING : PARCEL #" << p_id << "       |" << endl;
    cout << "\t\\---------------------------------------/" << endl;

    if (p->history_log.is_empty()) {
        cout << "\t  No movement history recorded yet." << endl;
    }
    else {
        for (int i = 0; i < p->history_log.size(); i++) {
        
            ActionLog log = p->history_log.get_at(i);

            cout << "\t  Step " << i + 1 << ": ";
            cout << " [" << global_cities.get_name(log.prev_city_id) << "] -> ";
            cout << "Action: " << log.prev_status;

            if (log.r_ref) cout << " (by Rider " << log.r_ref->id << ")";
            cout << "\nRider Not Assigned Yet";
            cout << endl;
          
        }
    }
    cout << "\t\\---------------------------------------/\n" << endl;
}
void LogisticsEngine::undo_last_action() {
    if (undo_stack.is_empty()) { cout << "\t! No actions to undo." << endl; return; }
    int p_id = undo_stack.pop();
    undo_parcel_action(p_id);
}

void LogisticsEngine::undo_parcel_action(int p_id) {
    Parcel* p = parcel_reg.search(p_id);
    if (!p || p->history_log.is_empty()) return;
    ActionLog redo_snap(p, p->assigned_rider, p->current_city_id, p->status);
    redo_buffer.push(redo_snap);
    ActionLog last = p->history_log.get_last();
    cout << "\tUNDO: BackTracking Parcel " << p_id << " to " << last.prev_status << endl;
    p->status = last.prev_status;
    p->current_city_id = last.prev_city_id;
    if (p->assigned_rider) {
        Rider* r = p->assigned_rider;
        r->parcels.remove(p);
        r->current_load -= p->weight;
        if (last.prev_status == "In Warehouse") {
            p->assigned_rider = nullptr;
            get_warehouse(p->current_city_id)->idle_riders_queue.enqueue(r);
            get_warehouse(p->current_city_id)->warehouse_sorting_heap.insert(p);
        }
    }
    p->history_log.remove_last();
}

void LogisticsEngine::link_pointers() {
    for (int i = 0; i < parcel_reg.get_capacity(); i++) {
        Parcel* p = parcel_reg.get_value_at(i);
        if (!p) continue;
        if (p->temp_rider_id != -1) {
            p->assigned_rider = rider_reg.search(p->temp_rider_id);
            if (p->assigned_rider) p->assigned_rider->parcels.insert(p);
        }
        for (int j = 0; j < p->history_log.size(); j++) {
            ActionLog& al = p->history_log.get_at_ref(j);
            al.p_ref = p;
            if (al.temp_r_id != -1) al.r_ref = rider_reg.search(al.temp_r_id);
        }
    }
}