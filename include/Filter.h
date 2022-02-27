#pragma once
#include <arm_math.h>
/*  weighting filter coeffs (WF)
 *  http://gaidi.ca/weblog/implementing-a-biquad-cascade-iir-filter-on-a-cortex-m4#
 *  This is a Direct Form II Transposed
 * 
 *  See the included matlab file for how these values were calculated
 */

// A-Weightting
#if WEIGHTING_TYPE == 'A'
# define WF_IIR_ORDER       6
# define WF_IIR_NUM_STAGES  3

# if SAMPLE_RATE == 48000
    static float32_t WF_biquad_coeffs[5 * WF_IIR_NUM_STAGES] = {
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
    static float32_t WF_biquad_scale[WF_IIR_NUM_STAGES + 1] = {
        0.2341830434f,
        1.0000000000f,
        1.0000000000f,
        1.0000000000f
    };
# elif SAMPLE_RATE == 44100
    static float32_t WF_biquad_coeffs[5 * WF_IIR_NUM_STAGES] = {
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
    static float32_t WF_biquad_scale[WF_IIR_NUM_STAGES + 1] = {
        0.2556115104f,
        1.0000000000f,
        1.0000000000f,
        1.0000000000f
    };
# else
    #error A-weighting coefficients not found for given sample rate!
# endif

// C-Weighting
#elif WEIGHTING_TYPE == 'C'
# define WF_IIR_ORDER       6
# define WF_IIR_NUM_STAGES  2

# if SAMPLE_RATE == 48000
    static float32_t WF_biquad_coeffs[5 * WF_IIR_NUM_STAGES] = {
        1.0000000000f,
        2.0000000000f,
        1.0000000000f,
        0.2245584581f,
        -0.0126066253f,
        1.0000000000f,
        -2.0000000000f,
        1.0000000000f,
        1.9946144560f,
        -0.9946217070f
    };
    // I can use this if I want, but IDRC about gain > 1 in this case
    // 0th element is gain before the first biquad
    static float32_t WF_biquad_scale[WF_IIR_NUM_STAGES + 1] = {
        0.1978907070f,
        1.0000000000f,
        1.0000000000f
    };
# elif SAMPLE_RATE == 44100
    static float32_t WF_biquad_coeffs[5 * WF_IIR_NUM_STAGES] = {
        1.0000000000f,
        2.0000000000f,
        1.0000000000f,
        0.1405360824f,
        -0.0049375976f,
        1.0000000000f,
        -2.0000000000f,
        1.0000000000f,
        1.9941388813f,
        -0.9941474694f
    };
    // I can use this if I want, but IDRC about gain > 1 in this case
    // 0th element is gain before the first biquad
    static float32_t WF_biquad_scale[WF_IIR_NUM_STAGES + 1] = {
        0.2170124955f,
        1.0000000000f,
        1.0000000000f
    };
# else
    #error C-weighting coefficients not found for given sample rate!
# endif
#endif
static float32_t WF_biquad_state0[WF_IIR_ORDER];
static float32_t WF_biquad_state1[WF_IIR_ORDER];