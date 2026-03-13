#include <Arduino.h>
#include "config/pins.h"
#include "drivers/doorlock/doorlock_driver.h"
#include "drivers/rfid/pn532.h"
#include "drivers/ld2410/ld2410_driver.h"
#include "drivers/ldr/ldr_driver.h"
#include "services/auth/access_control.h"
#include "services/attendance/attendance_manager.h"
#include "services/logging/log_manager.h"
#include "services/automation/presence_service.h"
#include "services/automation/light_service.h"
#include "services/automation/automation_controller.h"
#include "communication/wifi_manager.h"
#include "communication/mqtt_manager.h"
#include "services/system/heartbeat_service.h"
#include "system/state_machine.h"

DoorLockDriver doorLock(PIN_DOOR_LOCK);

WiFiManager wifiManager("SSID", "PASSWORD");
MQTTManager mqttManager("192.168.1.100", 1883);
LogManager logManager(&mqttManager);
AttendanceManager attendanceManager(&logManager);
StateMachine stateMachine(doorLock, 5400000, &attendanceManager); // 1.5 hours
PN532Driver rfid;
LD2410Driver presenceSensor(16, 17);
LDRDriver lightSensor(34); // using pin 34 for analog input
PresenceService presenceService(&presenceSensor);
LightService lightService(&lightSensor);
AutomationController automationController(&presenceService, &lightService);
HeartbeatService heartbeat(&mqttManager);
AccessControl accessControl;

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("[BOOT] Smart Classroom Node Starting...");

    if (!rfid.init()) {
        Serial.println("[ERROR] PN532 initialization failed");
    } else {
        Serial.println("[INFO] PN532 RFID reader initialized");
    }

    doorLock.begin();
    stateMachine.init();
    wifiManager.begin();
    mqttManager.begin();
    presenceSensor.begin();
    lightSensor.begin();

    Serial.println("[INFO] Type 'u' to request unlock");
}

void loop() {
    wifiManager.update();
    mqttManager.update();
    presenceSensor.update();
    lightSensor.update();
    presenceService.update();
    lightService.update();
    automationController.update();
    heartbeat.update();
    stateMachine.update();

    if (presenceService.justBecameOccupied())
    {
        Serial.println("[PRESENCE] Room became occupied");
    }

    if (presenceService.justBecameEmpty())
    {
        Serial.println("[PRESENCE] Room became empty");
    }

    if (lightService.isDark())
    {
        Serial.println("[LIGHT] Room is dark");
    }

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

    delay(10);
}