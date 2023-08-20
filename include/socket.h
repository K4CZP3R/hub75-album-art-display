#ifndef socket_h
#define socket_h

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "packet.h"

class Socket
{
public:
    Socket();
    ~Socket();

    void setup(byte (*externalFunction)(byte *buf, int length));
    void loop();

private:
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    AsyncWebServer *server = NULL;
    AsyncWebSocket *ws = NULL;
    byte (*externalFunction)(byte *buf, int length);
};

#endif