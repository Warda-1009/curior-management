#include "logistic engine.h"
#include "admin.h"
#include "customer.h"
#include <chrono>
#include <thread>
void welcome() {
    cout << "\n\n\n\n\n\n\n\n\n\t\t\t\t\t/==============================================\\" << endl;
    cout << "\t\t\t\t\t|                  WELCOME TO                  |" << endl;
    cout << "\t\t\t\t\t|              SWIFT EX LOGISTICS              |" << endl;
    cout << "\t\t\t\t\t|==============================================|" << endl;
    this_thread::sleep_for(chrono::milliseconds(1500));
}
extern void clear();
extern void pause();
int main() {

  
        LogisticsEngine engine(100);

  
    clear();
    cout << "\tSYSTEM STARTING.......";
    this_thread::sleep_for(chrono::milliseconds(1500));
    cout << endl;
    engine.load_map();
    engine.load_all();
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "\t DATA RECOVERY SUCCESSFUL." << endl;
    this_thread::sleep_for(chrono::milliseconds(800));
    clear();
    AdminControl admin(engine);
    CustomerPortal customer(engine);
    welcome();
    int mode;
    while (true) {

        clear();


        cout << "\n\n\n\n\n\n\n\n\n\t\t\t\t\t/==============================================\\" << endl;
        cout << "\t\t\t\t\t|          SWIFT EX LOGISTICS SYSTEM          |" << endl;
        cout << "\t\t\t\t\t|==============================================|" << endl;
        cout << "\t\t\t\t\t|   PLEASE SELECT YOUR ROLE:                   |" << endl;
        cout << "\t\t\t\t\t|   [1] >> ADMINISTRATOR ACCESS                |" << endl;
        cout << "\t\t\t\t\t|   [2] >> CUSTOMER PORTAL                     |" << endl;
        cout << "\t\t\t\t\t|   [0] >> SHUTDOWN SYSTEM                     |" << endl;
        cout << "\t\t\t\t\t \==============================================/" << endl;
        cout << "\t\t\t\t\t\t\t  Choice: ";

        if (!(cin >> mode)) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore(1000, '\n');

        if (mode == 0) break;
        if (mode == 1) admin.run();
        else if (mode == 2) customer.run();
    }

    clear();
    cout<<"SYSTEM SHUTDOWN & DATA SAVING.........";
    engine.save_map();
    engine.save_all();
    cout << "\n\t TERMINATING SECURELY." << endl;

    return 0;
}