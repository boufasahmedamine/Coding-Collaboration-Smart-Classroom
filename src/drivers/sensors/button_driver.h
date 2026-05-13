#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdint.h>

class ButtonDriver {
public:
    ButtonDriver(int pin, bool activeLow = true);
    void begin();
    
    // Non-blocking update, returns true if a valid "click" (press and release) happened.
    // For this simple override, we'll return true on the leading edge (press).
    bool isPressed();

private:
    int _pin;
    bool _activeLow;
    bool _lastState;
    uint32_t _lastDebounceTime;
    const uint32_t _debounceDelay = 50; 
};

#endif
