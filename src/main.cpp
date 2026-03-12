#include <Arduino.h>
#include "config/pins.h"
#include "drivers/actuators/door_lock.h"
#include "drivers/rfid/pn532.h"
#include "services/auth/access_control.h"
#include "system/state_machine.h"

DoorLock doorLock(PIN_DOOR_LOCK, true);
StateMachine stateMachine(doorLock, 5400000); // 1.5 hours
PN532Driver rfid;
AccessControl accessControl;

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 10000;  // every 10 seconds

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("[BOOT] Smart Classroom Node Starting...");

    if (!rfid.init()) {
        Serial.println("[ERROR] PN532 initialization failed");
    } else {
        Serial.println("[INFO] PN532 RFID reader initialized");
    }

    stateMachine.init();

    Serial.println("[INFO] Type 'u' to request unlock");
}

void loop() {
    stateMachine.update();

    // ---- Serial Input Handler ----
    if (Serial.available()) {
        char c = Serial.read();

        if (c == 'c') {
            // Trigger RFID simulation
            rfid.simulateCardDetected();
        } else if (c == 'u') {
            Serial.println("[EVENT] Session Start Requested");
            stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED);
        }
    }

    // ---- RFID Card Detection & Authorization ----
    uint8_t uid[7];
    uint8_t uidLength;

    if (rfid.readCard(uid, &uidLength)) {
        Serial.print("Card detected UID: ");
        for (int i = 0; i < uidLength; i++) {
            Serial.print(uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        if (accessControl.isAuthorized(uid, uidLength)) {
            Serial.println("[AUTH] ACCESS GRANTED");
            stateMachine.handleEvent(StateMachine::SystemEvent::ACCESS_GRANTED, uid, uidLength);
        } else {
            Serial.println("[AUTH] ACCESS DENIED");
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