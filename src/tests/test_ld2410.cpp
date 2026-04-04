#include <Arduino.h>
#include "config/pins.h"
#include "drivers/ld2410/ld2410_driver.h" // Or "drivers/sensors/ld2410.h"

void runLD2410Test() {
    static bool initialized = false;
    // static LD2410Driver ld2410(...);

    if (!initialized) {
        Serial.begin(115200);
        // ld2410.begin();
        Serial.println("LD2410 Presence Sensor Test Ready");
        initialized = true;
    }

    // ld2410.update(); // Continuously update sensor state

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        // Serial.print("Presence Output: ");
        // Serial.println(ld2410.isPresenceDetected() ? "YES" : "NO");
    }
}
