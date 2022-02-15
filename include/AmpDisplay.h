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
#define MAX_REFRESH_RATE 60

#define RMS_MIN_VAL_DB  26
#define RMS_MAX_VAL_DB  66

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
void setIndicator(String indicatorID, bool indicatorVar);
void setIndicator(String indicatorIdTrue, String indicatorIdFalse, bool indicatorVar);

uint32_t map_rms_to_display(float input, uint32_t fromMin, uint32_t fromMax, uint32_t toMin, uint32_t toMax);

#endif
