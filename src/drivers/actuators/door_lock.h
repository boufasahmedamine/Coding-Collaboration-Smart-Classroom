#ifndef DOOR_LOCK_H
#define DOOR_LOCK_H

#include <stdint.h>
#include "hal/gpio_hal.h"

class DoorLock
{
public:
    enum class State
    {
        LOCKED,
        UNLOCKED,
        UNKNOWN
    };

    DoorLock(GpioHal* gpio, uint8_t controlPin, bool activeHigh);

    bool init();
    void lock();
    void unlock();
    State getState() const;

private:
    GpioHal* _gpio;
    uint8_t _pin;
    bool _activeHigh;
    State _state;
    bool _initialized;

    void writeHardware(bool active);
};

#endif
