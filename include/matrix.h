#ifndef matrix_h
#define matrix_h

#include "config.h"

#include <Arduino.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#include <FastLED.h>

class Matrix
{
public:
    Matrix();
    ~Matrix();
    int setup();
    int loop();

    // Getters
    MatrixPanel_I2S_DMA *getDmaDisplay() { return dma_display; }
    VirtualMatrixPanel *getVirtDisplay() { return virt_display; }

private:
    MatrixPanel_I2S_DMA *dma_display = nullptr;
    VirtualMatrixPanel *virt_display = nullptr;
};

#endif