#include "AmpControl.h"

AmpControl::AmpControl() {

    pinMode(IN_RELAY_PIN, OUTPUT);
    //input = 0;
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

    tempByte = boolToByte(*toggle);
    if(toggle == &input) {
        EEPROM.put(EEPROM_BASE_ADDR + EEPROM_INPUT_ADDR, tempByte);
    }
    else if(toggle == &output) {
        EEPROM.put(EEPROM_BASE_ADDR + EEPROM_OUTPUT_ADDR, tempByte);
    }

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
