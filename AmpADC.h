#ifndef AMPADC_H
#define AMPADC_H

#include <ADC.h>

// ** arduinoFFT dev build: version >=2.0; Manual install currently
#define FFT_SPEED_OVER_PRECISION
//#define FFT_SQRT_APPROXIMATION  // DONT USE -- pretty bad spike of freq that isnt present/bad math
#include "arduinoFFT.h"

#define RESOLUTION  12
#define CONV_SPD    HIGH_SPEED
#define SAMPLE_SPD  HIGH_SPEED

// These are just estimates based off the serial plotter
#define CALIBRATION_OFFSET_L 15
#define CALIBRATION_OFFSET_R 4

 // WARNING: larger buffer sizes cause problems with RMS. See function for details
#define BUFF_SIZE     1024  // Power of 2 up to 2^16 <- On a Teensy 4, you'll run out of memory much earlier
#define SAMPLE_FREQ   48000
#define AVERAGES      4     // oversampling. 0, 4, 8, 16, 32 times. Probably should stick to 0 or 4
#define NUM_BINS      64

#define ADC_L_PIN     14
#define ADC_R_PIN     15

/*
    This is mostly a placeholder for the actual Teensy audio library stuff.
    Currently the ADC input doesnt work for Teensy 4...

    Im in a bit of a time crunch currently so this whole thing is pretty rough
    Im not super proud of all of this but I intend to replace all of it with PJRC's
    Teensy Audio Library in the future anyways.

    I would have loved to have time to use DMA and some of the special DSP instructions
    that the IMXRT1062 has built in, but this semester has been rough...

 */
class AmpADC : protected ADC {
public:
    // These functions are for debugging through the serial monitor only.
    // You will need to Serial.begin() before using these.
    void printBuffs();
    void printFFT();
    void printRMS();

protected:

    float rmsL;     // in dB
    float rmsR;     // in dB

    AmpADC();

    void getRMS();
    void runFFT();
    void readFFTtoBins();

private:
    ADC *adc;
    IntervalTimer Timer;  // This timer uses hardware timers and interrupts

    static AmpADC *instance;
    static void triggerISR();
    void adcISR();

    // These need to be volatile. Change in interrupt context
    volatile uint16_t buffLocation;
    volatile bool buffIsReady;

    // I'd use dynamic allocation for these, but I wanted them in RAM 1
    // Also these are NOT volatile. The pointers don't change in interrupts
    float adc0Buff[BUFF_SIZE];
    float adc1Buff[BUFF_SIZE];

    float vImagL[BUFF_SIZE];
    float vImagR[BUFF_SIZE];

    // used for precalculated windowing (fft.windowing wont relcalculate every time its called)
    float weighingFactors[BUFF_SIZE];
    
    ArduinoFFT<float> fftL = ArduinoFFT<float>(adc0Buff, vImagL, BUFF_SIZE, SAMPLE_FREQ, weighingFactors);
    ArduinoFFT<float> fftR = ArduinoFFT<float>(adc1Buff, vImagR, BUFF_SIZE, SAMPLE_FREQ, weighingFactors);

    // TODO: write from binA -> binB into an output fft array on log scale
    float fftBinsL[NUM_BINS];
    float fftBinsR[NUM_BINS];

};


#endif
