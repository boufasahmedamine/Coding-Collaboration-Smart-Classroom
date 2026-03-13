#ifndef DOORLOCK_DRIVER_H
#define DOORLOCK_DRIVER_H

class DoorLockDriver
{
public:
    DoorLockDriver(int pin);

    void begin();

    void lock();
    void unlock();

    bool isLocked();

private:
    int _pin;
    bool _locked;
};

#endif
