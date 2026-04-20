#include "services/automation/presence_service.h"

PresenceService::PresenceService(PIRDriver* driver)
    : _driver(driver),
      _occupied(false),
      _previousState(false)
{
}

void PresenceService::update()
{
    _previousState = _occupied;

    if (_driver) {
        _occupied = _driver->isMotionDetected();
    }
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
