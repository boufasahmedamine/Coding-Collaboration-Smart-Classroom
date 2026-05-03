#include "services/automation/environment_service.h"
#include "system/diagnostics.h"
#include <Arduino.h>

EnvironmentService::EnvironmentService(PIRDriver* pir, LDRDriver* ldr, 
                                       PresenceService* presence, LightService* light,
                                       AutomationController* controller, 
                                       LightingLogic* lightingLogic, ProjectorLogic* projectorLogic)
    : _pir(pir), _ldr(ldr), _presence(presence), _light(light),
      _controller(controller), _lightingLogic(lightingLogic), _projectorLogic(projectorLogic)
{
}

void EnvironmentService::init() {
    _pir->begin();
    _ldr->begin();
}

void EnvironmentService::update() {
    // Update Sensors
    _pir->update();
    _ldr->update();
    
    // Update Diagnostics
    Diagnostics::setLDRValue(_ldr->getLightLevel());

    // Update Services
    _presence->update();
    _light->update();
    
    // Update Logic
    _controller->update();
    _lightingLogic->update();
    _projectorLogic->update();

    // Log Presence changes
    if (_presence->justBecameOccupied()) {
        Diagnostics::logEvent("PRESENCE: Room became occupied");
    }
    if (_presence->justBecameEmpty()) {
        Diagnostics::logEvent("PRESENCE: Room became empty");
    }
}
