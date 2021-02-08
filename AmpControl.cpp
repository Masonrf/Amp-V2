#include "AmpControl.h"
#include <TimeLib.h>
#include <TimeAlarms.h>

//AmpControl *AmpControl::amp = 0;

AmpControl::AmpControl() {
//    amp = this;
//    attachInterrupt(digitalPinToInterrupt(FAULT_PIN), AmpControl::faultISR, LOW);
//    attachInterrupt(digitalPinToInterrupt(CLIP_PIN), AmpControl::clipISR, LOW);

    pinMode(IN_RELAY_PIN, OUTPUT);
    input = 0;
    digitalWrite(IN_RELAY_PIN, LOW);

    pinMode(OUT_RELAY_PIN, OUTPUT);
    output = 0;
    digitalWrite(OUT_RELAY_PIN, LOW);

    updateCtrl = 1;
}

void AmpControl::toggleRelay(boolean *toggle) {
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
}

/*
void AmpControl::resetAmp(uint8_t resetLengthSeconds) {
    digitalWrite(RESET_PIN, LOW);
    Alarm.timerOnce(resetLengthSeconds, endReset);
    Alarm.delay(100); // 100ms delay to allow the amp to go into a reset state
}
*/
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
/*
void AmpControl::endReset() {
    digitalWrite(RESET_PIN, HIGH);
    Alarm.delay(100); // 100ms delay to allow the amp to go into a normal state
}*/
