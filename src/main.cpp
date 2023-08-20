#include "main.h"
#include "USB.h"

MatrixPanel_I2S_DMA *dma_display = nullptr;
VirtualMatrixPanel *virt_display = nullptr;

const int START_MAGIC_SIZE = 2;
const int END_MAGIC_SIZE = 2;
const int MESSAGE_OVERHEAD = START_MAGIC_SIZE + 1 + END_MAGIC_SIZE;
const byte startMagic[START_MAGIC_SIZE] = {0x13, 0x37};
const byte endMagic[END_MAGIC_SIZE] = {0xde, 0xad};

byte *msgBuffer = NULL;
uint16_t *rgbBitmap = NULL;
int rgbBitmapWidth = 0;
int rgbBitmapHeight = 0;
int rgbBitmapCurrentIdx = 0;

int messageIdx = 0;
int expectedMessageLen = 0;

enum ReadingState
{
    WAITING,
    READING_MESSAGE,
    CHECK_END_DELIMITER
};

ReadingState currentState = WAITING;

void sendMessage4(byte type, uint32_t val)
{
    Serial.write(startMagic, START_MAGIC_SIZE);
    Serial.write(0x5); // message length
    Serial.write(type);
    Serial.write((byte *)&val, sizeof(val));
    Serial.write(endMagic, END_MAGIC_SIZE);
}
void sendMessage1(byte type, byte val)
{
    Serial.write(startMagic, START_MAGIC_SIZE);
    Serial.write(0x2); // message length
    Serial.write(type);
    Serial.write(val);
    Serial.write(endMagic, END_MAGIC_SIZE);
}

void screen_setup()
{

    HUB75_I2S_CFG mxconfig(
        PANEL_RES_X, // module width
        PANEL_RES_Y, // module height
        PANEL_CHAIN  // chain length
    );

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->setBrightness8(192);

    if (not dma_display->begin())
        sendMessage1(0xf0, 0x1);

    // create VirtualDisplay object based on our newly created dma_display object
    virt_display = new VirtualMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, VIRTUAL_MATRIX_CHAIN_TYPE);
    // So far so good, so continue
    virt_display->fillScreen(virt_display->color444(0, 0, 0));
}

void setup()
{
    Serial.begin(BAUD_RATE);
    screen_setup();
}

byte handleMessage(byte *buffer, int length)
{
    byte functionType = buffer[0];

    if (functionType == 0x01)
    {
        // Check if the message is long enough to contain the RGB values
        if (length < 4)
        {
            return 0x2;
        }
        byte r = buffer[1];
        byte g = buffer[2];
        byte b = buffer[3];

        virt_display->fillScreen(virt_display->color444(r, g, b));
    }
    if (functionType == 0x2)
    {
        if (length < 6)
        {
            return 0x2;
        }
        virt_display->drawPixel(buffer[1], buffer[2], virt_display->color444(buffer[3], buffer[4], buffer[5]));
    }
    if (functionType == 0x3)
    {
        virt_display->clearScreen();
    }
    if (functionType == 0x4)
    {
        if (length < 11)
        {
            return 0x2;
        }
        virt_display->drawChar(
            buffer[1],
            buffer[2],
            buffer[3],
            virt_display->color444(buffer[4], buffer[5], buffer[6]),
            virt_display->color444(buffer[7], buffer[8], buffer[9]),
            buffer[10]);
    }
    if (functionType == 0x5)
    {
        rgbBitmapWidth = buffer[1];
        rgbBitmapHeight = buffer[2];

        // Cleanup old bitmap
        if (rgbBitmap != NULL)
        {
            delete[] rgbBitmap;
        }

        sendMessage4(0x70, ESP.getFreeHeap());
        rgbBitmapCurrentIdx = 0;
        rgbBitmap = new uint16_t[rgbBitmapWidth * rgbBitmapHeight];
    }
    if (functionType == 0x6)
    {

        // Feed bitmap array starting from idx
        for (int i = 1; i < length; i += 3)
        {
            byte r = buffer[i];
            byte g = buffer[i + 1];
            byte b = buffer[i + 2];
            Serial.printf("rgbBitmapCurrentIdx: %d\n", rgbBitmapCurrentIdx);
            rgbBitmap[rgbBitmapCurrentIdx] = virt_display->color565(r, g, b);

            rgbBitmapCurrentIdx++;
        }

        sendMessage4(0x71, ESP.getFreeHeap());
    }
    if (functionType == 0x7)
    {
        if (rgbBitmap == NULL)
        {
            return 0x1;
        }

        int startX = buffer[1];
        int startY = buffer[2];

        virt_display->drawRGBBitmap(startX, startY, rgbBitmap, rgbBitmapWidth, rgbBitmapHeight);

        // delete[] rgbBitmap;
    }

    return 0x0;
}

void loop()
{
    while (Serial.available())
    {
        byte incomingByte = Serial.read();

        switch (currentState)
        {
        case WAITING:
            if (incomingByte == startMagic[messageIdx])
            {
                messageIdx++;

                if (messageIdx == START_MAGIC_SIZE)
                {
                    currentState = READING_MESSAGE;
                    messageIdx = 0;
                }
            }
            else
            {
                messageIdx = 0;
            }
            break;
        case READING_MESSAGE:
            if (messageIdx == 0)
            {
                expectedMessageLen = incomingByte;

                if (msgBuffer != NULL)
                {
                    delete[] msgBuffer;
                }

                msgBuffer = new byte[expectedMessageLen];
            }
            else
            {
                msgBuffer[messageIdx - 1] = incomingByte;
                if (messageIdx - 1 == expectedMessageLen - 1)
                {
                    currentState = CHECK_END_DELIMITER;
                }
            }

            messageIdx++;
            break;
        case CHECK_END_DELIMITER:
            if (incomingByte == endMagic[messageIdx - expectedMessageLen - 1])
            {
                messageIdx++;

                if (messageIdx - expectedMessageLen == END_MAGIC_SIZE)
                {
                    byte ret = handleMessage(msgBuffer, expectedMessageLen);
                    sendMessage1(0xff, ret);

                    delete[] msgBuffer;
                    msgBuffer = NULL;

                    currentState = WAITING;
                    messageIdx = 0;
                }
            }
            else
            {
                currentState = WAITING;
                messageIdx = 0;
            }
            break;
        }
    }
}