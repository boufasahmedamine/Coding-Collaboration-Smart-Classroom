#include "drivers/rfid/pn532.h"
#include "system/diagnostics.h"
#include "config/mqtt_config.h"
#include <string.h>
#include "config/pins.h"

PN532Driver::PN532Driver(uint8_t csPin, uint8_t irqPin, const char* name)
    : _csPin(csPin), _irqPin(irqPin), _name(name), _initialized(false), _nfc(nullptr),
      _lastSuccessfulRead(0), _lastUidLen(0), _lastScanTime(0), _debounceMs(RFID_DEBOUNCE_MS)
{
    memset(_lastUid, 0, 7);
}

bool PN532Driver::init() {
    pinMode(_irqPin, INPUT_PULLUP);
    
    if (_nfc) delete _nfc;
    _nfc = new Adafruit_PN532(_csPin);
    _nfc->begin();

    uint32_t versiondata = _nfc->getFirmwareVersion();
    if (!versiondata) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[RFID] Hardware %s not found on pin %u", _name, _csPin);
        Diagnostics::logEvent(buf);
        
        // Report to dashboard
        if (strcmp(_name, "OUTSIDE") == 0) Diagnostics::setRFIDStatusOut("HW NOT FOUND");
        else Diagnostics::setRFIDStatusIn("HW NOT FOUND");

        _initialized = false;
        return false; 
    }

    _nfc->SAMConfig();
    _lastSuccessfulRead = millis();

    // Report to dashboard
    if (strcmp(_name, "OUTSIDE") == 0) Diagnostics::setRFIDStatusOut("READY");
    else Diagnostics::setRFIDStatusIn("READY");

    char buf[64];
    snprintf(buf, sizeof(buf), "[RFID] %s initialized (IRQ:%u)", _name, _irqPin);
    Diagnostics::logEvent(buf);

    _initialized = true;
    return true;
}

bool PN532Driver::isAlive() {
    if (!_initialized || !_nfc) return false;
    
    // Check firmware version as a heartbeat
    return (_nfc->getFirmwareVersion() != 0);
}

void PN532Driver::resetCommunication() {
    Diagnostics::logEvent(String("[RFID] Resetting ") + _name + "...");
    init();
}

bool PN532Driver::readCard(uint8_t* uidBuffer, uint8_t* uidLength) {
    if (!_initialized || !_nfc) return false;

    uint8_t currentUid[7];
    uint8_t currentLen = 7;

    // Direct hardware read with a safe timeout
    bool detected = _nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, currentUid, &currentLen, 50);
    
    if (!detected) return false;

    _lastSuccessfulRead = millis();

    // 2. Smart Debounce Logic
    unsigned long now = millis();
    bool isSameUid = (currentLen == _lastUidLen) && (memcmp(currentUid, _lastUid, currentLen) == 0);
    bool timeoutPassed = (now - _lastScanTime >= _debounceMs);

    if (!isSameUid || timeoutPassed) {
        memcpy(_lastUid, currentUid, currentLen);
        _lastUidLen = currentLen;
        _lastScanTime = now;
        memcpy(uidBuffer, currentUid, currentLen);
        *uidLength = currentLen;
        return true;
    }
    return false;
}
