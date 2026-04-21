#ifndef MOCK_PN532_H
#define MOCK_PN532_H

#include "drivers/rfid/pn532.h"
#include <string.h>

/**
 * MockPN532 Reader
 * Used strictly for isolated testing in the src/tests directory.
 * Overrides the hardware readCard with a software-triggered mock.
 */
class MockPN532 : public PN532Driver {
public:
    MockPN532(const char* name = "MOCK_RFID") 
        : PN532Driver(0, name), _pendingRead(false) {}

    /**
     * Inject a card into the reader for the next pull.
     */
    void injectCard(const uint8_t* uid, uint8_t length) {
        memcpy(_uid, uid, length);
        _len = length;
        _pendingRead = true;
    }

    /**
     * Overriden readCard that returns the injected UID instead of polling hardware.
     */
    bool readCard(uint8_t* uidBuffer, uint8_t* uidLength) override {
        if (_pendingRead) {
            memcpy(uidBuffer, _uid, _len);
            *uidLength = _len;
            _pendingRead = false;
            return true;
        }
        return false;
    }

private:
    uint8_t _uid[7];
    uint8_t _len;
    bool _pendingRead;
};

#endif
