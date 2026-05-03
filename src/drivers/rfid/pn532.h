#ifndef PN532_DRIVER_H
#define PN532_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

class PN532Driver {
public:
    PN532Driver(uint8_t csPin, uint8_t irqPin, const char* name = "RFID");

    bool init();
    bool isAlive();
    void resetCommunication();

    virtual bool readCard(uint8_t* uidBuffer, uint8_t* uidLength);

private:
    uint8_t _csPin;
    uint8_t _irqPin;
    const char* _name;
    bool _initialized;
    Adafruit_PN532* _nfc;

    // Health Monitoring
    unsigned long _lastSuccessfulRead;
    const unsigned long _healthCheckInterval = 10000; // Check every 10s if no cards seen

    // Debounce State
    uint8_t _lastUid[7];
    uint8_t _lastUidLen;
    unsigned long _lastScanTime;
    unsigned long _debounceMs;
};

#endif
