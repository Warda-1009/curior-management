#pragma once
#pragma once
#include "logistic engine.h"

class CustomerPortal {
private:
    LogisticsEngine& engine;
public:
    CustomerPortal(LogisticsEngine& e);
    void run();

    void track_parcel();
};