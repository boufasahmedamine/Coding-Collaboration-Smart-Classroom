#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "drivers/actuators/door_lock.h"

class StateMachine {
public:
    enum class State {
        LOCKED,
        SESSION_UNLOCKED
    };

    StateMachine(DoorLock& doorLock, unsigned long sessionDurationMs);

    void init();
    void update();

    // Called when first authorized RFID is detected
    void requestSessionStart();

    State getState() const;

    // Future expansion hooks
    void setPresenceDetected(bool detected);
    void setOverrideActive(bool active);

private:
    DoorLock& _doorLock;

    State _currentState;

    unsigned long _sessionStartTime;
    unsigned long _sessionDurationMs;

    bool _presenceDetected;
    bool _overrideActive;
};

#endif