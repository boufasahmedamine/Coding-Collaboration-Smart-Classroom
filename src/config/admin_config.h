#ifndef ADMIN_CONFIG_H
#define ADMIN_CONFIG_H

#include <stdint.h>

/**
 * --- ADMIN BYPASS WHITELIST ---
 * These UIDs will unlock the door instantly without server approval.
 * Format: {UID bytes}
 */
const uint8_t ADMIN_UIDS[][7] = {
    {0x04, 0x5F, 0x32, 0x1A}, // Placeholder Admin 1
    {0xDE, 0xAD, 0xBE, 0xEF}  // Placeholder Admin 2
};

const int ADMIN_COUNT = 2;

#endif
