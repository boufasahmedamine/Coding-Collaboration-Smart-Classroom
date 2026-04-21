#ifndef DASHboard_SERVICE_H
#define DASHboard_SERVICE_H

#include "communication/mqtt_manager.h"
#include "system/state_machine.h"
#include "services/automation/presence_service.h"
#include "drivers/actuators/lighting.h"
#include "drivers/actuators/ir_projector.h"
#include <ArduinoJson.h>

class DashboardService {
public:
    DashboardService(MQTTManager* mqtt, StateMachine* sm, PresenceService* presence, Lighting* lights, IRProjector* projector);

    /**
     * Periodically broadcasts the classroom state.
     * @param force If true, sends immediately regardless of interval.
     */
    void update(bool force = false);

private:
    MQTTManager* _mqtt;
    StateMachine* _sm;
    PresenceService* _presence;
    Lighting* _lights;
    IRProjector* _projector;

    unsigned long _lastBroadcast;
    const unsigned long _interval = 5000; // 5 seconds
};

#endif
