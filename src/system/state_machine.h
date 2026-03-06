#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "drivers/actuators/door_lock.h"
#include "system/session_record.h"

class StateMachine {
public:
    enum class SystemEvent {
    ACCESS_GRANTED,
    PRESENCE_DETECTED,
    PRESENCE_LOST,
    OVERRIDE_ON,
    OVERRIDE_OFF
};
enum class State {
    LOCKED,
    SESSION_ACTIVE
};

    StateMachine(DoorLock& doorLock, unsigned long sessionDurationMs);

    void init();
    void update();

    // Event handling interface for future expansion
    void handleEvent(SystemEvent event);

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

    SessionRecord _session;

    void transitionTo(State newState);

    void handleLockedState(SystemEvent event);
    void handleSessionActiveState(SystemEvent event);
};

#endif