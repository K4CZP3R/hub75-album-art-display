#include "socket.h"

Socket::Socket()
{
}

Socket::~Socket()
{
}

void Socket::setup(byte (*externalFunction)(byte *buf, int length))
{
    this->externalFunction = externalFunction;
    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");

    ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                { this->onEvent(server, client, type, arg, data, len); });

    server->addHandler(ws);
    server->begin();
}

void Socket::loop()
{
    ws->cleanupClients();
}

void Socket::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{

    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_BINARY)
    {
        data[len] = 0; // null terminate data

        Serial.printf("Received %u bytes of binary data\n", len);
        for (size_t i = 0; i < len; i++)
        {
            Serial.printf("%02x ", data[i]);
        }
        Serial.println();

        if (!Packet::isValidPacket(data, len))
        {
            Serial.println("Invalid message");
            return;
        }

        byte ret = this->externalFunction(data + Packet::START_MAGIC_SIZE + 1, len - Packet::MESSAGE_OVERHEAD);
        // wsSendMessage1(0xff, ret);
    }
}

void Socket::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
        break;
    case WS_EVT_DATA:
        this->handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}