#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <stdint.h>

class GpioHal
{
public:
  enum class Direction
  {
    In,
    Out
  };

    virtual ~GpioHal() = default;

    virtual void configure(uint8_t pin, Direction dir) = 0;
    virtual void write(uint8_t pin, bool level) = 0;
    virtual bool read(uint8_t pin) = 0;  // Added for input pins
};

GpioHal* createGpioHal();

#endif
