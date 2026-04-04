#include <Arduino.h>
#include "config/pins.h"
#include "drivers/storage/rtc_ds3231.h"

void runRTCTest() {
    static bool initialized = false;
    // static RTCDriver rtc;

    if (!initialized) {
        Serial.begin(115200);
        // rtc.init();
        Serial.println("RTC DS3231 Test Ready");
        initialized = true;
    }

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        // Serial.print("Current RTC Time: ");
        // Serial.println(rtc.getTimeString());
    }
}
