#ifndef RDM6300_H
#define RDM6300_H

#include "drivers/rfid/irfid_reader.h"
#include <Arduino.h>

// RDM6300 Frame Constants
#define RDM6300_BAUDRATE 9600
#define RDM6300_HEAD_BYTE 0x02
#define RDM6300_TAIL_BYTE 0x03
#define RDM6300_FRAME_SIZE 14
#define RDM6300_TIMEOUT_MS 50

#define RFID_DEBOUNCE_MS 2000

class RDM6300Driver : public IRFIDReader {
public:
    RDM6300Driver(HardwareSerial* serialPort, uint8_t rxPin, const char* name);
    virtual ~RDM6300Driver() = default;

    bool init() override;
    bool isInitialized() const override;
    bool isAlive() override;
    bool readCard(uint8_t* uidBuffer, uint8_t* uidLength) override;
    void resetCommunication() override;

private:
    enum class ParserState {
        WAIT_FOR_0X02,
        READ_FRAME,
        VERIFY
    };

    HardwareSerial* _serial;
    uint8_t _rxPin;
    const char* _name;
    bool _initialized;

    ParserState _state;
    uint8_t _buffer[RDM6300_FRAME_SIZE];
    uint8_t _bufferIndex;
    unsigned long _frameStartTime;

    // Debounce state
    uint8_t _lastUid[5];
    unsigned long _lastScanTime;

    void resetParser();
    bool processByte(uint8_t b);
    bool validateChecksum();
    void convertAsciiToBinary(uint8_t* binaryUid);
    uint8_t asciiHexToByte(char c1, char c2);
};

#endif
