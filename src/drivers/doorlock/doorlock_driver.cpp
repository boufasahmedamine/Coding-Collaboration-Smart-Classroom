#include "drivers/doorlock/doorlock_driver.h"
#include <Arduino.h>
#include "system/diagnostics.h"

DoorLockDriver::DoorLockDriver(int pin)
    : _pin(pin), _locked(true)
{
}

void DoorLockDriver::begin()
{
    pinMode(_pin, OUTPUT);

    lock(); // Default to locked state for safety

    Diagnostics::logEvent("DOOR LOCK DRIVER INITIALIZED");
}

void DoorLockDriver::lock()
{
    digitalWrite(_pin, HIGH);
    _locked = true;

    Diagnostics::setDoorStatus("LOCKED");
    Diagnostics::logEvent("Door locked");
}

void DoorLockDriver::unlock()
{
    digitalWrite(_pin, LOW);
    _locked = false;

    Diagnostics::setDoorStatus("UNLOCKED");
    Diagnostics::logEvent("Door unlocked");
}

bool DoorLockDriver::isLocked()
{
    return _locked;
}
