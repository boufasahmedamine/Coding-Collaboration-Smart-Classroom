#ifndef AUTOMATION_CONTROLLER_H
#define AUTOMATION_CONTROLLER_H

#include "services/automation/presence_service.h"
#include "services/automation/light_service.h"

class AutomationController
{
public:
    AutomationController(PresenceService* presence, LightService* light);

    void update();

private:
    PresenceService* _presence;
    LightService* _light;

    bool _lightsOn;
};

#endif
