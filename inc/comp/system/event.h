#ifndef _EVENT_H_
#define _EVENT_H_

#include "sys_inc.h"
#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <semphr.h>
#include "queue.h"

#define COMMON_MSG_SIZE                 36
#define PLAYER_MSG_SIZE                 10

#define EVENT_GROUP_MASK                (0xFF000000)

#define NONE_EVENT                      0
#define DISP_EVENT                      0x60000000
#define RTC_EVENT                       0x70000000
#define PMU_EVENT                       0x80000000
#define EQ_EVENT                        0x90000000
#define DEVICE_EVENT                    0xA0000000
#define USB_EVENT                       0xA1000000
#define SYSTEM_EVENT                    0xB0000000
#define KEY_EVENT                       0xC0000000
//#define BT_EVENT                      0xD0000000
//#define AR_EVENT                      0xE0000000
#define LE_EVENT                        0xD1000000
#define PLAYER_EVENT                    0xF0000000
#define BTAUDIO_EVENT                   0xF1000000
#define EWS_AUDIO_EVENT                 0xF2000000

typedef enum {
    EWS_AUDIO_EVENT_NONE                = 0,
    EWS_AUDIO_EVENT_A2DP_START          = EWS_AUDIO_EVENT | 0x01,
    EWS_AUDIO_EVENT_A2DP_STOP           = EWS_AUDIO_EVENT | 0x02,
    EWS_AUDIO_EVENT_SCO_START           = EWS_AUDIO_EVENT | 0x03,
    EWS_AUDIO_EVENT_SCO_STOP            = EWS_AUDIO_EVENT | 0x04,
    EWS_AUDIO_EVENT_PLAY_TONE           = EWS_AUDIO_EVENT | 0x05,
    EWS_AUDIO_EVENT_STOP_TONE           = EWS_AUDIO_EVENT | 0x06,

    EWS_AUDIO_EVENT_CHANGE_VOLUME       = EWS_AUDIO_EVENT | 0x07,
    EWS_AUDIO_EVENT_NOTIFY_VOLUME_DOWN  = EWS_AUDIO_EVENT | 0x08,
    EWS_AUDIO_EVENT_NOTIFY_VOLUME_UP    = EWS_AUDIO_EVENT | 0x09,

    EWS_AUDIO_EVENT_INCOMING_TONE_STEP1 = EWS_AUDIO_EVENT | 0x0A,
    EWS_AUDIO_EVENT_INCOMING_TONE_STEP2 = EWS_AUDIO_EVENT | 0x0B,
} ews_audio_event_t;

typedef enum {
    DISP_EVENT_NONE                     = 0,
    DISP_EVENT_UPDATE                   = DISP_EVENT | 0x01,
    DISP_EVENT_SWITCH                   = DISP_EVENT | 0x02,
} disp_event_t;

typedef enum {
    RTC_EVENT_NONE                      = 0,
    RTC_EVENT_SECOND                    = RTC_EVENT | 0x01,
    RTC_EVENT_ALM0                      = RTC_EVENT | 0x02,
    RTC_EVENT_ALM1                      = RTC_EVENT | 0x03,
} rtc_event_t;

typedef enum {
    PMU_EVENT_NONE                      = 0,
    PMU_EVENT_BAT_LOW                   = PMU_EVENT | 0x01,
    PMU_EVENT_BAT_FULL                  = PMU_EVENT | 0x02,
    PMU_EVENT_BAT_TO                    = PMU_EVENT | 0x03,
    PMU_EVENT_POWERKEY_US               = PMU_EVENT | 0x04,
    PMU_EVENT_POWERKEY_S                = PMU_EVENT | 0x05,
    PMU_EVENT_POWERKEY_L                = PMU_EVENT | 0x06,
    PMU_EVENT_INT                       = PMU_EVENT | 0x07,
    PMU_EVENT_CHARGE_IN                 = PMU_EVENT | 0x08,
    PMU_EVENT_CHARGE_OUT                = PMU_EVENT | 0x09,
    PMU_EVENT_HSW_OFF                   = PMU_EVENT | 0x0a,
    PMU_EVENT_HSW_ON                    = PMU_EVENT | 0x0b,
    PMU_EVENT_VBUSUV                    = PMU_EVENT | 0x0c,
    PMU_EVENT_BAT_UNLOW                 = PMU_EVENT | 0x0d,
    PMU_EVENT_BAT_LOW_WRN               = PMU_EVENT | 0x0e,
} pmu_event_t;

typedef enum {
    EQ_EVENT_NONE                       = 0,
    EQ_EVENT_SET_MODE_EQ                = EQ_EVENT | 0x01,
    EQ_EVENT_SET_MODE_AUDIO             = EQ_EVENT | 0x02,
    EQ_EVENT_SET_EQ_DATA                = EQ_EVENT | 0x03,
    EQ_EVENT_SET_EQ_DATA_ISR            = EQ_EVENT | 0x04,
    EQ_EVENT_UPLOAD_RAM_EQ              = EQ_EVENT | 0x05,

    EQ_EVENT_AFX_TEST                   = EQ_EVENT | 0x80,
} eq_event_t;

typedef enum {
    DEVICE_EVENT_NONE                   = 0,
    DEVICE_EVENT_SD_IN                  = DEVICE_EVENT | 0x01,  //sdcard in
    DEVICE_EVENT_SD_OUT                 = DEVICE_EVENT | 0x02,  //sdcard out
    DEVICE_EVENT_SD_FAIL                = DEVICE_EVENT | 0x03,  //sdcard fail
    DEVICE_EVENT_UDISK_IN               = DEVICE_EVENT | 0x04,  //udisk in
    DEVICE_EVENT_UDISK_OUT              = DEVICE_EVENT | 0x05,  //udisk out
    DEVICE_EVENT_PC_IN                  = DEVICE_EVENT | 0x06,  //pc in
    DEVICE_EVENT_PC_OUT                 = DEVICE_EVENT | 0x07,  //pc out
    DEVICE_EVENT_LINE_IN                = DEVICE_EVENT | 0x08,
    DEVICE_EVENT_LINE_OUT               = DEVICE_EVENT | 0x09,
} device_event_t;

typedef enum {
    USB_EVENT_NONE                      = 0,
    USB_EVENT_PC_CONNECT                = USB_EVENT | 0x01,
    USB_EVENT_PC_CONNECT_TIMEOUT        = USB_EVENT | 0x02,
    USB_EVENT_PC_RX_SOF                 = USB_EVENT | 0x03,
    USB_EVENT_PCM_VOLUME                = USB_EVENT | 0x04,     //pc pcm volume set
    USB_EVENT_MIC_VOLUME                = USB_EVENT | 0x05,     //pc mic volume set
    USB_EVENT_TEST_UNIT                 = USB_EVENT | 0x06,
} usb_event_t;

typedef enum {
    SYSTEM_EVENT_NONE                   = 0,
    SYSTEM_EVENT_MODE_CHANGE            = SYSTEM_EVENT | 0x1,
    SYSTEM_EVENT_POWEROFF               = SYSTEM_EVENT | 0x2,
    SYSTEM_EVENT_UPDATE_STANBY          = SYSTEM_EVENT | 0x3,
    SYSTEM_EVENT_SECOND                 = SYSTEM_EVENT | 0x4,
} system_event_t;

enum le_event_e {
    LE_EVENT_NONE                       = 0,
    LE_EVENT_CONNECTED                  = LE_EVENT | 0x1,
    LE_EVENT_DISCONNECTED               = LE_EVENT | 0x2,
    LE_EVENT_APP                        = LE_EVENT | 0x3,

};

void event_init(void);
void event_put(uint32_t event);
void event_force_put(uint32_t event);
uint32_t event_get(bool wait);
uint8_t event_cnt_get(void);
uint16_t event_clear(uint32_t event);
uint16_t event_group_clear(uint32_t event_group);

#endif
