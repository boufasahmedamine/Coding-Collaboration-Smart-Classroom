#include "drivers/rfid/rdm6300.h"
#include "system/diagnostics.h"
#include "config/mqtt_config.h"
#include <string.h>

RDM6300Driver::RDM6300Driver(HardwareSerial* serialPort, uint8_t rxPin, const char* name)
    : _serial(serialPort), _rxPin(rxPin), _name(name), _initialized(false),
      _state(ParserState::WAIT_FOR_0X02), _bufferIndex(0), _frameStartTime(0),
      _lastScanTime(0)
{
    memset(_buffer, 0, sizeof(_buffer));
    memset(_lastUid, 0, sizeof(_lastUid));
}

bool RDM6300Driver::init() {
    // Explicit UART routing
    _serial->begin(RDM6300_BAUDRATE, SERIAL_8N1, _rxPin, -1);
    
    // Flush any garbage
    while (_serial->available()) {
        _serial->read();
    }
    
    resetParser();
    _initialized = true;

    // Report to dashboard
    if (strcmp(_name, "OUTSIDE") == 0) Diagnostics::setRFIDStatusOut("READY");
    else Diagnostics::setRFIDStatusIn("READY");

    char buf[64];
    snprintf(buf, sizeof(buf), "[RFID] %s (RDM6300) init on RX:%u", _name, _rxPin);
    Diagnostics::logEvent(buf);

    return true;
}

bool RDM6300Driver::isInitialized() const {
    return _initialized;
}

bool RDM6300Driver::isAlive() {
    // UART reader doesn't have a direct firmware query like PN532.
    // We assume it's alive if initialized, or we can check if serial is still configured.
    // For now, if _initialized is true, we consider it alive.
    return _initialized;
}

void RDM6300Driver::resetCommunication() {
    Diagnostics::logEvent(String("[RFID] Resetting ") + _name + " UART...");
    _serial->end();
    delay(10);
    init();
}

void RDM6300Driver::resetParser() {
    _state = ParserState::WAIT_FOR_0X02;
    _bufferIndex = 0;
    _frameStartTime = 0;
}

uint8_t RDM6300Driver::asciiHexToByte(char c1, char c2) {
    uint8_t val = 0;
    if (c1 >= '0' && c1 <= '9') val += (c1 - '0') << 4;
    else if (c1 >= 'A' && c1 <= 'F') val += (c1 - 'A' + 10) << 4;
    
    if (c2 >= '0' && c2 <= '9') val += (c2 - '0');
    else if (c2 >= 'A' && c2 <= 'F') val += (c2 - 'A' + 10);
    
    return val;
}

bool RDM6300Driver::validateChecksum() {
    // RDM6300 Frame: 0x02 [10 chars data] [2 chars checksum] 0x03
    // Checksum is XOR of 5 data bytes.
    uint8_t xorSum = 0;
    for (int i = 1; i < 11; i += 2) {
        xorSum ^= asciiHexToByte(_buffer[i], _buffer[i+1]);
    }
    uint8_t expectedSum = asciiHexToByte(_buffer[11], _buffer[12]);
    return xorSum == expectedSum;
}

void RDM6300Driver::convertAsciiToBinary(uint8_t* binaryUid) {
    // The 10 ASCII characters (indices 1 to 10) represent 5 bytes
    for (int i = 0; i < 5; i++) {
        binaryUid[i] = asciiHexToByte(_buffer[1 + (i*2)], _buffer[2 + (i*2)]);
    }
}

bool RDM6300Driver::readCard(uint8_t* uidBuffer, uint8_t* uidLength) {
    if (!_initialized) return false;

    // Non-blocking read
    while (_serial->available()) {
        uint8_t b = _serial->read();
        
        // Timeout handling
        if (_state != ParserState::WAIT_FOR_0X02) {
            if (millis() - _frameStartTime > RDM6300_TIMEOUT_MS) {
                resetParser(); // Frame timed out
            }
        }

        switch (_state) {
            case ParserState::WAIT_FOR_0X02:
                if (b == RDM6300_HEAD_BYTE) {
                    _buffer[0] = b;
                    _bufferIndex = 1;
                    _frameStartTime = millis();
                    _state = ParserState::READ_FRAME;
                }
                break;

            case ParserState::READ_FRAME:
                _buffer[_bufferIndex++] = b;
                if (_bufferIndex == RDM6300_FRAME_SIZE) {
                    if (b == RDM6300_TAIL_BYTE) {
                        _state = ParserState::VERIFY;
                    } else {
                        resetParser(); // Bad tail
                    }
                }
                break;

            case ParserState::VERIFY:
                // We shouldn't receive bytes in VERIFY state, but just in case
                break;
        }
    }

    if (_state == ParserState::VERIFY) {
        bool success = false;
        if (validateChecksum()) {
            uint8_t currentUid[5];
            convertAsciiToBinary(currentUid);
            
            unsigned long now = millis();
            bool isSameUid = (memcmp(currentUid, _lastUid, 5) == 0);
            bool timeoutPassed = (now - _lastScanTime >= RFID_DEBOUNCE_MS);

            if (!isSameUid || timeoutPassed) {
                memcpy(_lastUid, currentUid, 5);
                _lastScanTime = now;
                memcpy(uidBuffer, currentUid, 5);
                *uidLength = 5;
                success = true;
            }
        } else {
            Diagnostics::logEvent(String("[RFID] ") + _name + " Checksum error");
        }
        
        resetParser();
        return success;
    }

    return false;
}
