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

    if (_presence->justBecameOccupied()) {
        Diagnostics::logEvent("PRESENCE: Room became occupied");
    }
    if (_presence->justBecameEmpty()) {
        Diagnostics::logEvent("PRESENCE: Room became empty");
    }
}
