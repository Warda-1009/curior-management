#include "admin.h"
#include "cities.h"
#include <iostream>
#include <string>
#include <chrono> 
#include <thread> 

using namespace std;


extern void pause();

void clear() {
    system("cls");
}

AdminControl::AdminControl(LogisticsEngine& e) : engine(e) {}

bool AdminControl::authenticate() {
    clear();
    string pass;
    cout << "\n\n\t\t\t\t/==========================================================\\" << endl;
    cout << "\t\t\t\t|                 ADMIN LOGIN DASHBOARD                     |" << endl;
    cout << "\t\t\t\t\\==========================================================/" << endl;
    cout << "\t\t\t\t  ACCESS LEVEL: ADMINISTRATOR" << endl;
    cout << "\t\t\t\t  ENTER PASSWORD: ";
    cin >> pass;
    cin.ignore(1000, '\n'); // Clear buffer

    if (pass == "admin123") {
        clear();
        cout << "\n\n\t\t       IDENTITY VERIFIED......." << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
        return true;
    }

    cout << "\n\t  \t ! AUTHENTICATION FAILED. TERMINATING..." << endl;
    pause();
    return false;
}

void AdminControl::run() {
    if (!authenticate()) return;

    int choice;
    while (true) {
        clear();
        cout << "\t\t\t\t/==========================================================\\" << endl;
        cout << "\t\t\t\t|               MAIN CONTROL DASHBOARD                     |" << endl;
        cout << "\t\t\t\t|----------------------------------------------------------|" << endl;
        cout << "\t\t\t\t|  1. NETWORK MANAGEMENT                                   |" << endl;
        cout << "\t\t\t\t|  2. RIDER REGISTRATION                                   |" << endl;
        cout << "\t\t\t\t|  3. SIMULATION ENGINE                                    |" << endl;
        cout << "\t\t\t\t|  4. SYSTEM ANALYSIS                                      |" << endl;
        cout << "\t\t\t\t|  5. DATABASE SYNC                                        |" << endl;
        cout << "\t\t\t\t|                                                          |" << endl;
        cout << "\t\t\t\t|  0. LOGOUT                                               |" << endl;
        cout << "\t\t\t\t\\==========================================================/" << endl;
        cout << "\t\t\t\t  SELECT MODULE: ";

        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
        cin.ignore(1000, '\n');

        if (choice == 0) break;
        clear();
        switch (choice) {
        case 1: manageMap(); break;
        case 2: manage_rider(); break;
        case 3: runSimulation(); break;
        case 4: systemAnalysis(); break;
        case 5:
            clear();
            cout << "\n\t [SYSTEM: Saving Registry and Topology to files..." << endl;
            engine.save_map();
            engine.save_all();
            cout << "\n\t >>> Database backup complete." << endl;
            pause();
            break;
        }
    }
}

void AdminControl::manageMap() {
    int c;
    while (true) {
        clear();
        cout << "\t\t\t\t/----------------------------------------------------------\\" << endl;
        cout << "\t\t\t\t|                     MAP  MANAGEMENT                      |" << endl;
        cout << "\t\t\t\t|----------------------------------------------------------|" << endl;
        cout << "\t\t\t\t|  1. Add New City Node                                    |" << endl;
        cout << "\t\t\t\t|  2. Link Cities (New Road)                               |" << endl;
        cout << "\t\t\t\t|  3. Toggle Road Blockade                                 |" << endl;
        cout << "\t\t\t\t|  4. Simulate Traffic Overload                            |" << endl;
        cout << "\t\t\t\t|  5. View Roadmap                                         |" << endl;
        cout << "\t\t\t\t|  0. Return to Main Dashboard                             |" << endl;
        cout << "\t\t\t\t\\----------------------------------------------------------/" << endl;
        cout << "\t\t\t\t  COMMAND: "; cin >> c; cin.ignore(1000, '\n');

        if (c == 0) break;
        if (c == 1) {
            string n; cout << "\n\t  City Name: "; getline(cin, n);
            engine.add_city(n);
            pause();
        }
        else if (c == 2) {
            string a, b; int d;
            cout << "\n\t  City A: "; getline(cin, a);
            cout << "\n\t  City B: "; getline(cin, b);
            cout << "\n\t  Distance (km): "; cin >> d; cin.ignore(1000, '\n');
            engine.add_road(a, b, d);
            pause();
        }
        else if (c == 3) {
            string a, b; bool s;
            cout << "\n\t  Target City A: "; getline(cin, a);
            cout << "\t  Target City B: "; getline(cin, b);
            cout << "\t  Block (1-Yes, 0-No): "; cin >> s; cin.ignore(1000, '\n');
            engine.get_map().toggle_road_block(global_cities.get_id(a), global_cities.get_id(b), s);
            cout << "\n\t  >> Road Block status updated." << endl;
            pause();
        }
        else if (c == 4) {
            string a, b;
            cout << "\n\t  Target City A: "; getline(cin, a);
            cout << "\t  Target City B: "; getline(cin, b);
            engine.simulate_road_overload(a, b);
            pause();
        }
        else if (c == 5) {
            engine.get_map().display();
            pause();
        }
    }
}

void AdminControl::runSimulation() {
    int c;
    while (true) {
        clear();
        cout << "\t\t\t\t/----------------------------------------------------------\\" << endl;
        cout << "\t\t\t\t|                   SIMULATION CONTROL                     |" << endl;
        cout << "\t\t\t\t|----------------------------------------------------------|" << endl;
        cout << "\t\t\t\t|  1. Assign Parcels                                       |" << endl;
        cout << "\t\t\t\t|  2. Process Manual Tick                                  |" << endl;
        cout << "\t\t\t\t|  3. START AUTO-SIMULATION                                |" << endl;
        cout << "\t\t\t\t|  4. Transactional Undo                                   |" << endl;
        cout << "\t\t\t\t|  5. Transactional Redo                                   |" << endl;
        cout << "\t\t\t\t|  0. Return to Main Dashboard                             |" << endl;
        cout << "\t\t\t\t\\----------------------------------------------------------/" << endl;
        cout << "\t\t\t\t  COMMAND: "; cin >> c; cin.ignore(1000, '\n');

        if (c == 0) break;
        if (c == 1) {
            cout << "\n\t SYSTEM: Sorting Parcels and matching..." << endl;
            engine.process_assignments();
            pause();
        }
        else if (c == 2) {
            engine.process_tick();
            pause();
        }
        else if (c == 3) {
            int t, s;
            cout << "\n\t  Total Ticks to run: "; cin >> t;
            cout << "\t  Delay between ticks (sec): "; cin >> s; cin.ignore(1000, '\n');
            engine.run_auto(t, s);
            pause();
        }
        else if (c == 4) {
            clear();
            while(true){
            cout << "\t\t\t\t/----------------------------------------------------------\\" << endl;
            cout << "\t\t\t\t|                     UNDO CONTROL                         |" << endl;
            cout << "\t\t\t\t|----------------------------------------------------------|" << endl;
            cout << "\t\t\t\t|  1. Undo System Action                                   |" << endl;
            cout << "\t\t\t\t|  2. Undo Parcel Action                                   |" << endl;
            cout << "\t\t\t\t|  0. Return                                               |" << endl;
            cout << "\t\t\t\t\\----------------------------------------------------------/" << endl;
            cout << "\t\t\t\t  COMMAND: "; cin >> c; cin.ignore(1000, '\n');
            if (c == 0) break;
            if(c==1)engine.undo_last_action();
            if (c == 2) { 
                int id; cout << "\n\t  Enter Tracking ID: "; cin >> id; cin.ignore(1000, '\n');
                engine.undo_parcel_action(id);
            }
            pause();
            }
        }
        else if (c == 5) {
            engine.redo_last_undone_action();
            pause();
        }
    }
}

void AdminControl::systemAnalysis() {
    int c;
    while (true) {
        clear();
        cout << "\t\t\t\t/----------------------------------------------------------\\" << endl;
        cout << "\t\t\t\t|                     ANALYSE SYSTEM                       |" << endl;
        cout << "\t\t\t\t|----------------------------------------------------------|" << endl;
        cout << "\t\t\t\t|  1. List All Registered Riders                           |" << endl;
        cout << "\t\t\t\t|  2. List All Registered Parcels                          |" << endl;
        cout << "\t\t\t\t|  3. Forensic Trace (Detailed Parcel History)             |" << endl;
        cout << "\t\t\t\t|  4. Scan for Missing/Delayed Parcels                     |" << endl;
        cout << "\t\t\t\t|  0. Return to Main Dashboard                             |" << endl;
        cout << "\t\t\t\t\\----------------------------------------------------------/" << endl;
        cout << "\t\t\t\t  COMMAND: "; cin >> c; cin.ignore(1000, '\n');
        if (c == 0) break;

        if (c == 1) {
            for (int i = 0; i < engine.get_riders().get_capacity(); i++) {
                Rider* r = engine.get_riders().get_value_at(i);
                if (r) cout << *r << endl;
            }
            pause();
        }
        else if (c == 2) {
            for (int i = 0; i < engine.get_parcels().get_capacity(); i++) {
                Parcel* p = engine.get_parcels().get_value_at(i);
                if (p) cout << *p << endl;
            }
            pause();
        }
        else if (c == 3) {
            int id; cout << "\n\t  Enter Tracking ID: "; cin >> id; cin.ignore(1000, '\n');
         
            engine.parcel_forensics(id);
            pause();
        }
        else if (c == 4) {
            engine.detect_missing_parcels();
            pause();
        }
    }
}

void AdminControl::manage_rider() {
    while (true) {
        clear();
        cout << "\t\t\t\t/----------------------------------------------------------\\" << endl;
        cout << "\t\t\t\t|               RIDER REGISTRY OPERATIONS                  |" << endl;
        cout << "\t\t\t\t|----------------------------------------------------------|" << endl;
        cout << "\t\t\t\t|  1. Enroll New Rider                                     |" << endl;
        cout << "\t\t\t\t|  0. Return to Main Dashboard                             |" << endl;
        cout << "\t\t\t\t\\----------------------------------------------------------/" << endl;
        int c; cout << "\t\t\t\t  COMMAND: "; cin >> c; cin.ignore(1000, '\n');
        clear();
        if (c == 0) break;
        else if (c == 1) { engine.register_new_rider(); pause(); }

    }
}