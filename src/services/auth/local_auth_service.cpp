#include "services/auth/local_auth_service.h"
#include "config/admin_config.h"
#include <string.h>

LocalAuthService::LocalAuthService() {
}

bool LocalAuthService::isAdmin(const uint8_t* uid, uint8_t uidLength) {
    if (uid == nullptr || uidLength == 0) return false;

    for (int i = 0; i < ADMIN_COUNT; i++) {
        // Compare bytes up to uidLength
        if (memcmp(uid, ADMIN_UIDS[i], uidLength) == 0) {
            return true;
        }
    }
    return false;
}
