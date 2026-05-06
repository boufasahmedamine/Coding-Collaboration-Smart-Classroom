#include "drivers/sensors/mains_pir_input.h"
#include "system/diagnostics.h"

MainsPIRInput::MainsPIRInput(int pin)
    : _pin(pin), _motion(false), _risingEdge(false), 
      _currentPinState(HIGH), _stateChangeTime(0)
{
}

void MainsPIRInput::begin()
{
    pinMode(_pin, INPUT_PULLUP);
    _currentPinState = digitalRead(_pin);
    _stateChangeTime = millis();
    
    Diagnostics::logEvent("[PIR] Mains PIR Input Driver Initialized");
}

void MainsPIRInput::update()
{
    int rawState = digitalRead(_pin);
    unsigned long now = millis();

    // Reset edge trigger flag
    _risingEdge = false;

    // Has raw state changed from what we are currently tracking?
    if (rawState != _currentPinState) {
        _currentPinState = rawState;
        _stateChangeTime = now;
    }

    // Evaluate state stability asymmetrically
    if (_currentPinState == LOW) {
        // Optocoupler active (Motion)
        if (!_motion && (now - _stateChangeTime >= DEBOUNCE_TIME_LOW_MS)) {
            _motion = true;
            _risingEdge = true; // Emit one-time edge event
        }
    } else {
        // Optocoupler inactive (No Motion)
        if (_motion && (now - _stateChangeTime >= DEBOUNCE_TIME_HIGH_MS)) {
            _motion = false;
        }
    }
}

bool MainsPIRInput::isMotionDetected() const
{
    return _motion;
}

bool MainsPIRInput::risingEdgeDetected() const
{
    return _risingEdge;
}
