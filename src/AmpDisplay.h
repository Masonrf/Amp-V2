#ifndef AmpDisplay_h
#define AmpDisplay_h

#include "LiquidCrystalDisplay.h"
#include "AmpControl.h"
#include "AmpADC.h"

// Page Numbers
#define FAN_PAGE      1
#define FFT_PAGE      2
#define INPUTS_PAGE   3
#define MAIN_PAGE     4
#define OUTPUTS_PAGE  5

// Object IDs
#define HOME_INPUT_BTN          1
#define HOME_OUTPUT_BTN         2
#define HOME_FFT_BTN            3
#define HOME_FAN_BTN            4
#define HOME_RESET_BTN          5
#define HOME_CLIP_INDICATOR     6
#define HOME_FAULT_INDICATOR    7
#define HOME_L_VU_METER         8
#define HOME_R_VU_METER         9

#define INPUT_BACK_BTN          1
#define INPUT_SE_INDICATOR      2
#define INPUT_DIFF_INDICATOR    3
#define INPUT_SE_BTN            4
#define INPUT_DIFF_BTN          5

#define OUTPUT_BACK_BTN          1
#define OUTPUT_POST_INDICATOR    2
#define OUTPUT_SPEAKON_INDICATOR 3
#define OUTPUT_POST_BTN          4
#define OUTPUT_SPEAKON_BTN       5

#define FFT_BACK_BTN            1
#define FFT_L_CH_SELECTOR       2
#define FFT_R_CH_SELECTOR       3

#define FAN_BACK_BTN            1
#define FAN_DIAL_GUAGE          2
#define FAN_NUMERICAL_DISPLAY   3
#define FAN_SPEED_SLIDER        4

// Refresh rate in frames per second
#define MAX_REFRESH_RATE 24

// Inherits public items from LiquidCrystal library
class AmpDisplay: public LiquidCrystal, public AmpControl, public AmpADC {
public:
    AmpDisplay();

    void refreshDisplay();

private:
    elapsedMillis displayRefreshTimer;

    uint8_t   cmd_buffer[CMD_MAX_SIZE];
    uint8_t   data_size;
    uint8_t   update_en;
    uint8_t   current_page;

    void ProcessMessage(PCTRL_MSG msg, uint16_t dataSize);

    void UpdateUI();

    void NotifyTouchButton(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value);
    void NotifyTouchCheckbox(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value);
    void NotifyTouchSlider(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value);
    void NotifyTouchEdit(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value);
    void NotifyGetEdit(PEDIT_MSG msg);
    void NotifyGetTouchEdit(PEDIT_MSG msg);
    void NotifyGetPage(uint8_t page_id,uint8_t status);
    void NotifyGetCheckbox(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value);
    void NotifyGetSlider(uint8_t page_id, uint8_t control_id, uint8_t  state,uint8_t type,uint8_t value);

    void setIndicator(uint8_t pageID, uint8_t indicatorID, bool indicatorVar);
    void setIndicator(uint8_t pageID, uint8_t indicatorIdTrue, uint8_t indicatorIdFalse, bool indicatorVar);
};

#endif
