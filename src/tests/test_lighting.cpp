#include <Arduino.h>
#include "config/pins.h"
#include "drivers/actuators/lighting.h"

void runLightingTest() {
    static bool initialized = false;
    // Instantiate the driver using a pin from pins.h or replace with exact pin macro
    // static LightingDriver lighting(PIN_LIGHTING_RELAY);

    if (!initialized) {
        Serial.begin(115200);
        // lighting.init(); // Uncomment once initialized
        Serial.println("Lighting Test Ready");
        Serial.println("Type 't' to toggle lights.");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();

        if (c == 't') {
            Serial.println("Toggling lighting...");
            // lighting.toggle(); // Implement corresponding toggle or on/off method call
        }
    }
}
