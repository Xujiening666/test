#ifndef _KEY_H_
#define _KEY_H_

#include "event.h"
#include "key_adc.h"
#include "key_ir.h"
#include "key_io.h"
#include "key_coding.h"
#include "irrx.h"
#include "gpio.h"
#include "libc_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


typedef enum {
    KEY_EVENT_NONE  = 0,
    KEY_EVENT_S     = KEY_EVENT | 0x00100000,   //short
    KEY_EVENT_SU    = KEY_EVENT | 0x00200000,   //short up
    KEY_EVENT_L     = KEY_EVENT | 0x00300000,   //long
    KEY_EVENT_LU    = KEY_EVENT | 0x00400000,   //long up
    KEY_EVENT_H     = KEY_EVENT | 0x00500000,   //hold

    KEY_EVENT_KNOB  = KEY_EVENT | 0x00600000,   //tune
} key_event_t;

#define KEY_CNT_S           5
#define KEY_CNT_U           5
#define KEY_CNT_L           150
#define KEY_CNT_H           30
#define KEY_CNT_LH          (KEY_CNT_L + KEY_CNT_H)

#define KEY_ADC_CNT_L       20
#define KEY_ADC_CNT_H       5
#define KEY_ADC_CNT_LH      (KEY_ADC_CNT_L + KEY_ADC_CNT_H)

#define KEY_MASK            0xFF
#define KEY_NONE            0x00

//num key
#define KEY_NUM0            0x10
#define KEY_NUM1            0x11
#define KEY_NUM2            0x12
#define KEY_NUM3            0x13
#define KEY_NUM4            0x14
#define KEY_NUM5            0x15
#define KEY_NUM6            0x16
#define KEY_NUM7            0x17
#define KEY_NUM8            0x18
#define KEY_NUM9            0x19


//function key
#define KEY_PREV            0x20
#define KEY_PLAY            0x21
#define KEY_NEXT            0x22
#define KEY_VOL_DOWN        0x23
#define KEY_VOL_UP          0x24
#define KEY_MODE            0x25
#define KEY_REPEAT          0x26
#define KEY_EQ              0x27
#define KEY_PREV_VOLDOWN    0x28
#define KEY_NEXT_VOLUP      0x29
#define KEY_HSF             0x2A
#define KEY_POWER           0x2B
#define KEY_PLAY_POWER      0x2C
#define KEY_PLAY_MODE       0x2D
#define KEY_CH_DOWN         0x2E
#define KEY_CH_UP           0x2F
#define KEY_N100_PLUS       0x30
#define KEY_N10_PLUS        0x31
#define KEY_N10_MINUS       0x32
#define KEY_VSS             0x33
#define KEY_REC             0x34
//#define KEY_SIRI            0x35
#define KEY_PLAY_REC        0x36
#define KEY_PLAY_SAVE_REC   0x37
#define KEY_HP_IN           0x38
#define KEY_HP_OUT          0x39
#define KEY_BLE_START_RECORD                 0x40
#define KEY_BLE_STOP_RECORD                  0x41
#define KEY_BLE_START_SEND_RECORD_BY_ID      0x43
#define KEY_BLE_SEND_DIR_FILES_LIST          0x44
#define KEY_BLE_SEND_DEVICE_STATS            0x45
#define KEY_BLE_START_SEND_RECORD_BY_NAME    0x46
#define KEY_UNBIND_USER                      0x47








typedef struct{
    uint8_t cur_val;
    uint8_t last_val;
    uint8_t cur_cnt;
    uint8_t up_cnt;
} key_scan_t;

void key_init(void);
uint32_t key_get_value(void);
void key_scan(void);

void mode_update(int value);

#endif

