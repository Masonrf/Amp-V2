#include "AmpDisplay.h"

EasyNex nexDisplay(SERIAL_PORT);
AmpADC amp_adc;
AmpControl amp_control;

elapsedMillis displayRefreshTimer;
bool periodicUpdates = true;

// Tries to check for any commands in the queue and updates the display if necessary
// There's a limit on how often the MCU can update the display so that the MCU
// isn't constantly checking for updates and eating up cycles
void refreshDisplay() {
    nexDisplay.NextionListen();

    if( periodicUpdates && ( displayRefreshTimer >= (1000 / MAX_REFRESH_RATE) ) ) { // Get max number of milliseconds per frame
        displayRefreshTimer -= (1000 / MAX_REFRESH_RATE);

        switch(nexDisplay.currentPageId) {
            case MAIN_PAGE:
                // Items that need constant updates on main page

                nexDisplay.writeNum( "rms_left.val", (uint32_t)map_rms_to_display(amp_adc.rmsL, RMS_MIN_VAL_DB, RMS_MAX_VAL_DB, 0, 100) );    // These need a max of about 66dB otherwise it sends the display
                nexDisplay.writeNum( "rms_right.val", (uint32_t)map_rms_to_display(amp_adc.rmsR, RMS_MIN_VAL_DB, RMS_MAX_VAL_DB, 0, 100) );   // values > 100 which bugs out the whole thing

                // clip and fault signals
                // may just want to remove the interrupts and poll these with a digitalReadFast. Would get rid of some of the weirdness with interrupts
                if(amp_control.updateCtrl) {
                    noInterrupts();     // dont want updateCtrl changing while sending data
                    amp_control.updateCtrl = false;
                    nexDisplay.writeNum("clip_ind.val", amp_control.clip);
                    nexDisplay.writeNum("fault_ind.val", amp_control.fault);
                    interrupts();
                }
                break;
            
            case FFT_PAGE:  // probably want to not use delays

                drawFFT(amp_adc.fftGraph0, 383);
                drawFFT(amp_adc.fftGraph1, 63553);

            default:
                break;
        }
    }

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
    uint32_t readDutyCyle = nexDisplay.readNumber("fan_spd_ctrl.val");
    amp_control.fanDutyCycle = map(readDutyCyle, 0, 100, 0, 255);
    analogWrite(FAN_PWM_PIN, amp_control.fanDutyCycle);
    nexDisplay.writeNum("fan_gauge.val", map(amp_control.fanDutyCycle, 0, 255, 0, 180));
}

// init input page
void trigger6() {
    setIndicator("diff_sel.val", "se_sel.val", amp_control.input);
}

// init ouput page
void trigger7() {
    setIndicator("speakon_sel.val", "posts_sel.val", amp_control.output);
}

// init fan page
void trigger8() {
    nexDisplay.writeNum("fan_gauge.val", map(amp_control.fanDutyCycle, 0, 255, 0, 180));
    nexDisplay.writeNum("fan_spd_ctrl.val", map(amp_control.fanDutyCycle, 0, 255, 0, 100));
}

// init fft page
void trigger9() {

}

// enable periodic updates
void trigger12() {
    periodicUpdates = true;
}

// disable periodic updates
void trigger13() {
    periodicUpdates = false;
}

// FFT L checkbox
void trigger14() {

}

// FFT R checkbox
void trigger15() {

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

// a slightly modified map() function for use with RMS levels in dB
uint8_t map_rms_to_display(float input, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
    uint32_t x = (uint32_t)input;

    if(x > in_max) {
        return out_max;
    }
    else if(x < in_min) {
        return out_min;
    }
    else {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
}

// draw one channel
void drawFFT(uint8_t fftGraph[], uint16_t color) {
    static uint16_t BandXVals[NUM_BANDS] = {13, 68, 115, 145, 159, 184, 204, 219, 239, 253, 263, 278, 291, 305, 321, 333, 341, 351, 358, 369, 381, 396, 406, 418, 422, 428, 436, 444, 454, 462, 468};
    nexDisplay.writeStr("ref 0");    // refresh screen

    for(int i = 0; i < NUM_BANDS - 2; i++) {
        nexDisplay.writeStr( fillRectCmd(BandXVals[i], 250-fftGraph[i], BandXVals[i + 1] - BandXVals[i] - 1, fftGraph[i], color) );
    }
}

// draw both channels
void drawFFT(uint8_t fftGraphL[], uint16_t colorL, uint8_t fftGraphR[], uint16_t colorR, uint16_t colorCombined) {

}

String fillRectCmd(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // "fill x,y,w,h,color"
    return "fill " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(color);
}