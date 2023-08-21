#include "cover-art.h"

CoverArt::CoverArt(Matrix *matrix)
{
    this->matrix = matrix;
}

CoverArt::~CoverArt()
{
    if (rgbBitmap != NULL)
    {
        delete[] rgbBitmap;
        rgbBitmap = NULL;
    }
}

byte CoverArt::handleMessage(byte *buffer, int length)
{
    if (length < 1)
        return 1;

    byte functionType = buffer[0];
    switch (functionType)
    {
    case 1:
        if (length < 4)
            return 1;
        return handleFillScreen(buffer[1], buffer[2], buffer[3]);
    case 2:
        if (length < 6)
            return 1;
        return handleDrawPixel(buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    case 3:
        return handleClearScreen();
    case 4:
        if (length < 11)
            return 1;
        return handleDrawChar(buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10]);
    case 5:
        if (length < 3)
            return 1;
        return handleInitBitmap(buffer[1], buffer[2]);
    case 6:
        return handleFeedBitmap(buffer + 1, length - 1);
    case 7:
        if (length < 3)
            return 1;
        return handleDrawBitmap(buffer[1], buffer[2]);
    case 8:
        if (length < 2)
            return 1;
        return handleSetBrightness(buffer[1]);
    case 9:
        if (length < 8)
            return 1;
        return handleDrawLine(buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
    default:
        return 1;
    }
}

byte CoverArt::handleDrawLine(byte x0, byte y0, byte x1, byte y1, byte r, byte g, byte b)
{

    matrix->getVirtDisplay()->drawLine(x0, y0, x1, y1, matrix->getVirtDisplay()->color444(r, g, b));
    return 0;
}

byte CoverArt::handleFillScreen(byte r, byte g, byte b)
{
    matrix->getVirtDisplay()->fillScreen(matrix->getVirtDisplay()->color444(r, g, b));
    return 0;
}
byte CoverArt::handleDrawPixel(byte x, byte y, byte r, byte g, byte b)
{
    matrix->getVirtDisplay()->drawPixel(x, y, matrix->getVirtDisplay()->color444(r, g, b));
    return 0;
}
byte CoverArt::handleClearScreen()
{
    matrix->getVirtDisplay()->clearScreen();
    return 0;
}
byte CoverArt::handleDrawChar(byte x, byte y, byte c, byte r, byte g, byte b, byte br, byte bg, byte bb, byte size)
{
    matrix->getVirtDisplay()->drawChar(
        x, y, c, matrix->getVirtDisplay()->color444(r, g, b),
        matrix->getVirtDisplay()->color444(br, bg, bb), size, 1);

    return 0;
}
byte CoverArt::handleInitBitmap(byte width, byte height)
{
    if (rgbBitmap == NULL)
    {
        delete[] rgbBitmap;
        rgbBitmap = NULL;
    }
    rgbBitmapIdx = 0;
    rgbBitmapWidth = width;
    rgbBitmapHeight = height;
    rgbBitmap = new uint16_t[width * height];
    return 0;
}
byte CoverArt::handleFeedBitmap(byte *buffer, int length)
{
    if (rgbBitmap == NULL)
        return 1;
    for (int i = 0; i < length; i += 3)
    {
        byte r = buffer[i];
        byte g = buffer[i + 1];
        byte b = buffer[i + 2];
        rgbBitmap[rgbBitmapIdx] = matrix->getVirtDisplay()->color565(r, g, b);
        rgbBitmapIdx++;
    }

    return 0;
}
byte CoverArt::handleDrawBitmap(byte x, byte y)
{
    if (rgbBitmap == NULL)
    {
        return 1;
    }

    matrix->getVirtDisplay()->drawRGBBitmap(x, y, rgbBitmap, rgbBitmapWidth, rgbBitmapHeight);

    delete[] rgbBitmap;
    rgbBitmap = NULL;

    return 0;
}
byte CoverArt::handleSetBrightness(byte brightness)
{
    matrix->getDmaDisplay()->setBrightness8(brightness);
    return 0;
}