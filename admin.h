#pragma once
#include "logistic engine.h"

class AdminControl {
private:
    LogisticsEngine& engine;

    bool authenticate();

public:
    AdminControl(LogisticsEngine& e);

    void run();            
    void manageMap();      
    void manage_rider();   
    void runSimulation();  
    void systemAnalysis();    
};