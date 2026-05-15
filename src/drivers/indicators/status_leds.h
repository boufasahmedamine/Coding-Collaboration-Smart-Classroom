#ifndef STATUS_LEDS_H
#define STATUS_LEDS_H

#include <Arduino.h>

class StatusLEDs {
public:
    StatusLEDs(uint8_t redPin, uint8_t greenPin);

    void begin();
    void update();

    void showStandby();
    void pulseSuccess(unsigned long durationMs = 2000);

private:
    uint8_t _redPin;
    uint8_t _greenPin;

    bool _inPulse;
    unsigned long _pulseStartTime;
    unsigned long _pulseDuration;

    void setRed(bool on);
    void setGreen(bool on);
};

#endif
