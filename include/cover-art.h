#ifndef covertArt_h
#define covertArt_h

#include <Arduino.h>
#include "matrix.h"

class CoverArt
{
public:
    CoverArt(Matrix *matrix);
    ~CoverArt();
    byte handleMessage(byte *buffer, int length);

private:
    byte handleFillScreen(byte r, byte g, byte b);
    byte handleDrawPixel(byte x, byte y, byte r, byte g, byte b);
    byte handleClearScreen();
    byte handleDrawChar(byte x, byte y, byte c, byte r, byte g, byte b, byte br, byte bg, byte bb, byte size);
    byte handleInitBitmap(byte width, byte height);
    byte handleFeedBitmap(byte *buffer, int length);
    byte handleDrawBitmap(byte x, byte y);
    byte handleSetBrightness(byte brightness);
    Matrix *matrix = NULL;
    uint16_t *rgbBitmap = NULL;
    int rgbBitmapIdx = 0;
    int rgbBitmapWidth = 0;
    int rgbBitmapHeight = 0;
};

#endif