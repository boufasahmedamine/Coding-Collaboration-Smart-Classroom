#include "drivers/rfid/pn532.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "config/pins.h"

Adafruit_PN532 nfc(PN532_I2C_ADDRESS);

PN532Driver::PN532Driver()
    : _initialized(false)
{
}

bool PN532Driver::init() {

    Wire.begin();

    nfc.begin();

    uint32_t version = nfc.getFirmwareVersion();

    if (!version) {
        return false;
    }

    nfc.SAMConfig();

    _initialized = true;

    return true;
}

bool PN532Driver::readCard(uint8_t* uidBuffer, uint8_t* uidLength) {

    if (!_initialized) {
        return false;
    }

    return nfc.readPassiveTargetID(
        PN532_MIFARE_ISO14443A,
        uidBuffer,
        uidLength
    );
}
