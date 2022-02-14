#ifndef AMPADC_H
#define AMPADC_H

#include <Arduino.h>
#include <ADC.h>
#include <ADC_Module.h>
#include <AnalogBufferDMA.h>
#include <DMAChannel.h>
#include <arm_math.h>
#include <arm_const_structs.h>
#include "Filter.h"

// Resolution in bits. T4.1 may not work with all values
// For single-ended measurements: 8, 10, 12 or 16 bits. For differential measurements: 9, 11, 13 or 16 bits
#define ADC_RES         12

// Number of averages. Can be 0, 4, 8, 16 or 32
#define ADC_AVGS        0

// How fast the ADC will try to sample
// VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED.
#define ADC_SA_SPD      ADC_SAMPLING_SPEED::VERY_HIGH_SPEED

// Changes ADCK to alter conversion speed
// VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED, VERY_HIGH_SPEED, ADACK_2_4, ADACK_4_0, ADACK_5_2 or ADACK_6_2
#define ADC_CNV_SPD     ADC_CONVERSION_SPEED::VERY_HIGH_SPEED

// These are just estimates based off the serial plotter
#define CALIBRATION_OFFSET_0 -5
#define CALIBRATION_OFFSET_1 -5

 // WARNING: larger buffer sizes cause problems with RMS. See function for details
#define BUFF_SIZE     1024  // Power of 2 up to 2^16 <- On a Teensy 4, you'll run out of memory much earlier
#define SAMPLE_RATE   48000
#define NUM_BINS      64

#define ADC_L_PIN     14
#define ADC_R_PIN     15


class AmpADC : protected ADC {
public:
    float rmsL;     // in dB
    float rmsR;     // in dB

    float32_t mag0[BUFF_SIZE/2], mag1[BUFF_SIZE/2];

    AmpADC();
    void adc_task();

private:
    ADC *adc;

    void copy_from_dma_buff_to_dsp_buff(volatile uint16_t *dmaBuff, volatile uint16_t *end_dmaBuff, float32_t *dspBuff, float32_t offset);

    // Window types. Google these if you have questions or want to add more.
    float32_t window[BUFF_SIZE];
    enum windowTypes{HAMMING, HANNING, BLACKMANHARRIS, FLATTOP};
    void setWindowFunction(uint8_t windowType);
    void applyWindowToBuffer(float32_t *buffer);

    // FFT variables
    float32_t fftOutput0[BUFF_SIZE], fftOutput1[BUFF_SIZE];
    arm_rfft_fast_instance_f32 f32_instance0, f32_instance1;

    void printBuffers(bool print, uint16_t size, float32_t *buff0, float32_t *buff1 = nullptr);
};


#endif
