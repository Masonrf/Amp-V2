#include "AmpADC.h"

// DMA Buffers and structures
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff1_1[BUFF_SIZE];
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff1_2[BUFF_SIZE];
AnalogBufferDMA abdma1(dma_adc_buff1_1, BUFF_SIZE, dma_adc_buff1_2, BUFF_SIZE);

DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff2_1[BUFF_SIZE];
DMAMEM static volatile uint16_t __attribute__((aligned(32))) dma_adc_buff2_2[BUFF_SIZE];
AnalogBufferDMA abdma2(dma_adc_buff2_1, BUFF_SIZE, dma_adc_buff2_2, BUFF_SIZE);

AmpADC::AmpADC() {
    adc = new ADC();
    

    ////// ADC0 /////
    adc->adc0->setAveraging(ADC_AVGS); // set number of averages
    adc->adc0->setResolution(ADC_RES); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CNV_SPD); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SA_SPD); // change the sampling speed
    adc->adc0->recalibrate();

    ////// ADC1 /////
    adc->adc1->setAveraging(ADC_AVGS); // set number of averages
    adc->adc1->setResolution(ADC_RES); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CNV_SPD); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SA_SPD); // change the sampling speed
    adc->adc1->recalibrate();

    abdma1.init(adc, ADC_0/*, DMAMUX_SOURCE_ADC_ETC*/);
    abdma2.init(adc, ADC_1/*, DMAMUX_SOURCE_ADC_ETC*/);

    // Start the dma operation..
    adc->adc0->startSingleRead(ADC_L_PIN); // call this to setup everything before the Timer starts, differential is also possible
    adc->adc1->startSingleRead(ADC_R_PIN);

    adc->adc0->startTimer(SAMPLE_RATE); // samples/sec
    adc->adc1->startTimer(SAMPLE_RATE); // samples/sec

    setWindowFunction(HAMMING);
    // Initialize fft
    arm_rfft_fast_init_f32(&f32_instance0, BUFF_SIZE);
    arm_rfft_fast_init_f32(&f32_instance1, BUFF_SIZE);

    arm_biquad_cascade_df2T_init_f32(&AWF_filtInst0, AWF_IIR_NUM_STAGES, AWF_biquad_coeffs, AWF_biquad_state0);
    arm_biquad_cascade_df2T_init_f32(&AWF_filtInst1, AWF_IIR_NUM_STAGES, AWF_biquad_coeffs, AWF_biquad_state1);

}

void AmpADC::adc_task(int currentPage) {
    if ( !(abdma1.interrupted() && abdma2.interrupted()) ) {
        return;
    }

    volatile uint16_t *pbuffer0 = abdma2.bufferLastISRFilled();
    volatile uint16_t *end_pbuffer0 = pbuffer0 + abdma2.bufferCountLastISRFilled();
    volatile uint16_t *pbuffer1 = abdma1.bufferLastISRFilled();
    volatile uint16_t *end_pbuffer1 = pbuffer1 + abdma2.bufferCountLastISRFilled();

    if ((uint32_t)pbuffer0 >= 0x20200000u)  arm_dcache_delete((void*)pbuffer0, sizeof(dma_adc_buff1_1));
    if ((uint32_t)pbuffer1 >= 0x20200000u)  arm_dcache_delete((void*)pbuffer1, sizeof(dma_adc_buff1_1));


    //printBuffers(true, BUFF_SIZE, workBuffer0, workBuffer1);

    switch(currentPage) {
        case MAIN_PAGE:
            // Copy DMA buffers to a work buffer
            copy_from_dma_buff_to_dsp_buff(pbuffer0, end_pbuffer0, workBuffer0, CALIBRATION_OFFSET_0);
            copy_from_dma_buff_to_dsp_buff(pbuffer1, end_pbuffer1, workBuffer1, CALIBRATION_OFFSET_1);

            // A-weighting
            for(int i = 0; i < BUFF_SIZE; i++) {
                workBuffer0[i] *= AWF_biquad_scale[0];  // scale the input value
                workBuffer1[i] *= AWF_biquad_scale[0];
            }
            arm_biquad_cascade_df2T_f32(&AWF_filtInst0, workBuffer0, awfBuff0, BUFF_SIZE);
            arm_biquad_cascade_df2T_f32(&AWF_filtInst1, workBuffer1, awfBuff1, BUFF_SIZE);

            // Get RMS values in dB
            arm_rms_f32(awfBuff0, BUFF_SIZE, &rmsL);
            arm_rms_f32(awfBuff1, BUFF_SIZE, &rmsR);
            rmsL = 20 * log10f(rmsL);
            rmsR = 20 * log10f(rmsR);
            break;
        
        case FFT_PAGE:
            // Copy DMA buffers to a work buffer
            copy_from_dma_buff_to_dsp_buff(pbuffer0, end_pbuffer0, workBuffer0, CALIBRATION_OFFSET_0);
            copy_from_dma_buff_to_dsp_buff(pbuffer1, end_pbuffer1, workBuffer1, CALIBRATION_OFFSET_1);

            // Windows remove some of the errors/artifacts with doing FFTs of arrays of finite length
            applyWindowToBuffer(workBuffer0);
            applyWindowToBuffer(workBuffer1);

            // Set up for FFT
            // 32 bit float maths should be about the same speed as integer maths on Teensy 4. if double precision is required, its half speed of floats
            // We can use real fft here instead of complex fft and cut computation time in half.
            // The input data is all real. The output data is in complex form arr=[real0, imag0, real1, imag1,...]
            // Here is a good example http://gaidi.ca/weblog/configuring-cmsis-dsp-package-and-performing-a-real-fft

            // ooh math, shiny!!!!
            arm_rfft_fast_f32(&f32_instance0, workBuffer0, fftOutput0, 0);
            arm_rfft_fast_f32(&f32_instance1, workBuffer1, fftOutput1, 0);

            // compute the magnitude and put it in the mag buffers
            arm_cmplx_mag_f32(fftOutput0, mag0, BUFF_SIZE/2);
            arm_cmplx_mag_f32(fftOutput1, mag1, BUFF_SIZE/2);

            packMagIntoFFTGraph(mag0, fftGraph0);
            packMagIntoFFTGraph(mag1, fftGraph1);

            break;

        default:
            break;
    }
    
}

void AmpADC::copy_from_dma_buff_to_dsp_buff(volatile uint16_t *dmaBuff, volatile uint16_t *end_dmaBuff, float32_t *dspBuff, float32_t offset) {
    volatile uint16_t *dmaBuff_ptr = dmaBuff;
    float32_t *dspBuff_ptr = dspBuff;

    offset += 2048.0;     // from unsigned 12 bit to signed values

    while(dmaBuff_ptr < end_dmaBuff) {
        *dspBuff_ptr = *dmaBuff_ptr - offset;   
        dspBuff_ptr++;
        dmaBuff_ptr++;
        
    }
}

void AmpADC::setWindowFunction(uint8_t windowType) {
    const float32_t n = PI/(BUFF_SIZE - 1);
    switch(windowType) {
        case HAMMING:
            for(uint16_t i = 0; i < BUFF_SIZE; i++) { window[i] = 0.54 - 0.46*cos(2*i*n); }
            break;

        case HANNING:
            for(uint16_t i = 0; i < BUFF_SIZE; i++) { window[i] = 0.5 - 0.5*cos(2*i*n); }
            break;

        case BLACKMANHARRIS:
            for(uint16_t i = 0; i < BUFF_SIZE; i++) { window[i] = 0.35875 - 0.48829*cos(2*i*n) + 0.14128*cos(4*i*n) - 0.01168*cos(6*i*n); }
            break;

        case FLATTOP:
            for(uint16_t i = 0; i < BUFF_SIZE; i++) { window[i] = 0.21557895 - 0.41663158*cos(2*i*n) + 0.277263158*cos(4*i*n) - 0.083578947*cos(6*i*n) + 0.006947368*cos(8*i*n); }
            break;
        
        default: // Rectangular
            for(uint16_t i = 0; i < BUFF_SIZE; i++) { window[i] = 1; }
            break;
    }
}

void AmpADC::applyWindowToBuffer(float32_t *buffer) {
    float32_t *windowPtr = window;
    float32_t *bufferPtr = buffer;

    for(uint16_t i = 0; i < BUFF_SIZE; i++) {
        *bufferPtr *= *windowPtr;
        bufferPtr++;
        windowPtr++;
    }
}

/*
    FFT Bins:      512
    Display Bands: 32
    Sample Freq:   48000.0

    E = 1.1445

              Min         Max
    Band:  Bin (Freq)  Bin (Freq)
    0:    0 (   0.0)    0 (  46.9)
    1:    1 (  46.9)    1 (  93.8)
    2:    2 (  93.8)    2 ( 140.6)
    3:    3 ( 140.6)    3 ( 187.5)
    4:    4 ( 187.5)    5 ( 281.2)
    5:    6 ( 281.2)    7 ( 375.0)
    6:    8 ( 375.0)    9 ( 468.8)
    7:   10 ( 468.8)   12 ( 609.4)
    8:   13 ( 609.4)   15 ( 750.0)
    9:   16 ( 750.0)   18 ( 890.6)
    10:   19 ( 890.6)   22 (1078.1)
    11:   23 (1078.1)   26 (1265.6)
    12:   27 (1265.6)   31 (1500.0)
    13:   32 (1500.0)   37 (1781.2)
    14:   38 (1781.2)   44 (2109.4)
    15:   45 (2109.4)   52 (2484.4)
    16:   53 (2484.4)   61 (2906.2)
    17:   62 (2906.2)   71 (3375.0)
    18:   72 (3375.0)   82 (3890.6)
    19:   83 (3890.6)   95 (4500.0)
    20:   96 (4500.0)  110 (5203.1)
    21:  111 (5203.1)  127 (6000.0)
    22:  128 (6000.0)  146 (6890.6)
    23:  147 (6890.6)  168 (7921.9)
    24:  169 (7921.9)  194 (9140.6)
    25:  195 (9140.6)  223 (10500.0)
    26:  224 (10500.0)  256 (12046.9)
    27:  257 (12046.9)  294 (13828.1)
    28:  295 (13828.1)  338 (15890.6)
    29:  339 (15890.6)  388 (18234.4)
    30:  389 (18234.4)  445 (20906.2)
    31:  446 (20906.2)  511 (24000.0)
*/
void AmpADC::packMagIntoFFTGraph(float32_t mag[], uint8_t fftGraph[]) {
    static uint16_t lowBins[NUM_BANDS]  = {0, 1, 2, 3, 4, 6, 8, 10, 13, 16, 19, 23, 27, 32, 38, 45, 53, 62, 72, 83,  96, 111, 128, 147, 169, 195, 224, 257, 295, 339, 389, 446};
    static uint16_t highBins[NUM_BANDS] = {0, 1, 2, 3, 5, 7, 9, 12, 15, 18, 22, 26, 31, 37, 44, 52, 61, 71, 82, 95, 110, 127, 146, 168, 194, 223, 256, 294, 338, 388, 445, 511};
    
    // average into bins and convert to dB
    for(int i = 0; i < NUM_BANDS - 1; i++) {
        float32_t value = 20 * log10f( avgMagBins(lowBins[i], highBins[i], mag) );
        fftGraph[i] = map_rms_to_display(value, 50, 150, 0, 184); // map rms to waveform height
    }
}

float32_t AmpADC::avgMagBins(uint16_t lowBin, uint16_t highBin, float32_t mag[]) {
    /* who needs input sanitization anyways? Live life on the edge...
    if(lowBin > highBin || highBin > BUFF_SIZE/2) { // inputs need to make sense
        return 0.0;
    }
    */
    if(lowBin == highBin) { // no averaging needed
        return mag[lowBin];
    }
    // take an average of the bins
    uint16_t diff = highBin - lowBin;
    float32_t sum = 0;
    for(uint16_t i = 0; i < diff; i++) {
        sum += mag[lowBin + i];
    }
    return sum / diff;
    
}

void AmpADC::printBuffers(bool print, uint16_t size, float32_t *buff0, float32_t *buff1 = nullptr) {
    if(print == true) {
        if(buff1 == nullptr) {
            for(int i = 0; i < size; i++) {
                Serial.print(buff0[i]);
                Serial.println();
            }
        }
        else {
            for(int i = 0; i < size; i++) {
                Serial.print(buff0[i]);
                Serial.print("\t");
                Serial.print(buff1[i]);
                Serial.println();
            }
        }
        
    }
}