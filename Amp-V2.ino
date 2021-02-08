#include "AmpControl.h"
#include "AmpDisplay.h"

AmpDisplay Touchscreen;

void faultISR(){
    Touchscreen.fault = !(Touchscreen.fault);
    Touchscreen.updateCtrl = 1;
}

void clipISR() {
    Touchscreen.clip = !(Touchscreen.clip);
    Touchscreen.updateCtrl = 1;
}

void setup() {
    pinMode(FAULT_PIN, INPUT_PULLUP);
    pinMode(CLIP_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FAULT_PIN), faultISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CLIP_PIN), clipISR, CHANGE);

    delay(2000);
}

void loop() {
    Touchscreen.refreshDisplay();
}
