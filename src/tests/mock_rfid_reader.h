#ifndef MOCK_RFID_READER_H
#define MOCK_RFID_READER_H

#include "drivers/rfid/irfid_reader.h"
#include <string.h>

class MockRFIDReader : public IRFIDReader {
public:
    MockRFIDReader() : _initialized(true), _alive(true), _hasCard(false) {
        memset(_mockUid, 0, sizeof(_mockUid));
    }

    bool init() override {
        _initialized = true;
        return true;
    }

    bool isInitialized() const override { return _initialized; }
    
    bool isAlive() override { return _alive; }

    bool readCard(uint8_t* uidBuffer, uint8_t* uidLength) override {
        if (_hasCard && _initialized && _alive) {
            memcpy(uidBuffer, _mockUid, 5);
            *uidLength = 5;
            _hasCard = false; // consume it
            return true;
        }
        return false;
    }

    void resetCommunication() override {
        _initialized = true;
    }

    // --- Mock Controls ---
    void simulateCard(const uint8_t* uid) {
        memcpy(_mockUid, uid, 5);
        _hasCard = true;
    }

    void simulateHardwareFailure() { _alive = false; }
    void simulateHardwareRecovery() { _alive = true; }
    void simulateUninitialized() { _initialized = false; }

private:
    bool _initialized;
    bool _alive;
    bool _hasCard;
    uint8_t _mockUid[5];
};

#endif
