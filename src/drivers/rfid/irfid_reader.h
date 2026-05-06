#ifndef IRFID_READER_H
#define IRFID_READER_H

#include <stdint.h>

class IRFIDReader {
public:
    virtual bool init() = 0;
    virtual bool isInitialized() const = 0;
    virtual bool isAlive() = 0;
    
    // Reads a card and returns true if a new, valid scan occurred.
    // uidBuffer must be at least 7 bytes (though RDM6300 will use 5).
    virtual bool readCard(uint8_t* uidBuffer, uint8_t* uidLength) = 0;
    
    virtual void resetCommunication() = 0;
    virtual ~IRFIDReader() = default;
};

#endif
