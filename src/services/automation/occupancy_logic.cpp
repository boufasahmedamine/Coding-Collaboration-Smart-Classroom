#include "services/automation/occupancy_logic.h"

OccupancyLogic::OccupancyLogic(LD2410Driver& radar)
    : _radar(radar), _occupied(false)
{
}

void OccupancyLogic::update()
{
    _occupied = _radar.isPresenceDetected();
}

bool OccupancyLogic::isOccupied() const
{
    return _occupied;
}