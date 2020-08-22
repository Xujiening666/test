#ifndef _DISP_H_
#define _DISP_H_


#include "led_digital.h"
#include "led_diode.h"
#include "lcd_seg.h"
#include "ledc.h"
#include "timers.h"
#include "libc_common.h"
#include "event.h"
#include "player.h"
#include "task.h"
#include "device.h"
#include "task_main.h"


typedef struct {
    uint32_t menu;
    uint32_t count;
} disp_t;
extern disp_t disp;

#if LCD_SEG_EN
#define DISP_INTERVAL_TIME      3                           //interval time, unit:ms
#else
#define DISP_INTERVAL_TIME      100                         //interval time, unit:ms
#endif

#define DISP_DURATION_TIME      (2000/DISP_INTERVAL_TIME)   //duration time: 2s
#define DISP_FLASH_TIME         (200/DISP_INTERVAL_TIME)    //flash time: 400ms
#define DISP_FLASH_FAST_TIME    (200/DISP_INTERVAL_TIME)    //flash fast time: 200ms
#define DISP_FLASH_SLOW_TIME    (5000/DISP_INTERVAL_TIME)   //flash slow time: 5000ms
#define DISP_DELAY_OFF_TIME     (3000/DISP_INTERVAL_TIME)   //delay off: 3000ms
#define DISP_REC_START_TIME     (1000/DISP_INTERVAL_TIME)   //delay off: 3000ms


enum disp_menu_e {
    MENU_WELCOME        = 0,
    MENU_SCENE_MAIN,
    MENU_NUM,
    MENU_INPUT_NUM,
    MENU_VOLUME,
    MENU_EQ,
    MENU_REPEAT,
    MENU_WAITING,
    MENU_MUSIC_PLAY,
    MENU_MUSIC_STOP,
    MENU_CLOCK_TIME,
    MENU_FM_FREQ,
    MENU_FM_CHANNEL,
    MENU_RECORD_ING,
    MENU_RECORD_PLAY,
};

enum disp_state_e {
    //system
    STATE_POWERON       = 0,
    STATE_POWEROFF,
    STATE_RESET,

    //charge   3
    STATE_CHARGEING,
    STATE_UNCHARGEING,
    STATE_BAT_FULL,
    STATE_BAT_LOW,
    STATE_BAT_UNLOW,

    //bt    8
    STATE_BT_PAIR,
    STATE_BT_CON,
    STATE_BT_DIS,

    //music   11
    STATE_MUSIC_NODEV,
    STATE_MUSIC_PLAY,
    STATE_MUSIC_STOP,

    //fm     14
    STATE_FM_SEEK,
    STATE_FM_PLAY,
    
    //record  16
    STATE_REC_START,
    STATE_REC_END,
	STATE_REC_ERROR,

	//sunchip close led    19
//	STATE_CLOSE_RED,
//	STATE_CLOSE_BLUE,

};

enum disp_onoff_e {
    DISP_ON             = 0,
    DISP_OFF,
    DISP_FLASH,
    DISP_FLASH_FAST,
    DISP_FLASH_SLOW,
    DISP_ON_DELAY_OFF,
    DISP_FLASH_INTERVAL,
    DISP_REC_START,
};


void disp_init(void);
void disp_timer_cb(TimerHandle_t xTimer);
void disp_init(void);
void disp_menu(enum disp_menu_e menu, uint32_t val0, uint32_t va11);
void disp_state(enum disp_state_e state);


void disp_state_led_diode(enum disp_state_e state);


#endif

