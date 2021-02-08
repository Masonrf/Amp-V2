#include "AmpDisplay.h"
//
AmpDisplay::AmpDisplay() : LiquidCrystal(LCD_INT_PIN), AmpControl() {
    update_en = 1;
    data_size = 0;
    SetPage(MAIN_PAGE);
    current_page = MAIN_PAGE;


    fault = 1; // for testing
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
        default:
            break;
    }
}

void AmpDisplay::refreshDisplay() {
    if(displayRefreshTimer >= 1000 / REFRESH_RATE) { // Get number of milliseconds per frame
        displayRefreshTimer -= 1000 / REFRESH_RATE;  // who cares about precise refresh timings anyways

        data_size = check_for_cmd(cmd_buffer);
        if(data_size > 0) { // recieved a command
            //Serial.println(data_size, HEX);
            //Serial.println(F("ProcessMessage"));
            ProcessMessage((PCTRL_MSG)cmd_buffer, data_size);//command process
        }

        if(update_en || updateCtrl) {
            //Serial.println(F("UpdateUI"));
            updateCtrl = 0;
            update_en = 0;
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

// Everything below is a "I have no fucking clue if all of it works for sure"
// and is mostly incomplete

void AmpDisplay::NotifyTouchButton(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value) {
    switch(type) {
        case CHANGE_PAGE:
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
                    //    resetAmp(1);
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

        case CLEAR:  // Nothing of this type yet
            break;

        default:
            break;
    }
}

void AmpDisplay::NotifyTouchCheckbox(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value) {
    if(state == SELECT) {
        update_en = 1;
    }
}

void AmpDisplay::NotifyTouchSlider(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value) {
    if(update_en != 1) {
        SetNumberValue(page_id,28,(uint16_t)value);
    }
}

void AmpDisplay::NotifyTouchEdit(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value) {
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

void AmpDisplay::NotifyGetPage(uint8_t page_id,uint8_t status) {
    if(status == SUCCESS) {
        current_page = page_id;
    }
}


void AmpDisplay::NotifyGetCheckbox(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value) {
    if(state == SELECT) {
        update_en = 1;
    }
}

void AmpDisplay::NotifyGetSlider(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value) {
    if(state == SUCCESS) {
        //success get value
    }
    update_en = 1;
}

void AmpDisplay::setIndicator(uint8_t pageID, uint8_t indicatorID, boolean indicatorVar) {
    if(indicatorVar) {
        SetProgressbarValue(pageID, indicatorID, 100);
    }
    else {
        SetProgressbarValue(pageID, indicatorID, 0);
    }
}

void AmpDisplay::setIndicator(uint8_t pageID, uint8_t indicatorIdTrue, uint8_t indicatorIdFalse, boolean indicatorVar) {
    if(indicatorVar) {
        SetProgressbarValue(pageID, indicatorIdTrue, 100);
        SetProgressbarValue(pageID, indicatorIdFalse, 0);
    }
    else {
        SetProgressbarValue(pageID, indicatorIdTrue, 0);
        SetProgressbarValue(pageID, indicatorIdFalse, 100);
    }
}
