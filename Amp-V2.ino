#include "AmpControl.h"
#include "AmpDisplay.h"

AmpDisplay Touchscreen;
elapsedMillis ledTimer;

void setup() {
    pinMode(13, OUTPUT);

    delay(2000);
}

void loop() {
    // Not sure if it would be better to set this as an IntervalTimer or not.
    // Its not really critical if this updates on time.
    Touchscreen.refreshDisplay();
    if(ledTimer >= 1000) { // heartbeat led
        digitalWriteFast(13, !digitalReadFast(13));
        ledTimer = 0;
    }
}
