#ifndef ENVIRONMENT_SERVICE_H
#define ENVIRONMENT_SERVICE_H

#include "drivers/sensors/mains_pir_input.h"
#include "services/automation/presence_service.h"
#include "services/automation/lighting_logic.h"
#include "services/automation/projector_logic.h"

class EnvironmentService {
public:
    EnvironmentService(MainsPIRInput* pir, 
                       PresenceService* presence, 
                       LightingLogic* lightingLogic, ProjectorLogic* projectorLogic);

    void init();
    void update();

private:
    MainsPIRInput* _pir;
    PresenceService* _presence;
    LightingLogic* _lightingLogic;
    ProjectorLogic* _projectorLogic;
};

#endif
