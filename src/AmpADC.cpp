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
}

void AmpADC::adc_task() {
    if ( !(abdma1.interrupted() && abdma2.interrupted()) ) {
        return;
    }

    volatile uint16_t *pbuffer0 = abdma1.bufferLastISRFilled();
    volatile uint16_t *end_pbuffer0 = pbuffer0 + abdma1.bufferCountLastISRFilled();
    volatile uint16_t *pbuffer1 = abdma2.bufferLastISRFilled();
    volatile uint16_t *end_pbuffer1 = pbuffer1 + abdma1.bufferCountLastISRFilled();

    if ((uint32_t)pbuffer0 >= 0x20200000u)  arm_dcache_delete((void*)pbuffer0, sizeof(dma_adc_buff1_1));
    if ((uint32_t)pbuffer1 >= 0x20200000u)  arm_dcache_delete((void*)pbuffer1, sizeof(dma_adc_buff1_1));

    float32_t workBuffer0[BUFF_SIZE], workBuffer1[BUFF_SIZE];
    
    // Copy DMA buffers to a work buffer
    copy_from_dma_buff_to_dsp_buff(pbuffer0, end_pbuffer0, workBuffer0, CALIBRATION_OFFSET_0);
    copy_from_dma_buff_to_dsp_buff(pbuffer1, end_pbuffer1, workBuffer1, CALIBRATION_OFFSET_1);

    // -------- Do not use pbuffers after this point. Use work buffers --------

    // Get RMS values in dB
    arm_rms_f32(workBuffer0, BUFF_SIZE, &rmsL);
    arm_rms_f32(workBuffer1, BUFF_SIZE, &rmsR);
    rmsL = 20 * log10f(rmsL);
    rmsR = 20 * log10f(rmsR);

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