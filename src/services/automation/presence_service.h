#ifndef PRESENCE_SERVICE_H
#define PRESENCE_SERVICE_H

#include "drivers/sensors/mains_pir_input.h"

class PresenceService
{
public:
    PresenceService(MainsPIRInput* driver);

    void update();

    bool isOccupied();
    bool justBecameOccupied();
    bool justBecameEmpty();

private:
    MainsPIRInput* _driver;

    bool _occupied;
    bool _previousState;
};

#endif
