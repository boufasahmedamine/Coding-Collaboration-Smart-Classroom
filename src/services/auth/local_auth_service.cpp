#include "services/auth/local_auth_service.h"
#include "config/admin_config.h"
#include <string.h>

LocalAuthService::LocalAuthService() {
}

bool LocalAuthService::isAdmin(const uint8_t* uid, uint8_t uidLength) {
    if (uid == nullptr || uidLength == 0) return false;

    for (int i = 0; i < ADMIN_COUNT; i++) {
        if (memcmp(uid, ADMIN_UIDS[i], uidLength) == 0) {
            return true;
        }
    }
    return false;
}

bool LocalAuthService::isMaintenance(const uint8_t* uid, uint8_t uidLength) {
    if (uid == nullptr || uidLength == 0) return false;

    for (int i = 0; i < MAINTENANCE_COUNT; i++) {
        if (memcmp(uid, MAINTENANCE_UIDS[i], uidLength) == 0) {
            return true;
        }
    }
    return false;
}

LocalAuthService::UserRole LocalAuthService::getRole(const uint8_t* uid, uint8_t uidLength) {
    if (isAdmin(uid, uidLength)) return UserRole::ADMIN;
    if (isMaintenance(uid, uidLength)) return UserRole::MAINTENANCE;
    return UserRole::STUDENT;
}
