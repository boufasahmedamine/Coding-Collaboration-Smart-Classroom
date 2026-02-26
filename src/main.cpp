#include <Arduino.h>
#include "config/pins.h"
#include "drivers/actuators/door_lock.h"
#include "system/state_machine.h"

DoorLock doorLock(PIN_DOOR_LOCK, true);
StateMachine stateMachine(doorLock, 5400000); // 1.5 hours

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 10000;  // every 10 seconds

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("[BOOT] Smart Classroom Node Starting...");

    stateMachine.init();

    Serial.println("[INFO] Type 'u' to request unlock");
}

void loop() {
    stateMachine.update();
    
// ---- Serial Test Trigger ----
if (Serial.available()) {
    char c = Serial.read();

    if (c == 'u') {
        Serial.println("[EVENT] Session Start Requested");
        stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
    }
}
    // ---- Heartbeat ----
    if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        Serial.print("[ALIVE] ");
        Serial.print(millis() / 1000);
        Serial.println("s");
        lastHeartbeat = millis();
    }

    delay(10);
}