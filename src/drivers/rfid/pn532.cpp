#include "drivers/rfid/pn532.h"
#include <string.h>

PN532Driver::PN532Driver()
    : _initialized(false), _simulateCard(false)
{
}

bool PN532Driver::init() {

    Serial.println("[PN532] SIMULATION DRIVER INITIALIZED");
    Serial.println("[INFO] Type 'c' in serial monitor to simulate card scan");

    _initialized = true;
    return true;
}

void PN532Driver::simulateCardDetected() {
    _simulateCard = true;
}

bool PN532Driver::readCard(uint8_t* uidBuffer, uint8_t* uidLength) {

    if (!_initialized) return false;

    if (_simulateCard) {

        // Simulated UID (DEADBEEF)
        uint8_t simulatedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

        memcpy(uidBuffer, simulatedUID, 4);
        *uidLength = 4;

        Serial.println("[PN532] SIMULATED CARD DETECTED");

        _simulateCard = false;  // Reset flag after one read

        return true;
    }

    return false;
}
