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
#define MAX_REFRESH_RATE 30

void refreshDisplay();


void trigger0();    // Reset button press event
void trigger1();    // Input select button press event
void trigger2();    // Output select button press event
void trigger3();    // Exit fan page
void trigger4();    // Exit FFT page
void trigger5();    // Fan slider move event

void trigger6();    // init input page
void trigger7();    // init ouput page
void trigger8();    // init fan page
void trigger9();    // init fft page

void setIndicator(String indicatorID, bool indicatorVar);
void setIndicator(String indicatorIdTrue, String indicatorIdFalse, bool indicatorVar);

#endif
