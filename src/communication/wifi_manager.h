#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WiFiManager
{
public:
    WiFiManager(const char* ssid, const char* password);

    void begin();
    void update();

    bool isConnected();

private:
    const char* _ssid;
    const char* _password;
};

#endif
