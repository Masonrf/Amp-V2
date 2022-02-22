/*
 *  Functions related to the display for the amplifier
 */ 
#ifndef AmpDisplay_h
#define AmpDisplay_h

#include "EasyNextionLibrary.h"
#include "AmpControl.h"
#include "AmpADC.h"

#define MAIN_PAGE       0
#define INPUTS_PAGE     1
#define OUTPUTS_PAGE    2
#define FAN_PAGE        3
#define FFT_PAGE        4

// Refresh rate in frames per second
// probably can't really go higher than this for FFT page purposes
// Keep at 30. For some reason 30 causes the least tearing out of all the fps values I have tried.
#define MAX_REFRESH_RATE 30

#define RMS_MIN_VAL_DB  26
#define RMS_MAX_VAL_DB  66

#define SERIAL_PORT     Serial4

#define FFT_L_COLOR         383
#define FFT_R_COLOR         63553
#define FFT_COMBINED_COLOR  36882
#define FFT_START_X_COORD   13
#define FFT_BAND_WIDTH_PX   23

void refreshDisplay();

void trigger0();    // Reset button press event
void trigger1();    // Input select se button press event
void trigger2();    // Input select diff button press event
void trigger3();    // Exit fan page
void trigger4();    // Exit FFT page
void trigger5();    // Fan slider move event

void trigger6();    // init input page
void trigger7();    // init ouput page
void trigger8();    // init fan page
void trigger9();    // init fft page

void trigger10();   // Output select posts button press event
void trigger11();   // Output select speakon button press event

void trigger12();   // enable constant refresh
void trigger13();   // disable constant refresh

void trigger14();   // FFT L checkbox
void trigger15();   // FFT R checkbox

void setIndicator(String indicatorID, bool indicatorVar);
void setIndicator(String indicatorIdTrue, String indicatorIdFalse, bool indicatorVar);

uint8_t map_rms_to_display(float input, uint32_t fromMin, uint32_t fromMax, uint32_t toMin, uint32_t toMax);
void drawFFT(uint8_t fftGraph[], uint16_t color);
void drawFFT(uint8_t fftGraphL[], uint16_t colorL, uint8_t fftGraphR[], uint16_t colorR, uint16_t colorCombined);
String fillRectCmd(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif
