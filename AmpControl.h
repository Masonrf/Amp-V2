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

    boolean fault;
    boolean clip;
    boolean input;  // 0 == 3.5mm (SE), 1 == XLR (Diff)
    boolean output; // 0 == SpeakOns, 1 == Posts

    void toggleRelay(boolean *toggle);

private:

};

#endif
