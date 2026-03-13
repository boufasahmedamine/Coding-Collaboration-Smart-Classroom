#ifndef OCCUPANCY_LOGIC_H
#define OCCUPANCY_LOGIC_H

#include "drivers/ld2410/ld2410_driver.h"

class OccupancyLogic
{
public:
    OccupancyLogic(LD2410Driver& radar);
    void update();
    bool isOccupied() const;

private:
    LD2410Driver& _radar;
    bool _occupied;
};

#endif
