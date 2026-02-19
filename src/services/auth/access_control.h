#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <stdint.h>
#include <vector>

struct AccessEvent
{
    uint8_t uid[10];
    size_t uidLength;
    bool authenticated;
};

class AccessControlService
{
public:
    AccessControlService();
    ~AccessControlService();

    bool init();
    bool addAuthorizedUid(const uint8_t* uid, size_t length);
    bool removeAuthorizedUid(const uint8_t* uid, size_t length);
    bool isAuthorized(const uint8_t* uid, size_t length);
    AccessEvent processUid(const uint8_t* uid, size_t length);

private:
    std::vector<std::vector<uint8_t>> _authorizedUids;
    bool _initialized;

    bool loadFromStorage();  // Placeholder for SD card
    bool saveToStorage();    // Placeholder
};

#endif // ACCESS_CONTROL_H