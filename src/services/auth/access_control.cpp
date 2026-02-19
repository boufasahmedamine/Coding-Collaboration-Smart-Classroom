#include "access_control.h"
#include <cstring>  // For memcmp

AccessControlService::AccessControlService()
    : _initialized(false)
{
}

AccessControlService::~AccessControlService()
{
    // Cleanup
}

bool AccessControlService::init()
{
    // Load authorized UIDs from storage (SD card or flash)
    if (!loadFromStorage()) {
        // Add default UIDs for testing
        uint8_t defaultUid[] = {0x12, 0x34, 0x56, 0x78};
        addAuthorizedUid(defaultUid, sizeof(defaultUid));
    }

    _initialized = true;
    return true;
}

bool AccessControlService::addAuthorizedUid(const uint8_t* uid, size_t length)
{
    if (length == 0 || length > 10) return false;

    std::vector<uint8_t> newUid(uid, uid + length);
    _authorizedUids.push_back(newUid);
    return saveToStorage();
}

bool AccessControlService::removeAuthorizedUid(const uint8_t* uid, size_t length)
{
    for (auto it = _authorizedUids.begin(); it != _authorizedUids.end(); ++it) {
        if (it->size() == length && memcmp(it->data(), uid, length) == 0) {
            _authorizedUids.erase(it);
            return saveToStorage();
        }
    }
    return false;
}

bool AccessControlService::isAuthorized(const uint8_t* uid, size_t length)
{
    for (const auto& authUid : _authorizedUids) {
        if (authUid.size() == length && memcmp(authUid.data(), uid, length) == 0) {
            return true;
        }
    }
    return false;
}

AccessEvent AccessControlService::processUid(const uint8_t* uid, size_t length)
{
    AccessEvent event;
    memcpy(event.uid, uid, length);
    event.uidLength = length;
    event.authenticated = isAuthorized(uid, length);
    return event;
}

bool AccessControlService::loadFromStorage()
{
    // Placeholder: Load from SD card or flash
    // For now, return false to use defaults
    return false;
}

bool AccessControlService::saveToStorage()
{
    // Placeholder: Save to SD card or flash
    return true;
}