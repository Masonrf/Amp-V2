#pragma once
#include <arm_math.h>
/*
 *  A file for filter coefficients
 */


/*  A-weighting filter coeffs (AWF)
 *  http://gaidi.ca/weblog/implementing-a-biquad-cascade-iir-filter-on-a-cortex-m4#
 *  This is a Direct Form II Transposed at Fs = 48kHz
 */

#define AWF_IIR_ORDER       6
#define AWF_IIR_NUM_STAGES  (AWF_IIR_ORDER/2)

static float32_t AWF_biquad_state0[AWF_IIR_ORDER];
static float32_t AWF_biquad_state1[AWF_IIR_ORDER];

#if SAMPLE_RATE == 48000
    static float32_t AWF_biquad_coeffs[5 * AWF_IIR_NUM_STAGES] = {
        1.0000000000f,
        2.0000000000f,
        1.0000000000f,
        0.2245584581f,
        -0.0126066253f,
        1.0000000000f,
        -2.0000000000f,
        1.0000000000f,
        1.8938704947f,
        -0.8951597691f,
        1.0000000000f,
        -2.0000000000f,
        1.0000000000f,
        1.9946144560f,
        -0.9946217070f
    };
    // I can use this if I want, but IDRC about gain > 1 in this case
    // 0th element is gain before the first biquad
    static float32_t AWF_biquad_scale[AWF_IIR_NUM_STAGES + 1] = {
        0.2341830434f,
        1.0000000000f,
        1.0000000000f,
        1.0000000000f
    };
#elif SAMPLE_RATE == 44100
    static float32_t AWF_biquad_coeffs[5 * AWF_IIR_NUM_STAGES] = {
        1.0000000000f,
        2.0000000000f,
        1.0000000000f,
        0.1405360824f,
        -0.0049375976f,
        1.0000000000f,
        -2.0000000000f,
        1.0000000000f,
        1.8849012174f,
        -0.8864214718f,
        1.0000000000f,
        -2.0000000000f,
        1.0000000000f,
        1.9941388813f,
        -0.9941474694f
    };
    // I can use this if I want, but IDRC about gain > 1 in this case
    // 0th element is gain before the first biquad
    static float32_t AWF_biquad_scale[AWF_IIR_NUM_STAGES + 1] = {
        0.2556115104f,
        1.0000000000f,
        1.0000000000f,
        1.0000000000f
    };
#else
    #error A-weighting coefficients not found for given sample rate!
#endif