#include "LiquidCrystalDisplay.h"
#include "Arduino.h"

// PINs
#define CLIP_PIN      0
#define FAULT_PIN     1
#define RESET_PIN     2
#define PWM_PIN       3
#define TACH_PIN      4
#define IN_RELAY_PIN  5
#define OUT_RELAY_PIN 6
#define ADC_L_PIN     14
#define ADC_R_PIN     15
#define LCD_INT_PIN   20

// Display Page Numbers
#define FAN_PAGE      1
#define FFT_PAGE      2
#define INPUTS_PAGE   3
#define MAIN_PAGE     4
#define OUTPUTS_PAGE  5
