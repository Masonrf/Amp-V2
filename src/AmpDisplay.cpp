#include "AmpDisplay.h"

EasyNex nexDisplay(Serial4);
AmpADC amp_adc;
AmpControl amp_control;

elapsedMillis displayRefreshTimer;

// Tries to check for any commands in the queue and updates the display if necessary
// There's a limit on how often the MCU can update the display so that the MCU
// isn't constantly checking for updates and eating up cycles
void refreshDisplay() {
    nexDisplay.NextionListen();
/*
    if(displayRefreshTimer >= 1000 / MAX_REFRESH_RATE) { // Get max number of milliseconds per frame
        displayRefreshTimer -= 1000 / MAX_REFRESH_RATE;

        // Items that need constant updates on main page
        if(nexDisplay.currentPageId == MAIN_PAGE) {
            amp_adc.getRMS();  // calculate rms in dB
            //printRMS();
            nexDisplay.writeNum( "rms_left.val",  map(amp_adc.rmsL, 24, 54, 0, 100) );
            nexDisplay.writeNum( "rms_right.val", map(amp_adc.rmsR, 24, 54, 0, 100) );

            // clip and fault signals. Probably don't need to poll here, but I'm lazy
            
        }

        // Items that only update occasionally for ALL pages
        if(amp_control.updateCtrl) {
            noInterrupts();     // dont want updateCtrl changing while sending data
            amp_control.updateCtrl = false;
            nexDisplay.writeNum("clip_ind.val", amp_control.clip);
            nexDisplay.writeNum("fault_ind.val", amp_control.fault);
            interrupts();
            
        }
    }
*/
}


// Reset button press event
void trigger0() {
    amp_control.startReset();
}

// Input select se button press event
void trigger1() {
    if(amp_control.input != 0) {    // se is not selected 
        setIndicator("se_sel.val", "diff_sel.val", amp_control.input);
        amp_control.toggleRelay(&(amp_control.input));
    }
}

// Input select diff button press event
void trigger2() {
    if(amp_control.input != 1) {    // diff is not selected 
        setIndicator("se_sel.val", "diff_sel.val", amp_control.input);
        amp_control.toggleRelay(&(amp_control.input));
    }
}

// Output select posts button press event
void trigger10() {
    if(amp_control.output != 0) {
        setIndicator("posts_sel.val", "speakon_sel.val", amp_control.output);
        amp_control.toggleRelay(&(amp_control.output));
    }
}

// Output select speakon button press event
void trigger11() {
    if(amp_control.output != 1) {
        setIndicator("posts_sel.val", "speakon_sel.val", amp_control.output);
        amp_control.toggleRelay(&(amp_control.output));
    }
}

// Exit fan page
void trigger3() {
    /*
     * I have the MCU to write to EEPROM whenever it switches off the fan page
     * in order to keep number of writes to a minimum. In the future, it
     *  may be better to just set a timer to detect if its been awhile since
     * the fanDutyCycle has been updated
     */
    EEPROM.put(EEPROM_BASE_ADDR + EEPROM_FAN_ADDR, amp_control.fanDutyCycle);
}

// Exit FFT page
void trigger4() {

}

// Fan slider move event
void trigger5() {
    amp_control.fanDutyCycle = map(nexDisplay.readNumber("fan_spd_ctrl.val"), 0, 100, 0, 255);
    analogWrite(FAN_PWM_PIN, amp_control.fanDutyCycle);
    nexDisplay.writeNum("fan_gauge.val", map(amp_control.fanDutyCycle, 0, 255, 0, 180));
}

// init input page
void trigger6() {
    setIndicator("diff_sel.val", "se_sel.val", amp_control.input);
}

// init ouput page
void trigger7() {
    setIndicator("posts_sel.val", "speakon_sel.val", amp_control.output);
}

// init fan page
void trigger8() {

}

// init fft page
void trigger9() {

}

void setIndicator(String indicatorID, bool indicatorVar) {
    if(indicatorVar) {
        nexDisplay.writeNum(indicatorID, 100);
    }
    else {
        nexDisplay.writeNum(indicatorID, 0);
    }
}

void setIndicator(String indicatorIdTrue, String indicatorIdFalse, bool indicatorVar) {
    if(indicatorVar) {
        nexDisplay.writeNum(indicatorIdTrue, 100);
        nexDisplay.writeNum(indicatorIdFalse, 0);
    }
    else {
        nexDisplay.writeNum(indicatorIdTrue, 0);
        nexDisplay.writeNum(indicatorIdFalse, 100);
    }
}
