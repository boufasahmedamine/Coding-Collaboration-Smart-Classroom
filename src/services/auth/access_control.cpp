#include "services/auth/access_control.h"

AccessControl::AccessControl() {}

bool AccessControl::isAuthorized(uint8_t* uid, uint8_t uidLength) {

    for (int i = 0; i < AUTH_UID_COUNT; i++) {

        bool match = true;

        for (int j = 0; j < uidLength; j++) {

            if (uid[j] != _authorizedUIDs[i][j]) {
                match = false;
                break;
            }
        }

        if (match) {
            return true;
        }
    }

    return false;
}
