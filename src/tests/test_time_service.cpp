#include "tests/test_time_service.h"
#include <Arduino.h>
#include "services/network/time_service.h"

static TimeService timeService;

void runTimeServiceTest() {
    static bool initialized = false;
    if (!initialized) {
        Serial.begin(115200);
        Serial.println("TimeService Test Initialized");
        Serial.println("Type 's' to simulate sync (fixed epoch 1672531200)");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();
        if (c == 's') {
            timeService.syncTime(1672531200); // 2023-01-01 00:00:00
            Serial.println("[TIME] Synced to 1672531200");
        }
    }

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        if (timeService.isSynced()) {
            Serial.printf("Current Epoch: %u\n", timeService.getCurrentTime());
        } else {
            Serial.println("Waiting for time sync...");
        }
    }
}
