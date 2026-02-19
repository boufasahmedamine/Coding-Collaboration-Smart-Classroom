#include "uart_bus.h"
#include <HardwareSerial.h>

class ArduinoUartBus : public UartBus
{
private:
    HardwareSerial* _serial;

public:
    ArduinoUartBus() : _serial(&Serial1) {}  // Use Serial1 for peripherals

    bool init(uint8_t txPin, uint8_t rxPin, uint32_t baudRate = 115200) override
    {
        _serial->begin(baudRate, SERIAL_8N1, rxPin, txPin);
        return true;
    }

    bool writeByte(uint8_t data) override
    {
        return _serial->write(data) == 1;
    }

    bool writeBytes(const uint8_t* data, size_t length) override
    {
        return _serial->write(data, length) == length;
    }

    int readByte() override
    {
        if (_serial->available() > 0) {
            return _serial->read();
        }
        return -1;
    }

    size_t readBytes(uint8_t* buffer, size_t maxLength) override
    {
        return _serial->readBytes(buffer, maxLength);
    }

    size_t available() override
    {
        return _serial->available();
    }

    void flush() override
    {
        _serial->flush();
    }
};

UartBus* createUartBus()
{
    static ArduinoUartBus instance;
    return &instance;
}