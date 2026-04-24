#ifndef LOCAL_AUTH_SERVICE_H
#define LOCAL_AUTH_SERVICE_H

#include <stdint.h>

class LocalAuthService {
public:
    LocalAuthService();
    
    /**
     * Checks if a scanned UID matches any in the local Admin whitelist.
     */
    bool isAdmin(const uint8_t* uid, uint8_t uidLength);
};

#endif
