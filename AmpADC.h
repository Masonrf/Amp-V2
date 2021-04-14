#ifndef AMPADC_H
#define AMPADC_H

#include <ADC.h>

#define BUFF_SIZE   1024
#define RESOLUTION  12
#define CONV_SPD    HIGH_SPEED
#define SAMPLE_SPD  HIGH_SPEED

#define ADC_L_PIN     14
#define ADC_R_PIN     15

/*
    This is mostly a placeholder for the actual Teensy audio library stuff.
    Currently the ADC input doesnt work for Teensy 4...

    Im in a bit of a time crunch currently so this whole thing is pretty rough
    Im not super proud of all of this but I intend to replace all of it with PJRC's
    Teensy Audio Library in the future anyways.
 */
class AmpADC : protected ADC {
public:
    volatile bool buffIsReady;

    AmpADC();

protected:
    volatile uint16_t adc0Buff[BUFF_SIZE];
    volatile uint16_t adc1Buff[BUFF_SIZE];

private:
    float freqKHz;
    uint8_t averages; // oversampling. 0x or 4x are really the only usable options

    ADC *adc;
    IntervalTimer Timer;  // This timer uses hardware timers and interrupts

    volatile uint16_t buffLocation;

    static AmpADC *instance;
    static void triggerISR();
    void adcISR();
};


#endif
