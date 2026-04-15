#ifndef PN532_DRIVER_H
#define PN532_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

class PN532Driver {
public:
    PN532Driver(uint8_t csPin = 5);

    bool init();

    bool readCard(uint8_t* uidBuffer, uint8_t* uidLength);

    void simulateCardDetected();

private:
    uint8_t _csPin;
    bool _initialized;
    bool _simulateCard;
    Adafruit_PN532* _nfc;
};

#endif
