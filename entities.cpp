#include "entities.h"
#include "cities.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

int Parcel::next_id = 1000;
int Rider::next_id = 5000;

ActionLog::ActionLog(Parcel* p, Rider* r, int c, string s) : p_ref(p), r_ref(r), prev_city_id(c), prev_status(s), temp_r_id(-1) {}

void ActionLog::save(ofstream& os) const { os << (r_ref ? r_ref->id : temp_r_id) << "|" << prev_city_id << "|" << prev_status; }
void ActionLog::load(string s) {
    stringstream ss(s); string t;
    getline(ss, t, '|'); temp_r_id = stoi(t);
    getline(ss, t, '|'); prev_city_id = stoi(t);
    getline(ss, prev_status, '|'); getline(ss, t); 
}

ostream& operator<<(ostream& os, const ActionLog& n) {
    os<<"\t>> Parcel was Last Updated to Status: " << n.prev_status << " at City: " << global_cities.get_name(n.prev_city_id); return os;
}

Parcel::Parcel(double w, int p) : weight(w), priority(p), assigned_rider(nullptr), attempts(0), last_tick(0), temp_rider_id(-1),current_city_id(-1),dest_id(-1), source_id(-1) { id = -1; }

ostream& operator<<(ostream& os, const Parcel& p) {
    string priority_lbl = (p.priority == 3) ? "OVERNIGHT" : (p.priority == 2 ? "2-DAY" : "NORMAL");

    os << "\n\t==============================================" << endl;
    os << "\t          PARCEL REPORT [ID: " << p.id << "]" << endl;
    os << "\t================================================" << endl;
    os << "\t Status:     *" << p.status << "*" << endl;
    os << "\t Service:    " << priority_lbl << " (Level " << p.priority << ")" << endl;
    os << "\t Weight:     " << p.weight << " kg" << endl;
    os << "\t Location:   " << global_cities.get_name(p.current_city_id) << endl;
    os << "\t Route:      " << global_cities.get_name(p.source_id) << " - "
        << global_cities.get_name(p.dest_id) << endl;
    os << "\t Attempts:   " << p.attempts << endl;
    os << "\t------------------------------------------------" << endl;
    os << "\t SENDER:     " << p.sender.name << " [" << p.sender.contact << "]" << endl;
    os << "\t RECEIVER:   " << p.receiver.name << " [" << p.receiver.contact << "]" << endl;
    os << "\t==============================================\n" << endl;
    return os;
}


istream& operator>>(istream& is, Parcel& p) {
    string t, cityName;
   

    
        p.id = Parcel::next_id++; 
        cout << "\n---== Register New Parcel for Shipment ==---\n";
        cout << ">> Assigned Tracking ID: " << p.id << endl;

        cout << "Enter Package Weight (kg): ";
        getline(is >> ws, t); p.weight = stod(t);

        cout << "Select Priority (3:Overnight, 2:2-Day, 1:Normal): ";
        getline(is, t); p.priority = stoi(t);

        
        while (true) {
            cout << "Enter Source City Name: ";
            getline(is, cityName);
            p.source_id = global_cities.get_id(cityName);
            if (p.source_id != -1) { break; }
            cout << "   Error: City name '" << cityName << "' not recognized. Try again.\n";
        }

        while (true) {
            cout << "Enter Destination City Name: ";
            getline(is, cityName);
            p.dest_id = global_cities.get_id(cityName);
            if (p.dest_id != -1) {  break; }
            cout << "   Error: City name '" << cityName << "' not recognized. Try again.\n";
        }

        p.current_city_id = p.source_id;
        p.status = "Accepted at Warehouse";

        cout << "\n[SENDER Details]\n";
        cout << " Sender Name:    "; getline(is, p.sender.name);
        cout << " Sender Contact: "; getline(is, p.sender.contact);
        cout << " Sender Address: "; getline(is, p.sender.address);
        cout << "\n[RECIEVER Details]\n";
        cout << " Recv Name:      "; getline(is, p.receiver.name);
        cout << " Recv Contact:   "; getline(is, p.receiver.contact);
        cout << " Recv Address:   "; getline(is, p.receiver.address);

        cout << "\n>>> Parcel " << p.id << " successfully booked to " << global_cities.get_name(p.dest_id) << "!" << endl;
    
    
    return is;
}


void Parcel::save(ofstream& os) const {
    
    os << id << ","
        << weight << ","
        << priority << ","
        << status << ","
        << source_id << ","
        << dest_id << ","
        << current_city_id << ","
        << attempts << ","    
        << last_tick << ",";   

    os << sender.name << ","
        << sender.contact << ","
        << sender.address << ",";

    os << receiver.name << ","
        << receiver.contact << ","
        << receiver.address << ",";

    if (assigned_rider) os << assigned_rider;

    os << -1;

    os << ";" << history_log.size();
    for (int i = 0; i < history_log.size(); i++) {
        os << ";";
        history_log.get_at(i).save(os);
    }

    os << endl;
}

bool Parcel::operator>(const Parcel& other) const {
    if (this->priority != other.priority) return this->priority > other.priority;
    if (this->weight != other.weight) return this->weight > other.weight; 
    return this->last_tick < other.last_tick; 
}

bool Parcel::operator<(const Parcel& other) const { return !(*this > other); }
int Rider::get_next_city() {
    if (planned_route.is_empty()) return -1;
    for (int i = 0; i < planned_route.size() - 1; i++) {
        if (planned_route.get_at(i) == this->current_city_id) {
            return planned_route.get_at(i + 1);
        }
    }
    return -1;
}


Rider::Rider(double cap, int city) : weight_capacity(cap), current_city_id(city), current_load(0) {
    id = next_id++;
    target_destination_id = -1;
    name = "Not Assigned";
    contact = "None";
}
ostream& operator<<(ostream& out, const Rider& r) {
    out << "\n\t\t\t\t/----------------------------------------------------------\\" << endl;
    out << "\t\t\t\t|                   RIDER DATA                             |" << endl;
    out << "\t\t\t\t|----------------------------------------------------------|" << endl;
    out << "\t\t\t\t   ID:      " << r.id << endl;
    out << "\t\t\t\t   NAME:    " << r.name << endl;
    out << "\t\t\t\t   CONTACT: " << r.contact << endl;
    out << "\t\t\t\t   CITY:    " << global_cities.get_name(r.current_city_id) << endl;
    out << "\t\t\t\t   LOAD:    " << (int)r.current_load << " / " << (int)r.weight_capacity << " kg" << endl;
    out << "\t\t\t\t   STATUS:  " << (r.parcels.is_empty() ? "IDLE (In Pool)" : "ACTIVE (En Route)") << endl;
    out << "\t\t\t\t\----------------------------------------------------------/" << endl;
    return out;
}


istream& operator>>(istream& is, Rider& r) {
    string t, c;

        cout << "\n\t/--------------------------------------------\\" << endl;
        cout << "\t|         RIDER ENROLLMENT                    |" << endl;
        cout << "\t\\--------------------------------------------/" << endl;

        cout << "\t Full Name:      "; getline(is >> ws, r.name);
        cout << "\t Contact No:     "; getline(is, r.contact);
        cout << "\t Max Load (kg):  "; getline(is, t);
        try { r.weight_capacity = stod(t); }
        catch (...) { r.weight_capacity = 50.0; }

        while (true) {
            cout << "\t Assigned City:  "; getline(is, c);
            r.current_city_id = global_cities.get_id(c);
            if (r.current_city_id != -1) break;
            cout << "\t ! Error: City not found. Try again." << endl;
        }
        cout << "\t >>> RIder " << r.name << " registered with ID: " << r.id << endl;
    
    return is;
}
void Rider::save(ofstream& os) const {
    os << id << "," << name << "," << contact << ","
        << weight_capacity << "," << current_load << ","
        << current_city_id << "," << target_destination_id << endl;
}

void Rider::load(string line) {
    if (line.empty()) return;
    stringstream ss(line);
    string t;

    getline(ss, t, ','); id = stoi(t);
    getline(ss, name, ',');
    getline(ss, contact, ',');
    getline(ss, t, ','); weight_capacity = stod(t);
    getline(ss, t, ','); current_load = stod(t);
    getline(ss, t, ','); current_city_id = stoi(t);
    getline(ss, t);      target_destination_id = stoi(t);
}

void Parcel::load(string line) {
    if (line.empty()) return;
    stringstream ss(line);
    string t;

    
    getline(ss, t, ','); id = stoi(t);
    getline(ss, t, ','); weight = stod(t);
    getline(ss, t, ','); priority = stoi(t);
    getline(ss, status, ',');
    getline(ss, t, ','); source_id = stoi(t);
    getline(ss, t, ','); dest_id = stoi(t);
    getline(ss, t, ','); current_city_id = stoi(t);
    getline(ss, t, ','); attempts = stoi(t);
    getline(ss, t, ','); last_tick = stoi(t);

  
    getline(ss, sender.name, ',');
    getline(ss, sender.contact, ',');
    getline(ss, sender.address, ',');
    getline(ss, receiver.name, ',');
    getline(ss, receiver.contact, ',');
    getline(ss, receiver.address, ',');


    getline(ss, t, ',');
    temp_rider_id = stoi(t);

    if (getline(ss, t, ';')) {
        int h_size = stoi(t);
        for (int i = 0; i < h_size; i++) {
            string data;
            if (i < h_size - 1) getline(ss, data, ';');
            else getline(ss, data);

            ActionLog log;
            log.load(data); 
            log.p_ref = this;
            history_log.insert(log);
        }
    }
}


Warehouse::Warehouse(int id) : city_id(id), pickup_queue(100), transit_queue(100), warehouse_sorting_heap(100), idle_riders_queue(100) {}