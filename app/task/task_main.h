#ifndef _TASK_MAIN_H_
#define _TASK_MAIN_H_

#include "bt.h"
#include "music.h"
#include "record.h"
#include "fm.h"
#include "linein.h"
#include "clock.h"
#include "usbdev.h"
#include "spdif_mode.h"
#include "sleep.h"
#include "poweroff.h"
#include "charge.h"
#include "event.h"
#include "player.h"
#include "tone.h"
#include "key.h"
#include "disp.h"
#include "disk_update.h"
#include "info_mem.h"

//some modes of the main task:
enum mode_id_e {
    MODE_NULL,      // 0
    MODE_RECORD,    // 1
    MODE_MUSIC,     // 2
    MODE_BT,        // 3
    MODE_FM,        // 4
    MODE_LINEIN,    // 5
    MODE_USBDEV,    // 6
    MODE_SPDIF,     // 7
    MODE_CLOCK,     // 8
    MODE_SCENE_MAX, // 9

    MODE_SLEEP,     // 10
    MODE_POWEROFF,  // 11
    MODE_CHARGE,    // 12
    MODE_DISKUPDATE,// 13
    MODE_SYSTEM_MAX,// 14
};

enum switch_reason_e {
    SW_INIT,
    SW_KEY,
    SW_AUTO,
    SW_BTBG,
    SW_SDCARD,
    SW_UDISK,
    SW_CHARGE,
    SW_STARTUP_FIRST,
    SW_HSW,
    SW_VBUSIN,
    SW_LB
};

typedef void (*mode_hook_t)(void);

typedef struct mode_info_s {
    uint32_t mode;
    mode_hook_t hook;
} mode_info_t;

typedef struct {
    enum mode_id_e sta;        //scene status
    enum mode_id_e last_sta;   //last scene status
    enum switch_reason_e switch_reason;
    bool exit_flag;
} scene_mode_t;
extern scene_mode_t scene_mode;

typedef struct {
    uint32_t val;
    uint32_t bcnt;
    uint32_t tcnt;
} knum_t;
extern knum_t knum;

typedef struct {
   bool auto_shutdown_init;
   bool auto_shutdown_status;
   bool shutdown_ing;
} auto_shutdown_t;
extern auto_shutdown_t *auto_shutdown_context;

extern uint32_t poweroff_begin_time;
extern volatile bool key_mode;
extern bool key_mode_exit;

void var_init(void);
void task_main(void *pvParameters);
void common_event_process(uint32_t msg);
void bt_bg_preprocess(void);

void mode_update(int value);
void mode_set(uint32_t mode);
void mode_set_force(uint32_t mode);
void mode_switch(void);
void mode_switch_auto(void);
enum mode_id_e mode_get_sta(void);


void cpu_load_timer_cb(xTimerHandle xTimer);


#endif


