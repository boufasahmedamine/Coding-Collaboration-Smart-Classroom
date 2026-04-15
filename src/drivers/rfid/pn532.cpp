#include "drivers/rfid/pn532.h"
#include <string.h>

PN532Driver::PN532Driver(uint8_t csPin)
    : _csPin(csPin), _initialized(false), _simulateCard(false), _nfc(nullptr)
{
}

bool PN532Driver::init() {
    _nfc = new Adafruit_PN532(_csPin);
    _nfc->begin();

    uint32_t versiondata = _nfc->getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[PN532] HARDWARE NOT FOUND. Check wiring.");
        // Still returning true so simulation commands can work, but mark hardware as detached.
        // In a strict production system, we might return false here.
        _initialized = false;
        return true; 
    }

    _nfc->SAMConfig();
    Serial.println("[PN532] HARDWARE DRIVER INITIALIZED");

    _initialized = true;
    return true;
}

void PN532Driver::simulateCardDetected() {
    _simulateCard = true;
}

bool PN532Driver::readCard(uint8_t* uidBuffer, uint8_t* uidLength) {

    // 1. Process Simulation
    if (_simulateCard) {
        uint8_t simulatedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};
        memcpy(uidBuffer, simulatedUID, 4);
        *uidLength = 4;
        Serial.println("[PN532] SIMULATED CARD DETECTED");
        _simulateCard = false;
        return true;
    }

    // 2. Hardware Read
    if (!_initialized || !_nfc) return false;

    // VERY IMPORTANT:
    // We use a small timeout (e.g. 40ms) instead of the default 1000ms.
    // If we used the default blocking timeout, the RFID FreeRTOS task would stall the SPI bus
    // for a full second during every empty read cycle, preventing SD logger from working!
    bool success = _nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uidBuffer, uidLength, 40);

    return success;
}
