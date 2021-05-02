#include "AmpDisplay.h"

AmpDisplay::AmpDisplay() : LiquidCrystal(LCD_INT_PIN), AmpControl(), AmpADC() {
    update_en = 1;
    data_size = 0;
    SetPage(MAIN_PAGE);
    current_page = MAIN_PAGE;
}

void AmpDisplay::UpdateUI() {
    switch(current_page) {
        case MAIN_PAGE:
            setIndicator(MAIN_PAGE, HOME_CLIP_INDICATOR, clip);
            setIndicator(MAIN_PAGE, HOME_FAULT_INDICATOR, fault);


            break;

        case INPUTS_PAGE:
            setIndicator(INPUTS_PAGE, INPUT_DIFF_INDICATOR, INPUT_SE_INDICATOR, input);
            break;

        case OUTPUTS_PAGE:
            setIndicator(OUTPUTS_PAGE, OUTPUT_POST_INDICATOR, OUTPUT_SPEAKON_INDICATOR, output);
            break;

        case FAN_PAGE:
            SetCircleGaugeValue(FAN_PAGE, FAN_DIAL_GUAGE, map(fanDutyCycle, 0, 255, 0, 180));
            SetSliderValue(FAN_PAGE, FAN_SPEED_SLIDER, map(fanDutyCycle, 0, 255, 0, 100));
            //SetNumberValue(FAN_PAGE, FAN_NUMERICAL_DISPLAY, fanDutyCycle);
            break;

        default:
            break;
    }
}

// Tries to check for any commands in the queue and updates the display if necessary
// There's a limit on how often the MCU can update the display so that the MCU
// isn't constantly checking for updates and eating up cycles
void AmpDisplay::refreshDisplay() {
    if(displayRefreshTimer >= 1000 / MAX_REFRESH_RATE) { // Get max number of milliseconds per frame
        displayRefreshTimer -= 1000 / MAX_REFRESH_RATE;

        data_size = check_for_cmd(cmd_buffer);
        if(data_size > 0) { // recieved a command
            noInterrupts();
            //Serial.println(data_size, HEX);
            //Serial.println(F("ProcessMessage"));
            ProcessMessage((PCTRL_MSG)cmd_buffer, data_size);//command process
            interrupts();
        }

        // Items that need constant updates on main page
        if(current_page == MAIN_PAGE) {
            getRMS();  // calculate rms in dB
            //printRMS();
            SetProgressbarValue( MAIN_PAGE, HOME_L_VU_METER, map(rmsL, 24, 54, 0, 100) );
            SetProgressbarValue( MAIN_PAGE, HOME_R_VU_METER, map(rmsR, 24, 54, 0, 100) );
        }

        /*
        // Items that need constant updates on FFT page
        if(current_page == FFT_PAGE) {
            update_en == ;
        }
        */

        // Items that only update occasionally for ALL pages
        if(update_en || updateCtrl) {
            //Serial.println(F("UpdateUI"));
            updateCtrl = false;
            update_en = false;
            UpdateUI();
        }
    }
}

//////////////////////////////////////////////
// Private
//////////////////////////////////////////////

void AmpDisplay::ProcessMessage(PCTRL_MSG msg, uint16_t dataSize) {
    uint8_t cmd_type    = msg->cmd_type;
    uint8_t control_id  = msg->control_id;
    uint8_t page_id     = msg->page_id;
    uint8_t _status     = msg->status;
    uint8_t key_type    = msg->key_type;
    uint8_t key_value   = msg->key_value;

    switch(cmd_type) {
        case BUTTON:
            NotifyTouchButton(page_id,control_id,_status,key_type,key_value);
            break;

        case CHECKBOX_SEND:
            NotifyTouchCheckbox(page_id,control_id,_status,key_type,key_value);
            break;

        case SLIDER:
            NotifyTouchSlider(page_id,control_id,_status,key_type,key_value);
            break;

        case TOUCH_EDIT_SEND_VALUE:
            NotifyTouchEdit(page_id,control_id,_status,key_type,key_value);
            break;

        case GET_EDIT_VALUE:
            NotifyGetEdit((PEDIT_MSG)cmd_buffer);
            break;

        case GET_TOUCH_EDIT_VALUE:
            NotifyGetTouchEdit((PEDIT_MSG)cmd_buffer);
            break;

        case GET_PAGE_ID:
            NotifyGetPage(page_id,_status);
            break;

        case GET_CHECKBOX_VALUE:
            NotifyGetCheckbox(page_id,control_id,_status,key_type,key_value);
            break;

        case GET_SLIDER_VALUE:
            NotifyGetSlider(page_id,control_id,_status,key_type,key_value);
            break;

        default:
            break;
  }
}

// Display notifying the MCU that a button has been touched
void AmpDisplay::NotifyTouchButton(uint8_t page_id, uint8_t control_id, uint8_t state, uint8_t type, uint8_t value) {
    switch(type) {
        case CHANGE_PAGE:
            if(value == FAN_PAGE) { // Changing to fan page
                //attach interrupt
            }
            else if(current_page == FAN_PAGE){ // Changing from fan page to another page
                // check its not detached & detach

                /* I have the MCU to write to EEPROM whenever it switches off the fan page
                 * in order to keep number of writes to a minimum. In the future, it
                 *  may be better to just set a timer to detect if its been awhile since
                 * the fanDutyCycle has been updated
                 */
                EEPROM.put(EEPROM_BASE_ADDR + EEPROM_FAN_ADDR, fanDutyCycle);
            }
            current_page = value;
            update_en = 1;
            break;

        case ENTER:
            if(state == KEY_RELEASE) {
                GetEditValue(page_id,value); // I think this is old/from example
            }
            break;

        case CHAR:  // Nothing of this type yet
            break;

        case UPLOAD_CONTROL_ID:
            switch(page_id) {
                case FAN_PAGE:
                    break;

                case FFT_PAGE:
                    break;

                case INPUTS_PAGE: // hmm maybe change else if to an else to improve perf? basically copy mainpage to input and output
                    if((control_id == INPUT_SE_BTN && input) || (control_id == INPUT_DIFF_BTN && !input)) {
                        update_en = 1;
                        toggleRelay(&input);
                    }
                    break;

                case MAIN_PAGE:
                    // Check to make sure its the right button even though there's only
                    // one of type UPLOAD_CONTROL_ID on this page
                    if(control_id == HOME_RESET_BTN) {
                        startReset();
                    }
                    break;

                case OUTPUTS_PAGE:
                    if((control_id == OUTPUT_SPEAKON_BTN && output) || (control_id == OUTPUT_POST_BTN && !output)) {
                        update_en = 1;
                        toggleRelay(&output);
                    }
                    break;

                default:
                    break;
            }

            break;

        case BTN_TYPE_CLEAR:  // Nothing of this type yet
            break;

        default:
            break;
    }
}

// Display notifying the MCU that a checkbox has been touched
void AmpDisplay::NotifyTouchCheckbox(uint8_t page_id, uint8_t control_id, uint8_t state,uint8_t type, uint8_t value) {
    if(state == SELECT) {
        update_en = 1;
    }
}

// Display notifying the MCU that a slider has been touched
void AmpDisplay::NotifyTouchSlider(uint8_t page_id, uint8_t control_id, uint8_t state, uint8_t type, uint8_t value) {
    if(page_id == FAN_PAGE) {
        // value is in the range 0-100 so here's getting it into analogWrite duty cycle 0-255
        value = map(value, 0, 100, 0, 255);
        fanDutyCycle = value;
        analogWrite(FAN_PWM_PIN, fanDutyCycle);
        update_en = 1;
    }
}

// Display notifying the MCU that an edit box has been touched
void AmpDisplay::NotifyTouchEdit(uint8_t page_id, uint8_t control_id, uint8_t state, uint8_t type, uint8_t value) {
    if(update_en != 1) {
        GetTouchEditValue(page_id,control_id);
    }
}

// Not in use currently and I dont want to screw around with the example code
// thats currently in here
void AmpDisplay::NotifyGetEdit(PEDIT_MSG msg) {
    /*
  //uint8_t cmd_type    = msg->cmd_type;  //command
  //uint8_t control_id  = msg->control_id;//object Id
  //uint8_t page_id     = msg->page_id;   //page Id
  //uint8_t _status     = msg->status;


  //The test passward number 1 2 3 4,ASCII code is 0x31 0x32 0x33 0x34
  if(msg->param[0] == 0x31 && msg->param[1] == 0x32 && msg->param[2] == 0x33 && msg->param[3] == 0x34)
  {
    Display_Message(0X18,2,(unsigned char *)String01);
  }
  else
  {
    Display_Message(0X18,2,(unsigned char *)String02);
  }
  */
}

// Same note as above
void AmpDisplay::NotifyGetTouchEdit(PEDIT_MSG msg) {
    /*
  //uint8_t cmd_type    = msg->cmd_type;  //command
  //uint8_t control_id  = msg->control_id;//object Id
  //uint8_t page_id     = msg->page_id;   //page Id
  //uint8_t _status     = msg->status;


  //The test passward number 1 2 3 4,ASCII code is 0x31 0x32 0x33 0x34
  if(msg->param[0] == 0x31 && msg->param[1] == 0x32 && msg->param[2] == 0x33 && msg->param[3] == 0x34)
  {
    Display_Message(0X18,2,(unsigned char *)String04);
  }
  else
  {
    Display_Message(0X18,2,(unsigned char *)String05);
  }
  */
}

// Read current page off of the display
void AmpDisplay::NotifyGetPage(uint8_t page_id,uint8_t status) {
    if(status == SUCCESS) {
        current_page = page_id;
    }
}

// Read current checkbox value off the display
void AmpDisplay::NotifyGetCheckbox(uint8_t page_id, uint8_t control_id, uint8_t state, uint8_t type, uint8_t value) {
    if(state == SELECT) {
        update_en = 1;
    }
}

// Read current slider value off the display
void AmpDisplay::NotifyGetSlider(uint8_t page_id, uint8_t control_id, uint8_t state, uint8_t type, uint8_t value) {
    if(state == SUCCESS) {
        //success get value
    }
    update_en = 1;
}

void AmpDisplay::setIndicator(uint8_t pageID, uint8_t indicatorID, bool indicatorVar) {
    if(indicatorVar) {
        SetProgressbarValue(pageID, indicatorID, 100);
    }
    else {
        SetProgressbarValue(pageID, indicatorID, 0);
    }
}

void AmpDisplay::setIndicator(uint8_t pageID, uint8_t indicatorIdTrue, uint8_t indicatorIdFalse, bool indicatorVar) {
    if(indicatorVar) {
        SetProgressbarValue(pageID, indicatorIdTrue, 100);
        SetProgressbarValue(pageID, indicatorIdFalse, 0);
    }
    else {
        SetProgressbarValue(pageID, indicatorIdTrue, 0);
        SetProgressbarValue(pageID, indicatorIdFalse, 100);
    }
}
