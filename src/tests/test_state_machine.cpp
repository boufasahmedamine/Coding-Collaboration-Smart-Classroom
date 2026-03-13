#include <Arduino.h>
#include "config/pins.h"
#include "drivers/doorlock/doorlock_driver.h"
#include "system/state_machine.h"

static DoorLockDriver doorLock(PIN_DOOR_LOCK);
static StateMachine stateMachine(doorLock, 5000);

void runStateMachineTest() {
    static bool initialized = false;

    if (!initialized) {
        Serial.begin(115200);
        stateMachine.init();
        Serial.println("StateMachine Test Ready");
        Serial.println("Type 'u' to unlock");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();

        if (c == 'u') {
            Serial.println("Unlock session requested");
            stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
        }
    }

    stateMachine.update();

    // Optional debug print
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();

        if (stateMachine.getState() == StateMachine::SystemState::LOCKED)
            Serial.println("State: LOCKED");
        else
            Serial.println("State: UNLOCKED");
    }
}