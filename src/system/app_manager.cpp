#include "system/app_manager.h"
#include "system/diagnostics.h"

extern SemaphoreHandle_t xMutex_MQTT;

AppManager::AppManager(WiFiManager* wifi, MQTTManager* mqtt, 
                       AccessService* access, EnvironmentService* env,
                       HeartbeatService* hb, DashboardService* ds, 
                       AttendanceManager* am)
    : _wifi(wifi), _mqtt(mqtt), _access(access), _env(env), 
      _hb(hb), _ds(ds), _am(am)
{
}

void AppManager::start() {
    // Core 0 Tasks (Networking)
    xTaskCreatePinnedToCore(vTaskNetwork, "NetworkTask", 8192, this, 2, NULL, 0);

    // Core 1 Tasks (Application Logic + Sensors)
    xTaskCreatePinnedToCore(vTaskRFID, "RFIDTask", 4096, this, 3, NULL, 1);
    xTaskCreatePinnedToCore(vTaskStatus, "StatusTask", 6144, this, 1, NULL, 1);
    xTaskCreatePinnedToCore(vTaskCoreLogic, "CoreLogic", 8192, this, 2, NULL, 1);
    
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
        vTaskDelay(50 / portTICK_PERIOD_MS); // Yield 20Hz
    }
}

void AppManager::vTaskCoreLogic(void* pvParameters) {
    AppManager* app = (AppManager*)pvParameters;
    for (;;) {
        app->_env->update();
        vTaskDelay(50 / portTICK_PERIOD_MS); // Logic engine runs at 20Hz
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

void AppManager::vTaskDiagnostics(void* pvParameters) {
    for (;;) {
#if ENABLE_DIAGNOSTICS_DASHBOARD
        Diagnostics::updateTable();
#endif
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
