#include "drivers/rfid/pn532.h"
#include "system/diagnostics.h"
#include "config/mqtt_config.h"
#include <string.h>

PN532Driver::PN532Driver(uint8_t csPin, const char* name)
    : _csPin(csPin), _name(name), _initialized(false), _nfc(nullptr),
      _lastUidLen(0), _lastScanTime(0), _debounceMs(RFID_DEBOUNCE_MS)
{
    memset(_lastUid, 0, 7);
}

bool PN532Driver::init() {
    _nfc = new Adafruit_PN532(_csPin);
    _nfc->begin();

    uint32_t versiondata = _nfc->getFirmwareVersion();
    if (!versiondata) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[RFID] Hardware %s not found on pin %u", _name, _csPin);
        Diagnostics::logEvent(buf);
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

    uint8_t currentUid[7];
    uint8_t currentLen = 7; // Initialize to max length expected by library

    // 1. Low-level SPI read (40ms timeout)
    bool detected = _nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, currentUid, &currentLen, 40);
    
    if (!detected) return false;

    // 🔴 DEBUG: If we got here, hardware definitely saw a card
    // Serial.printf("[RFID] %s detected card. Checking debounce...\n", _name);

    // 2. Smart Debounce Logic
    unsigned long now = millis();
    bool isSameUid = (currentLen == _lastUidLen) && (memcmp(currentUid, _lastUid, currentLen) == 0);
    bool timeoutPassed = (now - _lastScanTime >= _debounceMs);

    if (!isSameUid || timeoutPassed) {
        // ✅ ACCEPT: Different card OR same card after timeout
        memcpy(_lastUid, currentUid, currentLen);
        _lastUidLen = currentLen;
        _lastScanTime = now;

        // Copy to output buffer
        memcpy(uidBuffer, currentUid, currentLen);
        *uidLength = currentLen;

        return true;
    } else {
        // ❌ IGNORE: Same card within window
        return false;
    }
}
