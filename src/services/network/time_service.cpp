#include "services/network/time_service.h"
#include <Arduino.h>

TimeService::TimeService()
    : _baseEpoch(0), _syncMillis(0), _hasSync(false)
{
}

void TimeService::syncTime(uint32_t epochSeconds)
{
    _baseEpoch = epochSeconds;
    _syncMillis = millis();
    _hasSync = true;
    Serial.printf("[TIME] Synced to epoch: %u\n", _baseEpoch);
}

uint32_t TimeService::getCurrentTime()
{
    if (!_hasSync) {
        return 0;
    }

    uint32_t elapsedMillis = millis() - _syncMillis;
    return _baseEpoch + (elapsedMillis / 1000);
}

bool TimeService::isSynced()
{
    return _hasSync;
}
