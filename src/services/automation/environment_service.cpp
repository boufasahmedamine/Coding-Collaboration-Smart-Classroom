#include "services/automation/environment_service.h"
#include "system/diagnostics.h"
#include <Arduino.h>

EnvironmentService::EnvironmentService(MainsPIRInput* pir, LDRDriver* ldr, 
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
    _pir->update();
    _ldr->update();
    Diagnostics::setLDRValue(_ldr->getLightLevel());
    _presence->update();
    _light->update();
    _controller->update();
    _lightingLogic->update();
    _projectorLogic->update();

    if (_presence->justBecameOccupied()) {
        Diagnostics::logEvent("PRESENCE: Room became occupied");
    }
    if (_presence->justBecameEmpty()) {
        Diagnostics::logEvent("PRESENCE: Room became empty");
    }
}
