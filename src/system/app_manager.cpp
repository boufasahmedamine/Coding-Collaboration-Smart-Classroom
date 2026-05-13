#include "system/app_manager.h"
#include "config/pins.h"
#include "system/diagnostics.h"

extern SemaphoreHandle_t xMutex_MQTT;

AppManager::AppManager(WiFiManager* wifi, MQTTManager* mqtt, 
                       AccessService* access, EnvironmentService* env,
                       HeartbeatService* hb, DashboardService* ds, 
                       AttendanceManager* am, StateMachine* sm, LightingLogic* ll)
    : _wifi(wifi), _mqtt(mqtt), _access(access), _env(env), 
      _hb(hb), _ds(ds), _am(am), _stateMachine(sm), _lightingLogic(ll),
      _exitBtn(PIN_EXIT_BUTTON), _lightBtn(PIN_LIGHT_BUTTON)
{
}

void AppManager::start() {
    xTaskCreatePinnedToCore(vTaskNetwork, "NetworkTask", 8192, this, 2, NULL, 0);
    xTaskCreatePinnedToCore(vTaskRFID, "RFIDTask", 4096, this, 3, NULL, 1);
    xTaskCreatePinnedToCore(vTaskStatus, "StatusTask", 6144, this, 1, NULL, 1);
    xTaskCreatePinnedToCore(vTaskCoreLogic, "CoreLogic", 8192, this, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskButtons, "Buttons", 2048, this, 2, NULL, 1);
#if ENABLE_DIAGNOSTICS_DASHBOARD
    xTaskCreatePinnedToCore(vTaskDiagnostics, "Diagnostics", 4096, NULL, 1, NULL, 1);
#endif
}

void AppManager::vTaskNetwork(void* pvParameters) {
    AppManager* app = (AppManager*)pvParameters;
    for (;;) {
        app->_wifi->update();
        if (xSemaphoreTake(xMutex_MQTT, portMAX_DELAY) == pdTRUE) {
            app->_mqtt->update();
            xSemaphoreGive(xMutex_MQTT);
        }
        app->_hb->update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void AppManager::vTaskRFID(void* pvParameters) {
    AppManager* app = (AppManager*)pvParameters;
    for (;;) {
        app->_access->update();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void AppManager::vTaskCoreLogic(void* pvParameters) {
    AppManager* app = (AppManager*)pvParameters;
    for (;;) {
        app->_env->update();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void AppManager::vTaskStatus(void* pvParameters) {
    AppManager* app = (AppManager*)pvParameters;
    for (;;) {
        app->_am->update();
        app->_ds->update();
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
    }
}

void AppManager::vTaskButtons(void* pvParameters) {
    AppManager* app = (AppManager*)pvParameters;
    app->_exitBtn.begin();
    app->_lightBtn.begin();
    
    extern SemaphoreHandle_t xMutex_StateMachine;

    for (;;) {
        if (app->_exitBtn.isPressed()) {
            Diagnostics::logEvent("[BTN] Force Exit Triggered");
            if (xSemaphoreTake(xMutex_StateMachine, portMAX_DELAY) == pdTRUE) {
                app->_stateMachine->handleEvent(StateMachine::SystemEvent::UNLOCK_REQUEST);
                xSemaphoreGive(xMutex_StateMachine);
            }
        }

        if (app->_lightBtn.isPressed()) {
            Diagnostics::logEvent("[BTN] Force Light Toggle Triggered");
            app->_lightingLogic->toggleManualOverride();
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void AppManager::vTaskDiagnostics(void* pvParameters) {
    for (;;) {
#if ENABLE_DIAGNOSTICS_DASHBOARD
        Diagnostics::updateTable();
#endif
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
