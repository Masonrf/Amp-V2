#include "AmpControl.h"

AmpControl::AmpControl() {
    pinMode(IN_RELAY_PIN, OUTPUT);
    input = 0;
    digitalWrite(IN_RELAY_PIN, LOW);

    pinMode(OUT_RELAY_PIN, OUTPUT);
    output = 0;
    digitalWrite(OUT_RELAY_PIN, LOW);
}

void AmpControl::toggleRelay(boolean *toggle) {
    if(toggle == &input) { // If passed the input boolean, ...
        if(*toggle == 0) {
            digitalWrite(IN_RELAY_PIN, HIGH); // Switch to XLR
            *toggle = 1;
        }
        else {
            digitalWrite(IN_RELAY_PIN, LOW); // Switch to 3.5mm
            *toggle = 0;
        }

    }
    else if(toggle == &output) {  // If passed the output boolean, ...
        if(*toggle == 0) {
            digitalWrite(OUT_RELAY_PIN, HIGH); // Switch to Binding Posts
            *toggle = 1;
        }
        else {
            digitalWrite(OUT_RELAY_PIN, LOW); // Switch to SpeakOns
            *toggle = 0;
        }
    }
}
