#include <Arduino.h>
#include "config/pins.h"
#include "drivers/doorlock/doorlock_driver.h"

void runDoorlockTest() {
    static bool initialized = false;
    // Utilize the standard DoorLockDriver mapping
    // static DoorLockDriver doorlock(PIN_DOOR_LOCK);

    if (!initialized) {
        Serial.begin(115200);
        // doorlock.init();
        Serial.println("Doorlock Test Ready");
        Serial.println("Type 'u' to unlock, 'l' to lock.");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();

        if (c == 'u') {
            Serial.println("Unlocking door...");
            // doorlock.unlock();
        } else if (c == 'l') {
            Serial.println("Locking door...");
            // doorlock.lock();
        }
    }
}
