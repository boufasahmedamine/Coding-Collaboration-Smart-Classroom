#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <SD.h>

class SDLogger
{
public:
    SDLogger(int csPin);

    void begin();

    void log(const char* message);

private:
    int _csPin;
};

#endif
