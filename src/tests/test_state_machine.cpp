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
        Serial.println("--- StateMachine Verification Test ---");
        Serial.println("Commands:");
        Serial.println("  'r' : Simulate RFID card read (-> WAITING_FOR_AUTH)");
        Serial.println("  'g' : Simulate Access Granted by Server (-> UNLOCKED)");
        Serial.println("  'd' : Simulate Access Denied by Server (-> LOCKED)");
        Serial.println("  'u' : Quick unlock (direct grant)");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();

        switch (c) {
            case 'r':
                Serial.println("Event: RFID_READ");
                stateMachine.handleEvent(StateMachine::SystemEvent::RFID_READ);
                break;
            case 'g':
                Serial.println("Event: ACCESS_GRANTED");
                stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
                break;
            case 'd':
                Serial.println("Event: ACCESS_DENIED");
                stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_DENIED);
                break;
            case 'u':
                Serial.println("Event: UNLOCK_REQUEST (Grant)");
                stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
                break;
        }
    }

    stateMachine.update();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        
        Serial.print("Current State: ");
        switch(stateMachine.getState()) {
            case StateMachine::SystemState::LOCKED:           Serial.println("LOCKED"); break;
            case StateMachine::SystemState::WAITING_FOR_AUTH: Serial.println("WAITING_FOR_AUTH"); break;
            case StateMachine::SystemState::UNLOCKED:         Serial.println("UNLOCKED"); break;
            default:                                          Serial.println("UNKNOWN"); break;
        }
    }
}