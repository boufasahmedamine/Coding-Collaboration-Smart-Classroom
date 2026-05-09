#include "services/automation/environment_service.h"
#include "system/diagnostics.h"
#include <Arduino.h>

EnvironmentService::EnvironmentService(MainsPIRInput* pir, 
                                       PresenceService* presence, 
                                       LightingLogic* lightingLogic, ProjectorLogic* projectorLogic)
    : _pir(pir), _presence(presence),
      _lightingLogic(lightingLogic), _projectorLogic(projectorLogic)
{
}

void EnvironmentService::init() {
    _pir->begin();
}

void EnvironmentService::update() {
    _pir->update();
    _presence->update();
    _lightingLogic->update();
    _projectorLogic->update();

    static unsigned long _lastPresenceLog = 0;
    unsigned long now = millis();

    if (_presence->justBecameOccupied() && (now - _lastPresenceLog > 5000)) {
        _lastPresenceLog = now;
        Diagnostics::setPresenceStatus("OCCUPIED");
        Diagnostics::logEvent(">>> PRESENCE DETECTED <<<");
    }
    if (_presence->justBecameEmpty() && (now - _lastPresenceLog > 5000)) {
        _lastPresenceLog = now;
        Diagnostics::setPresenceStatus("EMPTY");
        Diagnostics::logEvent("PRESENCE: Room became empty");
    }
}
