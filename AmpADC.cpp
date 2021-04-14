#include "AmpADC.h"

AmpADC *AmpADC::instance;

AmpADC::AmpADC() {
    instance = this;
    adc = new ADC();
    averages = 4;
    freqKHz = 96;
    buffIsReady = false;
    buffLocation = 0;

    ////// ADC0 /////
    adc->adc0->setAveraging(averages); // set number of averages
    adc->adc0->setResolution(RESOLUTION); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::CONV_SPD); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::SAMPLE_SPD); // change the sampling speed

    ////// ADC1 /////
    adc->adc1->setAveraging(averages); // set number of averages
    adc->adc1->setResolution(RESOLUTION); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::CONV_SPD); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::SAMPLE_SPD); // change the sampling speed

    Timer.begin(triggerISR, 1000.0 / freqKHz);
}

void AmpADC::triggerISR() {
    instance->adcISR();
}

// Its probably not going to like this being in a class
void AmpADC::adcISR() {
  adc0Buff[buffLocation] = adc->adc0->analogRead(ADC_L_PIN);
  adc1Buff[buffLocation] = adc->adc1->analogRead(ADC_R_PIN);

  if(buffLocation == 0) {
    buffIsReady = false;
  }

  if(buffLocation < BUFF_SIZE - 1) {
    buffLocation++;
  }
  else {
    buffLocation = 0;
    buffIsReady = true;
  }
}
