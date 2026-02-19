#ifndef UART_BUS_H
#define UART_BUS_H

#include <stdint.h>
#include <vector>

class UartBus
{
public:
    virtual ~UartBus() = default;

    virtual bool init(uint8_t txPin, uint8_t rxPin, uint32_t baudRate = 115200) = 0;
    virtual bool writeByte(uint8_t data) = 0;
    virtual bool writeBytes(const uint8_t* data, size_t length) = 0;
    virtual int readByte() = 0;  // Returns -1 if no data
    virtual size_t readBytes(uint8_t* buffer, size_t maxLength) = 0;
    virtual size_t available() = 0;
    virtual void flush() = 0;
};

UartBus* createUartBus();

#endif // UART_BUS_H