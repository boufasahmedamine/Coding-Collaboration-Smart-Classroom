#ifndef PRESENCE_SERVICE_H
#define PRESENCE_SERVICE_H

#include "drivers/pir/pir_driver.h"

class PresenceService
{
public:
    PresenceService(PIRDriver* driver);

    void update();

    bool isOccupied();
    bool justBecameOccupied();
    bool justBecameEmpty();

private:
    PIRDriver* _driver;

    bool _occupied;
    bool _previousState;
};

#endif
