#include "matrix.h"

Matrix::Matrix()
{
}

Matrix::~Matrix()
{
    delete dma_display;
    delete virt_display;
}

int Matrix::setup()
{
    HUB75_I2S_CFG mxconfig(
        PANEL_RES_X,
        PANEL_RES_Y,
        PANEL_CHAIN);
    // mxconfig.double_buff = true;
    mxconfig.min_refresh_rate = 30;
    mxconfig.latch_blanking = 4;
    mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
    mxconfig.clkphase = false;

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    if (not dma_display->begin())
        return 1;
    dma_display->setBrightness8(128);
    virt_display = new VirtualMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, VIRTUAL_MATRIX_CHAIN_TYPE);
    virt_display->fillScreen(virt_display->color444(0, 0, 0));

    return 0;
}

int Matrix::loop()
{
    return 0;
}