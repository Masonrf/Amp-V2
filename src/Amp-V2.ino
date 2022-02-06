#include "AmpControl.h"
#include "AmpDisplay.h"

elapsedMillis ledTimer;

extern EasyNex nexDisplay;

void setup() {
    pinMode(13, OUTPUT);
    nexDisplay.begin(512000);

    delay(500);
}

void loop() {

    refreshDisplay();
    if(ledTimer >= 1000) { // heartbeat led
        digitalWriteFast(13, !digitalReadFast(13));
        ledTimer = 0;
    }
}
