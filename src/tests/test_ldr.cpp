#include <Arduino.h>
#include "config/pins.h"
#include "drivers/ldr/ldr_driver.h"

void runLDRTest() {
    static bool initialized = false;
    // static LDRDriver ldr(PIN_LDR_ANALOG);

    if (!initialized) {
        Serial.begin(115200);
        // ldr.init();
        Serial.println("LDR Sensor Test Ready");
        initialized = true;
    }

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        // int lightLevel = ldr.readRaw();
        // Serial.print("Current Light Level (Raw): ");
        // Serial.println(lightLevel);
    }
}
