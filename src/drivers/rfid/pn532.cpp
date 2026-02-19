#include "pn532.h"
#include <Arduino.h>  // For delay

Pn532::Pn532(I2cBus* i2cBus, uint8_t address)
    : _i2c(i2cBus), _address(address), _state(State::IDLE), _initialized(false)
{
}

Pn532::~Pn532()
{
    // Cleanup if needed
}

bool Pn532::init()
{
    if (!_i2c) return false;

    // Wake up PN532
    if (!wakeUp()) return false;

    // Set parameters (e.g., enable CRC)
    if (!setParameters()) return false;

    _initialized = true;
    _state = State::IDLE;
    return true;
}

bool Pn532::configureIrq(uint8_t irqPin)
{
    // Configure GPIO for interrupt (external pull-up)
    pinMode(irqPin, INPUT_PULLUP);
    return true;
}

bool Pn532::startRead()
{
    if (!_initialized) return false;

    // Send InListPassiveTarget command for ISO14443A cards
    uint8_t cmd[] = {0xD4, 0x4A, 0x01, 0x00};  // InListPassiveTarget, 1 target, 106 kbps
    if (!sendCommand(cmd, sizeof(cmd))) return false;

    _state = State::READING;
    return true;
}

bool Pn532::getUid(uint8_t* uid, size_t& length)
{
    if (!_initialized || _state != State::READING) return false;

    uint8_t resp[32];
    size_t respLen;
    if (!receiveResponse(resp, respLen, 2000)) {
        _state = State::ERROR;
        return false;
    }

    // Parse response: Check for Tg and UID
    if (respLen < 5 || resp[0] != 0xD5 || resp[1] != 0x4B) {
        _state = State::IDLE;
        return false;  // No card or error
    }

    uint8_t numTg = resp[2];
    if (numTg == 0) {
        _state = State::IDLE;
        return false;
    }

    // Extract UID (simplified for 4-byte UID)
    length = resp[7];  // UID length
    if (length > 10) length = 10;  // Cap
    memcpy(uid, &resp[8], length);

    _state = State::IDLE;
    return true;
}

Pn532::State Pn532::getState() const
{
    return _state;
}

bool Pn532::sendCommand(const uint8_t* cmd, size_t cmdLen)
{
    uint8_t frame[64];
    frame[0] = 0x00;  // Preamble
    frame[1] = 0x00;  // Start code
    frame[2] = 0xFF;  // Start code
    frame[3] = cmdLen + 1;  // Length
    frame[4] = ~frame[3] + 1;  // LCS
    frame[5] = 0xD4;  // TFI
    memcpy(&frame[6], cmd, cmdLen);

    // Calculate DCS
    uint8_t dcs = 0xD4;
    for (size_t i = 0; i < cmdLen; ++i) dcs += cmd[i];
    frame[6 + cmdLen] = ~dcs + 1;
    frame[7 + cmdLen] = 0x00;  // Postamble

    size_t frameLen = 8 + cmdLen;
    return _i2c->writeBytes(_address, 0x00, frame, frameLen);  // Write to I2C without reg
}

bool Pn532::receiveResponse(uint8_t* resp, size_t& respLen, uint16_t timeout)
{
    uint32_t start = millis();
    while (millis() - start < timeout) {
        uint8_t status;
        if (_i2c->readByte(_address, 0x00, status) && (status & 0x01)) {  // Ready bit
            // Read response
            uint8_t buffer[64];
            if (_i2c->readBytes(_address, 0x00, buffer, 64)) {
                // Parse frame (simplified)
                if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0xFF) {
                    uint8_t len = buffer[3];
                    memcpy(resp, &buffer[5], len - 1);  // Skip TFI
                    respLen = len - 1;
                    return true;
                }
            }
        }
        delay(10);
    }
    return false;
}

bool Pn532::wakeUp()
{
    uint8_t wakeCmd[] = {0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    return _i2c->writeBytes(_address, 0x00, wakeCmd, sizeof(wakeCmd));
}

bool Pn532::setParameters()
{
    uint8_t cmd[] = {0xD4, 0x12, 0x14};  // SetParameters: enable CRC, etc.
    return sendCommand(cmd, sizeof(cmd));
}