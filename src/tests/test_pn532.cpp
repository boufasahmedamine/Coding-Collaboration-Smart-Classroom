#include <Arduino.h>
#include "config/pins.h"
#include "drivers/rfid/pn532.h"

void runPN532Test() {
    static bool initialized = false;
    // static PN532Driver pn532(...);

    if (!initialized) {
        Serial.begin(115200);
        // pn532.init();
        Serial.println("PN532 RFID Test Ready");
        Serial.println("Waiting for an RFID card...");
        initialized = true;
    }

    // Call update/loop regularly if driver is non-blocking
    // pn532.update();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        // if (pn532.cardDetected()) {
        //     Serial.print("Card Scanned: UID = ");
        //     Serial.println(pn532.getUID());
        // }
    }
}
