#include <Arduino.h>
#include "config/pins.h"
#include "drivers/actuators/ir_projector.h" // adjust if exact path is different

void runIRProjectorTest() {
    static bool initialized = false;

    if (!initialized) {
        Serial.begin(115200);
        Serial.println("IR Projector Test Ready");
        Serial.println("Type '1' to turn ON projector, '0' to turn OFF.");
        initialized = true;
    }

    if (Serial.available()) {
        char c = Serial.read();

        if (c == '1') {
            Serial.println("Sending IR ON Command...");
            // TODO: Call IR projector driver ON method here
        } else if (c == '0') {
            Serial.println("Sending IR OFF Command...");
            // TODO: Call IR projector driver OFF method here
        }
    }
}
