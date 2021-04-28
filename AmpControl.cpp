#include "AmpControl.h"

AmpControl *AmpControl::instance;

AmpControl::AmpControl() {
    instance = this;

    pinMode(IN_RELAY_PIN, OUTPUT);
    EEPROM.get(EEPROM_BASE_ADDR + EEPROM_INPUT_ADDR, tempByte);
    input = byteToBool(tempByte);
    if(input == 0) {
        digitalWrite(IN_RELAY_PIN, LOW);
    }
    else {
        digitalWrite(IN_RELAY_PIN, HIGH);
    }

    pinMode(OUT_RELAY_PIN, OUTPUT);
    //output = 0;
    EEPROM.get(EEPROM_BASE_ADDR + EEPROM_OUTPUT_ADDR, tempByte);
    output = byteToBool(tempByte);
    if(output == 0) {
        digitalWrite(OUT_RELAY_PIN, LOW);
    }
    else {
        digitalWrite(OUT_RELAY_PIN, HIGH);
    }

    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, HIGH);

    pinMode(FAN_PWM_PIN, OUTPUT);
    analogWriteFrequency(FAN_PWM_PIN, FAN_PWM_FREQ);
    //fanDutyCycle = 128;
    EEPROM.get(EEPROM_BASE_ADDR + EEPROM_FAN_ADDR, fanDutyCycle);
    analogWrite(FAN_PWM_PIN, fanDutyCycle);

    pinMode(FAULT_PIN, INPUT_PULLUP);
    pinMode(CLIP_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FAULT_PIN), faultTrigger, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CLIP_PIN), clipTrigger, CHANGE);

    updateCtrl = true;
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

    tempByte = boolToByte(*toggle);
    if(toggle == &input) {
        EEPROM.put(EEPROM_BASE_ADDR + EEPROM_INPUT_ADDR, tempByte);
    }
    else if(toggle == &output) {
        EEPROM.put(EEPROM_BASE_ADDR + EEPROM_OUTPUT_ADDR, tempByte);
    }
}

void AmpControl::startReset() {
    if(reset) {
        return;
    }
    reset = true;
    resetTimer.begin(endResetTrigger, 1000 * RESET_DELAY); // delay is in micros
    digitalWrite(RESET_PIN, LOW);
}


//////////////////////////////////////////////
// Private
//////////////////////////////////////////////


void AmpControl::endResetTrigger() {
    instance->endReset();
}

void AmpControl::endReset() {
    if(!reset) {
        return;
    }
    digitalWrite(RESET_PIN, HIGH);
    resetTimer.end();
    // Wanted a bit more delay to give time  for the amp to come out of reset
    resetTimer.begin(endResetFinishTrigger, 1000 * RESET_DELAY / 2);
}

void AmpControl::endResetFinishTrigger() {
    instance->endResetFinish();
}

void AmpControl::endResetFinish() {
    if(!reset) {
        return;  // yeah dont call this function yourself
    }
    resetTimer.end();
    reset = false;
}

void AmpControl::faultTrigger() {
    instance->faultISR();
}

void AmpControl::faultISR(){
    if(reset) {
        return; // do not update during a reset
    }
    fault = !(fault);
    updateCtrl = true;
}

void AmpControl::clipTrigger() {
    instance->clipISR();
}

void AmpControl::clipISR() {
    if(reset) {
        return; // do not update during a reset
    }
    clip = !(clip);
    updateCtrl = true;
}

void AmpControl::toggleBool(boolean *toggle) {
    *toggle = !(*toggle);
}

uint8_t AmpControl::boolToByte(boolean inputBool) {
    if(inputBool) {
        return 1;
    }
    else {
        return 0;
    }
}

boolean AmpControl::byteToBool(uint8_t inputByte) {
    if(inputByte == 0) {
        return 0;
    }
    else {
        return 1;
    }
}
