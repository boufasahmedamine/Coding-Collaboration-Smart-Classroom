#include "drivers/doorlock/doorlock_driver.h"
#include <Arduino.h>

DoorLockDriver::DoorLockDriver(int pin)
    : _pin(pin), _locked(true)
{
}

void DoorLockDriver::begin()
{
    pinMode(_pin, OUTPUT);

    lock(); // Default to locked state for safety

    Serial.println("[LOCK] Door lock driver initialized");
}

void DoorLockDriver::lock()
{
    digitalWrite(_pin, HIGH);
    _locked = true;

    Serial.println("[LOCK] Door locked");
}

void DoorLockDriver::unlock()
{
    digitalWrite(_pin, LOW);
    _locked = false;

    Serial.println("[LOCK] Door unlocked");
}

bool DoorLockDriver::isLocked()
{
    return _locked;
}
