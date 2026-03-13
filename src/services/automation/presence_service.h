#ifndef PRESENCE_SERVICE_H
#define PRESENCE_SERVICE_H

#include "drivers/ld2410/ld2410_driver.h"

class PresenceService
{
public:
    PresenceService(LD2410Driver* driver);

    void update();

    bool isOccupied();
    bool justBecameOccupied();
    bool justBecameEmpty();

private:
    LD2410Driver* _driver;

    bool _occupied;
    bool _previousState;
};

#endif
