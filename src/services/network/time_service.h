#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include <stdint.h>

class TimeService {
public:
    TimeService();

    /**
     * Updates the internal reference time with a new epoch timestamp from the server.
     */
    void syncTime(uint32_t epochSeconds);

    /**
     * Returns the current estimated epoch time in seconds.
     */
    uint32_t getCurrentTime();

    /**
     * Returns true if the time has been synchronized at least once.
     */
    bool isSynced();

private:
    uint32_t _baseEpoch;
    uint32_t _syncMillis;
    bool _hasSync;
};

#endif
