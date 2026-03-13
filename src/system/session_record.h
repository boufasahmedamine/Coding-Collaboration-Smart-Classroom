#ifndef SESSION_RECORD_H
#define SESSION_RECORD_H

#include <stdint.h>

struct SessionRecord {
    uint8_t uid[7];          // UID of card that started session
    uint8_t uidLength;       // UID length
    unsigned long startTime; // millis() at session start
    unsigned long endTime;   // millis() at session end
    bool active;

    // Logging flags
    bool loggedStart;
    bool loggedEnd;
};

#endif
