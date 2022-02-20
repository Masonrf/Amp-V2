#include "AmpControl.h"
#include "AmpDisplay.h"

elapsedMillis ledTimer;

extern EasyNex nexDisplay;
extern AmpADC amp_adc;

void setup() {
    pinMode(13, OUTPUT);
    nexDisplay.begin(921600);

    //Serial.begin(9600);

    delay(500);
}

void loop() {

    refreshDisplay();
    amp_adc.adc_task(nexDisplay.currentPageId);

    if(ledTimer >= 1000) { // heartbeat led
        digitalWriteFast(13, !digitalReadFast(13));
        ledTimer = 0;
    }
}
