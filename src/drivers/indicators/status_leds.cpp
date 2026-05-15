#include "drivers/indicators/status_leds.h"

StatusLEDs::StatusLEDs(uint8_t redPin, uint8_t greenPin)
    : _redPin(redPin), _greenPin(greenPin), _inPulse(false), _pulseStartTime(0), _pulseDuration(0) {
}

void StatusLEDs::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    showStandby();
}

void StatusLEDs::showStandby() {
    _inPulse = false;
    setRed(true);
    setGreen(false);
}

void StatusLEDs::pulseSuccess(unsigned long durationMs) {
    _inPulse = true;
    _pulseStartTime = millis();
    _pulseDuration = durationMs;
    
    setRed(false);
    setGreen(true);
}

void StatusLEDs::update() {
    if (_inPulse) {
        if (millis() - _pulseStartTime >= _pulseDuration) {
            showStandby();
        }
    }
}

void StatusLEDs::setRed(bool on) {
    digitalWrite(_redPin, on ? HIGH : LOW);
}

void StatusLEDs::setGreen(bool on) {
    digitalWrite(_greenPin, on ? HIGH : LOW);
}
