#include "AmpControl.h"
#include "AmpDisplay.h"

AmpDisplay Touchscreen;
elapsedMillis ledTimer;

void faultISR(){
    Touchscreen.fault = !(Touchscreen.fault);
    Touchscreen.updateCtrl = 1;
}

void clipISR() {
    Touchscreen.clip = !(Touchscreen.clip);
    Touchscreen.updateCtrl = 1;
}

void setup() {
    pinMode(13, OUTPUT);
    pinMode(FAULT_PIN, INPUT_PULLUP);
    pinMode(CLIP_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FAULT_PIN), faultISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CLIP_PIN), clipISR, CHANGE);

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
