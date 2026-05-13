#include "drivers/sensors/button_driver.h"
#include <Arduino.h>

ButtonDriver::ButtonDriver(int pin, bool activeLow)
    : _pin(pin), _activeLow(activeLow), _lastState(false), _lastDebounceTime(0) {
}

void ButtonDriver::begin() {
    if (_activeLow) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT_PULLDOWN);
    }
    // Initial state read
    _lastState = (digitalRead(_pin) == (_activeLow ? LOW : HIGH));
}

bool ButtonDriver::isPressed() {
    bool currentState = (digitalRead(_pin) == (_activeLow ? LOW : HIGH));
    bool triggered = false;

    if (currentState != _lastState) {
        if ((millis() - _lastDebounceTime) > _debounceDelay) {
            if (currentState) {
                triggered = true; // State changed to pressed
            }
            _lastState = currentState;
            _lastDebounceTime = millis();
        }
    }

    return triggered;
}
