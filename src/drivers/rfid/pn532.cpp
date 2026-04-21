#include "drivers/rfid/pn532.h"
#include "system/diagnostics.h"
#include <string.h>

PN532Driver::PN532Driver(uint8_t csPin, const char* name)
    : _csPin(csPin), _name(name), _initialized(false), _nfc(nullptr)
{
}

bool PN532Driver::init() {
    _nfc = new Adafruit_PN532(_csPin);
    _nfc->begin();

    uint32_t versiondata = _nfc->getFirmwareVersion();
    if (!versiondata) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[RFID] Hardware %s not found on pin %u", _name, _csPin);
        Diagnostics::logEvent(buf);
        // We'll manage per-reader status in a generic way
        _initialized = false;
        return false; 
    }

    _nfc->SAMConfig();
    char buf[64];
    snprintf(buf, sizeof(buf), "[RFID] %s initialized on pin %u", _name, _csPin);
    Diagnostics::logEvent(buf);

    _initialized = true;
    return true;
}

bool PN532Driver::readCard(uint8_t* uidBuffer, uint8_t* uidLength) {
    if (!_initialized || !_nfc) return false;

    // VERY IMPORTANT:
    // We use a small timeout (e.g. 40ms) instead of the default 1000ms.
    // If we used the default blocking timeout, the RFID FreeRTOS task would stall the SPI bus
    // for a full second during every empty read cycle, preventing SD logger from working!
    bool success = _nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uidBuffer, uidLength, 40);

    return success;
}
