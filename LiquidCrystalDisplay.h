/*
	This library is primarily made up of the original Hunda LCD I2C library
	and example arduino sketch. All credit for that goes to
	martin xhli <http://www.hundatech.com>

	Here is a list of edits I (Masonrf) have made to that code:
	* Moved from the included I2C library to the Wire library
	* Small definition name changes for compatibility with the Teensy USB library
	* Moved the ISR into the class instead of the sketch
	* Moved queue operations from public to private
	* Moved example page number definitons to the sketch
	* Removed TFTInit function since it only triggered the LCD reset pin which isn't
		used for I2C
	* Removed the _int_pin variable since it wasn't used anywhere
	* Removed duplicate definitions
    * Cleaned up some formatting

    The purpose of this library is more oriented toward how the display handles lower level
    functions and interrupts. These functions will not change no matter what custom
    UI is made for the display.
*/

#ifndef LiquidCrystalDisplay_h
#define LiquidCrystalDisplay_h

#include <inttypes.h>
#include "Print.h"
#include <Wire.h>


#define LCD_ASC8_FONT				1
#define LCD_ASC12_FONT				2
#define LCD_ASC16_FONT				3

#define LCD_HZK16_FONT				4
#define LCD_HZK24_FONT				5
#define LCD_HZK32_FONT				6


///////////////////////////////////////////////////
#define CMD_ICON_DISPLAY			0x49
#define CMD_CUT_ICON				0x4f
#define CMD_CLEAR_SCREEN			0x43
#define CMD_STR_DISPLAY				0x53
#define CMD_DRAW_FILL_RECT			0x46
#define CMD_DRAW_RECT				0x52
#define CMD_DRAW_LINE				0x4c
#define CMD_DRAW_NUMBER				0x4e
#define CMD_SET_LCD_BRIGHTNESS		0x42
#define CMD_TP_CALIBRATION			0x50
#define CMD_TP_TEST					0x54
#define CMD_TP_SET					0x51
#define CMD_MESSAGE_DISPLAY			0x4d
#define CMD_CONNECT					0x48
#define CMD_RESET					0x4a

#define PAGE						0x70
#define GET_PAGE_ID					0x71 //NOTIFY_GET_PAGE
#define RECT 						0x72
#define LINE 						0x6c
#define LABLE 						0x74
#define BUTTON 						0x62 // NOTIFY_TOUCH_BUTTON
#define BOX							0x78
#define EDIT						0x65
#define PROGRESS					0x6F
#define CHECKBOX					0x63
#define IMAGE						0x69
#define CIRCLEGAUGE 				0x7a
#define BARGAUGE 					0x61
#define WATERGAUGE 					0x77
#define THERMOMETER 				0x6d
#define WAVEFORM 					0x66
#define BATTERY 					0x79
#define FORM 						0x73
#define SLIDER 						0x67 //NOTIFY_TOUCH_SLIDER
#define GET_SLIDER_VALUE 			0x68 //NOTIFY_GET_SLIDER
#define SET_SLIDER_VALUE 			0x5f
#define GET_CHECKBOX_VALUE 			0x64 //NOTIFY_GET_CHECKBOX
#define CHECKBOX_SEND				0x60 //NOTIFY_TOUCH_CHECKBOX
#define GET_EDIT_VALUE 				0x75 //NOTIFY_GET_EDIT
#define TOUCH_EDIT_SEND_VALUE 		0x7b //NOTIFY_TOUCH_EDIT
#define GET_TOUCH_EDIT_VALUE 		0x7c //NOTIFY_GET_TOUCH_EDIT
#define CLEAR_WAVEFORM 				0x76
#define INSER_WAVEFORM_DATA			0x6a
#define REFRESS_WAVEFORM			0x6b
#define NUMBER		 				0x6E


#define	CMD							0x00FF
#define	DAT							0x0100

#define HI							0x01
#define LO							0x00


#define LCD_RED 					0xF800
#define LCD_GREEN 					0x07e0
#define LCD_BLUE  					0x001F
#define LCD_LIGHT_BLUE 				0x075F

#define LCD_I2C_ADDRESS				119 // Wire uses decimal address. Hex is 0x77

#define CMD_MAX_SIZE 				65
#define QUEUE_MAX_SIZE 				256

#define CMD_DATA_LENGTH				6

#define  CHAR						0X30
#define  BTN_TYPE_CLEAR				0X31
#define  BACK_SPACE					0X32
#define  SPACE 						0X33
#define  ENTER						0X34
#define  CHANGE_PAGE				0X35
#define  UPLOAD_CONTROL_ID			0X36

#define SUCCESS						0x6f
#define FAILURE						0x65

#define LCD_KEY_DOWN	  			0x44 // ** Renamed to avoid a Teensy USB definition
#define KEY_RELEASE 				0x55

#define SELECT						0X53
#define UNSELECT					0X55

typedef uint8_t qdata;
typedef uint16_t qsize;

#define HEAD  0xaa

typedef struct _QUEUE {
    qsize _head;
	qsize _tail;
	qdata _data[QUEUE_MAX_SIZE];
}QUEUE;


typedef struct {
	uint8_t    cmd_type;
	uint8_t    control_id;
	uint8_t    page_id;
	uint8_t    status;
	uint8_t    key_type;
    uint8_t    key_value;
}CTRL_MSG,*PCTRL_MSG;

typedef struct {
	uint8_t    cmd_type;
	uint8_t    control_id;
	uint8_t    page_id;
	uint8_t    status;
	uint8_t    key_type;
    uint8_t    string_length;
	uint8_t    param[57];
}EDIT_MSG,*PEDIT_MSG;

class LiquidCrystal : public Print {
protected:

	LiquidCrystal(uint8_t isrPin);

  	virtual size_t write(uint8_t value);
  	void command(uint8_t value);

	void I2C_Add_CMD();

  	void SetReset();
 	void PutString(uint16_t x, uint16_t y, uint8_t DisplayType, uint8_t FontSize, uint8_t ImageNo, uint16_t BackColor, uint16_t ForeColor, uint8_t *strings);
	void Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t ForeColor);
	void Rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t ForeColor);
	void RectangleFill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t ForeColor);
	void ClrScreen(uint16_t Color);
	void Display_Image(uint16_t x, uint16_t y, uint8_t image_id);
	void DisplayCut_Image(uint16_t image_x, uint16_t image_y, uint16_t image_w, uint16_t image_h, uint8_t image_id);
	void Display_Message(uint8_t FontSize, uint8_t time, uint8_t *strings);
	void SetBackLight(uint8_t brightness);
	void SetTouchPaneOption(uint8_t enable);
	void CalibrateTouchPane();
	void TestTouchPane(uint8_t enable);

	void SetPage(uint8_t page_id);
	void GetPage();
	void SetLableValue(uint8_t page_id, uint8_t control_id, uint8_t *strings);
	void SetNumberValue(uint8_t page_id, uint8_t control_id, uint16_t number);
	void SetEditValue(uint8_t page_id, uint8_t control_id, uint8_t *strings);
	void GetEditValue(uint8_t page_id, uint8_t control_id);
	void SetProgressbarValue(uint8_t page_id, uint8_t control_id, uint8_t value);
	void SetCheckboxValue(uint8_t page_id, uint8_t control_id, uint8_t value);
	void GetCheckboxValue(uint8_t page_id, uint8_t control_id);
	void SetCircleGaugeValue(uint8_t page_id, uint8_t control_id, uint16_t value);
	void SetBarGaugeValue(uint8_t page_id, uint8_t control_id, uint16_t value);
	void SetWaterGaugeValue(uint8_t page_id, uint8_t control_id, uint16_t value);
	void SetThermometerValue(uint8_t page_id, uint8_t control_id, uint16_t value);
	void SetBatteryValue(uint8_t page_id, uint8_t control_id, uint16_t value);
	void SetWaveformValue(uint8_t page_id, uint8_t control_id, uint8_t channelNo, uint8_t value);
	void WaveformDataClear(uint8_t page_id, uint8_t control_id);
	void WaveformDataInsert(uint8_t page_id, uint8_t control_id, uint8_t channelNo, uint8_t value);
	void WaveformDataRefress(uint8_t page_id, uint8_t control_id, uint8_t channelNo);
	void GetSliderValue(uint8_t page_id, uint8_t control_id);
	void SetSliderValue(uint8_t page_id, uint8_t control_id, uint8_t value);
	void GetTouchEditValue(uint8_t page_id, uint8_t control_id);

	uint8_t check_for_cmd(qdata *cmd);

  	using Print::write;
private:

  	uint8_t  sendbuf[65];

  	void I2C_Send(uint8_t *buf, uint8_t len);
	uint8_t I2C_Read();

	static LiquidCrystal *sLCD;
	static void I2C_ISR();

	void queue_reset();
  	void queue_push(qdata _data);
  	void queue_pop(qdata* _data);
  	uint8_t queue_size();
  	uint8_t queue_find_cmd(qdata *cmd, qsize buf_len);

  	qsize cmd_pos;
  	qdata _length;
  	qdata cmd_backup;
  	qdata cmd_statu;
  	qdata cmd_length;

  	QUEUE que;
};

#endif
