#ifndef LOCAL_AUTH_SERVICE_H
#define LOCAL_AUTH_SERVICE_H

#include <stdint.h>

class LocalAuthService {
public:
    enum class UserRole {
        STUDENT,
        ADMIN,
        MAINTENANCE
    };

    LocalAuthService();
    
    bool isAdmin(const uint8_t* uid, uint8_t uidLength);
    bool isMaintenance(const uint8_t* uid, uint8_t uidLength);
    UserRole getRole(const uint8_t* uid, uint8_t uidLength);
};

#endif
