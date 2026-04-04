#include <Arduino.h>
#include "config/pins.h"
#include "drivers/storage/sdcard.h"

void runSDCardTest() {
    static bool initialized = false;
    // static SDCardDriver sdcard(PIN_SD_CS);

    if (!initialized) {
        Serial.begin(115200);
        // sdcard.init();
        Serial.println("SD Card Test Ready");
        Serial.println("Type 'w' to write a test file, 'r' to read it.");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();

        if (c == 'w') {
            Serial.println("Attempting to write to /test.txt...");
            // sdcard.writeFile("/test.txt", "SD Card Write Test Successful!");
        } else if (c == 'r') {
            Serial.println("Attempting to read from /test.txt:");
            // sdcard.readFile("/test.txt");
        }
    }
}
