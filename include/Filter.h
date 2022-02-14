#pragma once
#include "AmpADC.h"
/*
 *  A file for filter coefficients
 */


// A-weighting filter coeffs (AWF)
// http://gaidi.ca/weblog/implementing-a-biquad-cascade-iir-filter-on-a-cortex-m4#
#define AWF_IIR_ORDER       6
#define AWF_IIR_NUM_STAGES  (AWF_IIR_ORDER/2)

static float32_t m_biquad_state[AWF_IIR_ORDER];
static float32_t m_biquad_coeffs[5 * AWF_IIR_NUM_STAGES] = {
    1.0000000000f,
    0.0000000000f,
    -1.0000000000f,
    -0.4286284394f,
    0.2710337076f,
    1.0000000000f,
    0.0000000000f,
    -1.0000000000f,
    0.0910482487f,
    0.6749836143f,
    1.0000000000f,
    -2.0000000000f,
    1.0000000000f,
    1.9467893387f,
    -0.9474971824f
};
// I can use this if I want, but IDRC about gain > 1 in this case, so I probably won't
static float32_t m_biquad_scale[AWF_IIR_NUM_STAGES + 1] = {
    0.6094088340f,
    1.0000000000f,
    1.0000000000f,
    1.0000000000f
};