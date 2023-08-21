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

    // https://github.com/me-no-dev/ESPAsyncWebServer/issues/1334#issue-1842238065
    if (!wsRequests.isEmpty())
    {
        WebsocketRequest *wr = wsRequests.shift(); // data[len] = 0; // null terminate data

        Serial.printf("Received %u bytes of binary data\n", wr->len);
        for (size_t i = 0; i < wr->len; i++)
        {
            Serial.printf("%02x ", wr->data[i]);
        }
        Serial.println();
        Serial.printf("Free heap: %u\n", ESP.getFreeHeap());

        if (!Packet::isValidPacket(wr->data, wr->len))
        {
            Serial.println("Invalid message");
            return;
        }

        byte ret = this->externalFunction(wr->data + Packet::START_MAGIC_SIZE + 1, wr->len - Packet::MESSAGE_OVERHEAD);
        sendMessageByte(0xff, ret);
        delete wr;
    }
}

void Socket::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{

    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_BINARY)
    {
        if (!wsRequests.isFull())
        {
            WebsocketRequest *wr = new WebsocketRequest;
            // Copy data to buffer
            wr->data = new uint8_t[len];
            memcpy(wr->data, data, len);

            // Copy length
            wr->len = len;

            wsRequests.push(wr);
        }
        else
        {
            Serial.println("Websocket request queue full");
        }
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

void Socket::sendMessageByte(byte type, byte val)
{
    byte buffer[] = {Packet::startMagic[0], Packet::startMagic[1], 0x2, type, val, Packet::endMagic[0], Packet::endMagic[1]};
    ws->binaryAll((char *)buffer, sizeof(buffer));
}
void Socket::sendMessageWord(byte type, uint16_t val)
{
    byte buffer[] = {Packet::startMagic[0], Packet::startMagic[1], 0x5, type, (byte)(val >> 8), (byte)(val), Packet::endMagic[0], Packet::endMagic[1]};
    ws->binaryAll((char *)buffer, sizeof(buffer));
}