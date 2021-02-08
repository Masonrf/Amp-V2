#include "AmpControl.h"

//AmpControl *AmpControl::amp = 0;

AmpControl::AmpControl() {
//    amp = this;

//    pinMode(FAULT_PIN, INPUT_PULLUP);
//    pinMode(CLIP_PIN, INPUT_PULLUP);
//    attachInterrupt(digitalPinToInterrupt(FAULT_PIN), AmpControl::faultISR, LOW);
//    attachInterrupt(digitalPinToInterrupt(CLIP_PIN), AmpControl::clipISR, LOW);

    pinMode(IN_RELAY_PIN, OUTPUT);
    input = 0;
    digitalWrite(IN_RELAY_PIN, LOW);

    pinMode(OUT_RELAY_PIN, OUTPUT);
    output = 0;
    digitalWrite(OUT_RELAY_PIN, LOW);

    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, HIGH);

    updateCtrl = 1;
}

void AmpControl::toggleRelay(boolean *toggle) {
    // A disabling the amp is necessary to prevent popping when switching the relays
    startReset();

    if(toggle == &input) { // If passed the input boolean, ...
        if(*toggle) {
            digitalWrite(IN_RELAY_PIN, LOW); // Switch to 3.5mm
        }
        else {
            digitalWrite(IN_RELAY_PIN, HIGH); // Switch to XLR
        }
    }
    else if(toggle == &output) {  // If passed the output boolean, ...
        if(*toggle) {
            digitalWrite(OUT_RELAY_PIN, LOW); // Switch to SpeakOns
        }
        else {
            digitalWrite(OUT_RELAY_PIN, HIGH); // Switch to Binding Posts
        }
    }

    toggleBool(toggle);  // Switch the boolean

    endReset();
}

void AmpControl::startReset() {
    if(reset) {
        return;
    }
    reset = 1;
    digitalWrite(RESET_PIN, LOW);
    delay(RESET_DELAY);
}

void AmpControl::endReset() {
    if(!reset) {
        return;
    }
    digitalWrite(RESET_PIN, HIGH);
    delay(RESET_DELAY / 2);
    reset = 0;
}

//////////////////////////////////////////////
// Private
//////////////////////////////////////////////
/*
void AmpControl::faultISR() {
    if(amp != 0) {
        amp->faultISRWork();
    }
}

void AmpControl::faultISRWork() {
    toggleBool(&fault);
    updateCtrl = 1;
}

void AmpControl::clipISR() {
    if(amp != 0) {
        amp->clipISRWork();
    }
}

void AmpControl::clipISRWork() {
    toggleBool(&clip);
    updateCtrl = 1;
}
*/

void AmpControl::toggleBool(boolean *toggle) {
    *toggle = !(*toggle);
}
