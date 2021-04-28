/*
 * All functions related to I/O handling for the Teensy
 */

#ifndef AmpControl_h
#define AmpControl_h

#include "Arduino.h"
#include <EEPROM.h>

// PINs
#define CLIP_PIN      0
#define FAULT_PIN     1
#define RESET_PIN     2
#define FAN_PWM_PIN   3
#define TACH_PIN      4
#define IN_RELAY_PIN  5
#define OUT_RELAY_PIN 6
#define LCD_INT_PIN   20

#define RESET_DELAY   500    // Time in ms to hold the reset pin low
#define FAN_PWM_FREQ  25000  // PWM frequncy in Hz

/* These EEPROMs are rated for >100k writes per address. Realistically, you probably
 * won't wear them out to the point of failure any time soon. However, you can adjust
 * the base address to move the block of stored data elsewhere.
 */
#define EEPROM_BASE_ADDR   0
#define EEPROM_MAX_ADDR    1079  // Teensy 4.0 has 1080 bytes of EEPROM
// Starting addresses for each var
#define EEPROM_INPUT_ADDR  0
#define EEPROM_OUTPUT_ADDR 1
#define EEPROM_FAN_ADDR    2


class AmpControl {
protected:
    AmpControl();

    volatile boolean fault;  // 0 == no fault,    1 == fault
    volatile boolean clip;   // 0 == no clip,     1 == clipping
    boolean input;           // 0 == 3.5mm (SE),  1 == XLR (Diff)
    boolean output;          // 0 == SpeakOns,    1 == Posts
    volatile boolean reset;

    boolean updateCtrl;     // Tell the display to update

    uint8_t fanDutyCycle;

    void toggleRelay(boolean *toggle);
    void startReset();


private:
    uint8_t tempByte;
    IntervalTimer resetTimer;
    static AmpControl *instance;

    static void endResetTrigger();
    static void endResetFinishTrigger();
    static void faultTrigger();
    static void clipTrigger();

    void endReset();
    void endResetFinish();
    void faultISR();
    void clipISR();

    void toggleBool(boolean *toggle);
    uint8_t boolToByte(boolean inputBool);
    boolean byteToBool(uint8_t inputByte);
};

#endif
