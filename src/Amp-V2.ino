#include "AmpControl.h"
#include "AmpDisplay.h"

elapsedMillis ledTimer;

extern EasyNex nexDisplay;

void setup() {
    pinMode(13, OUTPUT);
    nexDisplay.begin(115200);

    delay(2000);
}

void loop() {
    // Not sure if it would be better to set this as an IntervalTimer or not.
    // Its not really critical if this updates on time.
    refreshDisplay();
    if(ledTimer >= 1000) { // heartbeat led
        digitalWriteFast(13, !digitalReadFast(13));
        ledTimer = 0;
    }
}
