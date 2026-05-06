#include "tests/test_auth_proxy.h"
#include "tests/mock_rfid_reader.h"
#include "services/auth/auth_proxy.h"
#include "communication/mqtt_manager.h"
#include <Arduino.h>

static MQTTManager dummyMqtt("127.0.0.1", 1883);
static AuthProxy authProxy(&dummyMqtt);
static MockRFIDReader mockRFID;

void runAuthProxyTest() {
    static bool initialized = false;
    if (!initialized) {
        Serial.begin(115200);
        Serial.println("--- AuthProxy Mock Verification ---");
        Serial.println("1. Type 'i' to inject card into Mock Reader");
        Serial.println("2. Test will then pull from mock and send to AuthProxy");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();
        if (c == 'i') {
            uint8_t uid[] = {0x12, 0x34, 0x56, 0x78};
            mockRFID.simulateCard(uid);
            Serial.println("[TEST] Card injected into mock reader.");
        }
    }

    // Logic simulation as it would appear in a task
    uint8_t uidBuf[7];
    uint8_t uidLen;
    if (mockRFID.readCard(uidBuf, &uidLen)) {
        Serial.println("[TEST] Reader detected mock card, forwarding to AuthProxy...");
        authProxy.requestAuthorization(uidBuf, uidLen);
    }
}
