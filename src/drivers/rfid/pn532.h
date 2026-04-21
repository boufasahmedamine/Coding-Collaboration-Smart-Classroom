#ifndef PN532_DRIVER_H
#define PN532_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

class PN532Driver {
public:
    PN532Driver(uint8_t csPin = 5, const char* name = "RFID");

    bool init();

    virtual bool readCard(uint8_t* uidBuffer, uint8_t* uidLength);

private:
    uint8_t _csPin;
    const char* _name;
    bool _initialized;
    Adafruit_PN532* _nfc;
};

#endif
