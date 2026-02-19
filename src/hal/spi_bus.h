#ifndef SPI_BUS_H
#define SPI_BUS_H

#include <stdint.h>

class SpiBus
{
public:
    virtual ~SpiBus() = default;

    virtual bool init(uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin, uint32_t frequency = 1000000) = 0;
    virtual uint8_t transfer(uint8_t data) = 0;
    virtual void transferBytes(const uint8_t* txData, uint8_t* rxData, size_t length) = 0;
    virtual void beginTransaction() = 0;
    virtual void endTransaction() = 0;
};

SpiBus* createSpiBus();

#endif // SPI_BUS_H