#ifndef ENVIRONMENT_SERVICE_H
#define ENVIRONMENT_SERVICE_H

#include "drivers/pir/pir_driver.h"
#include "drivers/ldr/ldr_driver.h"
#include "services/automation/presence_service.h"
#include "services/automation/light_service.h"
#include "services/automation/automation_controller.h"
#include "services/automation/lighting_logic.h"
#include "services/automation/projector_logic.h"

class EnvironmentService {
public:
    EnvironmentService(PIRDriver* pir, LDRDriver* ldr, 
                       PresenceService* presence, LightService* light,
                       AutomationController* controller, 
                       LightingLogic* lightingLogic, ProjectorLogic* projectorLogic);

    void init();
    void update();

private:
    PIRDriver* _pir;
    LDRDriver* _ldr;
    PresenceService* _presence;
    LightService* _light;
    AutomationController* _controller;
    LightingLogic* _lightingLogic;
    ProjectorLogic* _projectorLogic;
};

#endif
