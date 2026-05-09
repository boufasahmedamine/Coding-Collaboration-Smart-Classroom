#ifndef MAINS_PIR_INPUT_H
#define MAINS_PIR_INPUT_H

#include <Arduino.h>

class MainsPIRInput {
public:
    MainsPIRInput(int pin);

    void begin();
    void update();

    bool isMotionDetected() const;
    bool risingEdgeDetected() const;

private:
    int _pin;
    bool _motion;
    bool _risingEdge;
    
    // Low level pin state
    int _currentPinState;
    unsigned long _stateChangeTime;
    
    // Asymmetric timing constants
    static const unsigned long DEBOUNCE_TIME_LOW_MS = 500;  // Half second for stability
    static const unsigned long DEBOUNCE_TIME_HIGH_MS = 3000; // 3 seconds to clear chatter
};

#endif
