#include <Arduino.h>
#include "config/pins.h"
#include "drivers/actuators/door_lock.h"
#include "system/state_machine.h"

static DoorLock doorLock(PIN_DOOR_LOCK, true);
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
            stateMachine.requestSessionStart();
        }
    }

    stateMachine.update();

    // Optional debug print
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();

        if (stateMachine.getState() == StateMachine::State::LOCKED)
            Serial.println("State: LOCKED");
        else
            Serial.println("State: UNLOCKED");
    }
}