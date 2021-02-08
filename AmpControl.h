/*
 * All functions related to I/O handling for the Teensy
 */

#ifndef AmpControl_h
#define AmpControl_h

#include "Arduino.h"

// PINs
#define CLIP_PIN      0
#define FAULT_PIN     1
#define RESET_PIN     2
#define PWM_PIN       3
#define TACH_PIN      4
#define IN_RELAY_PIN  5
#define OUT_RELAY_PIN 6
#define ADC_L_PIN     14
#define ADC_R_PIN     15
#define LCD_INT_PIN   20


class AmpControl {
public:
    AmpControl();

    volatile boolean fault;  // 0 == no fault,    1 == fault
    volatile boolean clip;   // 0 == no clip,     1 == clipping
    boolean input;           // 0 == 3.5mm (SE),  1 == XLR (Diff)
    boolean output;          // 0 == SpeakOns,    1 == Posts

    boolean updateCtrl;     // Tell the display to update

    void toggleRelay(boolean *toggle);
//    void resetAmp(uint8_t resetLengthSeconds);

private:
//    static AmpControl *amp;

//    static void faultISR();
//    void faultISRWork();

//    static void clipISR();
//    void clipISRWork();

    void toggleBool(boolean *toggle);
//    static void endReset();
};

#endif
