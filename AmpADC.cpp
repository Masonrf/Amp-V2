#include "AmpADC.h"

AmpADC *AmpADC::instance;

AmpADC::AmpADC() {
    instance = this;
    adc = new ADC();
    buffIsReady = false;
    buffLocation = 0;

    ////// ADC0 /////
    adc->adc0->setAveraging(AVERAGES); // set number of averages
    adc->adc0->setResolution(RESOLUTION); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::CONV_SPD); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::SAMPLE_SPD); // change the sampling speed
    adc->adc0->recalibrate();

    ////// ADC1 /////
    adc->adc1->setAveraging(AVERAGES); // set number of averages
    adc->adc1->setResolution(RESOLUTION); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::CONV_SPD); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::SAMPLE_SPD); // change the sampling speed
    adc->adc1->recalibrate();

    Serial.begin(9600);
    Timer.begin(triggerISR, 1000000.0 / SAMPLE_FREQ);
}

void AmpADC::triggerISR() {
    instance->adcISR();
}

void AmpADC::adcISR() {
    if(buffIsReady == true) {
        return;
    }
    // Weird things happen when you try to adcXBuff[buffLocation] = float(...anaglogRead... - max/2 + cal);
    // So did it this way with tmp vars and that seems to have fixed it.
    int16_t sampleL = adc->adc0->analogRead(ADC_L_PIN) - adc->adc0->getMaxValue() / 2 + CALIBRATION_OFFSET_L;
    int16_t sampleR = adc->adc1->analogRead(ADC_R_PIN) - adc->adc1->getMaxValue() / 2 + CALIBRATION_OFFSET_R;

    adc0Buff[buffLocation] = float(sampleL);
    adc1Buff[buffLocation] = float(sampleR);

    vImagL[buffLocation] = 0;
    vImagR[buffLocation] = 0;

    if(buffLocation < BUFF_SIZE - 1) {
        buffLocation++;
    }
    else {
        buffLocation = 0;
        buffIsReady = true;
    }
}

void AmpADC::printBuffs() {
    if(!buffIsReady) {
        return;
    }
    for (int i = 0; i < BUFF_SIZE; i++) {
        Serial.print(adc0Buff[i], DEC);
        Serial.print("\t");
        Serial.print(adc1Buff[i], DEC);
        Serial.println();
    }
    buffIsReady = false;
}

void AmpADC::getRMS() {
    if(!buffIsReady) {
        return;
    }
    // If you start to get rollover with larger buffer sizes, change these to 64 bit unsigned ints.
    double sumL = 0, sumR = 0;
    for(uint16_t i = 0; i < BUFF_SIZE; i++) {
        sumL += adc0Buff[i] * adc0Buff[i];
        sumR += adc1Buff[i] * adc1Buff[i];
    }

    rmsL = 20 * log10( sqrt(sumL / BUFF_SIZE) );
    rmsR = 20 * log10( sqrt(sumR / BUFF_SIZE) );

    buffIsReady = false;
}

void AmpADC::printRMS() {
    getRMS();

    noInterrupts();
    Serial.print(rmsL, DEC);
    Serial.print("\t");
    Serial.print(rmsR, DEC);
    Serial.println();
    interrupts();
}

void AmpADC::runFFT() {

    if(!buffIsReady) {
        return;
    }

    fftL.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    fftL.compute(FFTDirection::Forward);
    fftL.complexToMagnitude();

    fftR.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    fftR.compute(FFTDirection::Forward);
    fftR.complexToMagnitude();

    buffIsReady = false;
}

// Serial monitor is a bit whacky. Looks like all the data is there though...
void AmpADC::printFFT() {
    runFFT();

    // noisr/isr and get rid of buffIsReady here???
    noInterrupts();
    for (int i = 0; i < BUFF_SIZE / 2; i++) {
        Serial.print(adc0Buff[i], DEC);
        Serial.print("\t");
        Serial.print(adc1Buff[i], DEC);
        Serial.println();
    }
    interrupts();

}
