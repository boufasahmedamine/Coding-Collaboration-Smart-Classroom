#include "services/automation/presence_service.h"

PresenceService::PresenceService(MainsPIRInput* driver)
    : _driver(driver),
      _occupied(false),
      _previousState(false),
      _lastActivityTime(0)
{
}

void PresenceService::update()
{
    _previousState = _occupied;

    if (_driver) {
        bool rawMotion = _driver->isMotionDetected();
        unsigned long now = millis();

        if (rawMotion) {
            _lastActivityTime = now;
            _occupied = true;
        } else {
            // Only mark as empty if the persistence window has expired
            if (now - _lastActivityTime >= PERSISTENCE_WINDOW_MS) {
                _occupied = false;
            }
        }
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
