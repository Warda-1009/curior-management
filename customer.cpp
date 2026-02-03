#include "customer.h"
#include <iostream>
#include <chrono>
#include <thread>

void pause() {
    cout << "\n\t PRESS ENTER TO CONTINUE...";
    cin.get();
}
extern void clear();


CustomerPortal::CustomerPortal(LogisticsEngine& e) : engine(e) {}

void CustomerPortal::run() {
    int choice;
    while (true) {
        clear();
        cout << "\n\n\n\n\n\n\n\n\n\t\t\t\t\t/=======================================\\" << endl;
        cout << "\t\t\t\t\t|            CUSTOMER PORTAL            |" << endl;
        cout << "\t\t\t\t\t|---------------------------------------|" << endl;
        cout << "\t\t\t\t\t|  1. [SHIP]  Book a New Parcel         |" << endl;
        cout << "\t\t\t\t\t|  2. [TRACK] Live Parcel Tracking      |" << endl;
        cout << "\t\t\t\t\t|  0. [BACK]  Exit to Role Menu         |" << endl;
        cout << "\t\t\t\t\t\\=======================================/" << endl;
        cout << "\t\t\t\t\t\t  Choice: ";
        cin >> choice;

        if (choice == 0) break;
        if (choice == 1) {
            clear();
            engine.register_new_parcel();
            pause();
        }
        else if (choice == 2) {
            track_parcel();
            this_thread::sleep_for(chrono::milliseconds(1000));
            cout << "\n\t PRESS ENTER TO CONTINUE...";
            cin.get();
        }
    }
}

void CustomerPortal::track_parcel() {
    int id;
    cout << "\n\n\t\t\t\tENTER TRACKING ID: ";
    cin >> id;
    cin.ignore(1000, '\n');
    clear();
    cout<<"\n\n\t\t\tLOCATING PACKAGE........";
    this_thread::sleep_for(chrono::milliseconds(1000));

    Parcel* p = engine.get_parcels().search(id);
    if (p) {
        clear();
        cout << *p; 
        cout << "\t---PARCEL'S JOURNEY  ---\n";

        if (p->history_log.is_empty()) {
            cout << "\t! NO MOVEMENT DATA RECORDED YET.\n";
        }
        else {

            for (int i = 0; i < p->history_log.size(); i++) {
                this_thread::sleep_for(chrono::milliseconds(300));
                cout << "\t " << p->history_log.get_at(i) << endl;
            }
        }
    }
    else {
        cout << "\t! ERROR: INVALID TRACKING ID. RECORD NOT FOUND.\n";
    }
}