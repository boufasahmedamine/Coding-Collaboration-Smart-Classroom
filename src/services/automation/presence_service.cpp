#include "services/automation/presence_service.h"

PresenceService::PresenceService(LD2410Driver* driver)
    : _driver(driver),
      _occupied(false),
      _previousState(false)
{
}

void PresenceService::update()
{
    _previousState = _occupied;

    _occupied = _driver->isPresenceDetected();
}

bool PresenceService::isOccupied()
{
    return _occupied;
}

bool PresenceService::justBecameOccupied()
{
    return (!_previousState && _occupied);
}

bool PresenceService::justBecameEmpty()
{
    return (_previousState && !_occupied);
}
