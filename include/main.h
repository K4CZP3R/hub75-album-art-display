#include <Arduino.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#include <FastLED.h>

#define BAUD_RATE 115200 // serial debug port baud rate

#define PANEL_RES_X 32                 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 16                 // Number of pixels tall of each INDIVIDUAL panel module.
#define NUM_ROWS 2                     // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 1                     // Number of INDIVIDUAL PANELS per ROW
#define PANEL_CHAIN NUM_ROWS *NUM_COLS // total number of panels chained one to another
#define VIRTUAL_MATRIX_CHAIN_TYPE CHAIN_BOTTOM_LEFT_UP
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 32

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define WIFI_SSID "PanicAtTheCiscoEXT"
#define WIFI_PASS "aQkkVzctkrj6"