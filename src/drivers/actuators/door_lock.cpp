#include "door_lock.h"

DoorLock::DoorLock(GpioHal* gpio, uint8_t controlPin, bool activeHigh)
    : _gpio(gpio),
      _pin(controlPin),
      _activeHigh(activeHigh),
      _state(State::UNKNOWN),
      _initialized(false)
{
}

bool DoorLock::init()
{
    if (_gpio == nullptr)
        return false;

    _gpio->configure(_pin, GpioHal::Direction::Out);

    // Force safe state immediately
    lock();

    _initialized = true;
    return true;
}

void DoorLock::lock()
{
    if (!_initialized)
        return;

    writeHardware(_activeHigh);
    _state = State::LOCKED;
}

void DoorLock::unlock()
{
    if (!_initialized)
        return;

    writeHardware(!_activeHigh);
    _state = State::UNLOCKED;
}

DoorLock::State DoorLock::getState() const
{
    return _state;
}

void DoorLock::writeHardware(bool active)
{
    _gpio->write(_pin, active);
}
