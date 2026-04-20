#include "tests/test_pir.h"
#include <Arduino.h>
#include "drivers/pir/pir_driver.h"
#include "config/pins.h"

static PIRDriver pir(PIN_PIR);

void runPIRTest() {
    static bool initialized = false;
    if (!initialized) {
        Serial.begin(115200);
        pir.begin();
        Serial.println("PIR Sensor Test Initialized");
        initialized = true;
    }

    pir.update();

    if (pir.isMotionDetected()) {
        static unsigned long lastDetected = 0;
        if (millis() - lastDetected > 500) {
            Serial.println("[PIR] MOTION DETECTED!");
            lastDetected = millis();
        }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}
