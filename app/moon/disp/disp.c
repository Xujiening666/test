#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "disp.h"
#include "record.h"

disp_t disp;
TimerHandle_t DispTimerHandle;


AT(.disp_init_seg)
void disp_init(void)
{
#if DISP_EN
    if (DispTimerHandle == 0) {
        DispTimerHandle = xTimerCreate("DispTimer", pdMS_TO_TICKS(DISP_INTERVAL_TIME), pdTRUE, (void *)1, disp_timer_cb);
    }
    xTimerStart(DispTimerHandle, 10);

#if LED_DIGITAL_EN
    disp_led_digital_init();
#endif

#if LED_DIODE_EN
    disp_led_diode_init();
#endif

#if LCD_SEG_EN
    disp_lcd_seg_init();
#endif
#endif
}

AT(.disp_seg)
void disp_timer_cb(TimerHandle_t xTimer)
{
#if LED_DIGITAL_EN
    if (scene_mode.sta == MODE_MUSIC) {
        if (AP_STATUS_PLAYING == player_get_status()) {
            event_clear(DISP_EVENT_UPDATE);
            event_put(DISP_EVENT_UPDATE);
        }
    } else if (scene_mode.sta == MODE_RECORD ||
               scene_mode.sta == MODE_LINEIN ||
               scene_mode.sta == MODE_FM) {
#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC)
        if (context->status == REC_STATUS_RECORDING ||
            context->status == REC_STATUS_PLAYING) {
            event_clear(DISP_EVENT_UPDATE);
            event_put(DISP_EVENT_UPDATE);
        }
#endif
    }

#endif

#if LED_DIODE_EN
    disp_led_diode_timer();
#endif

#if LCD_SEG_EN
    disp_lcd_seg();
#endif

    if (disp.count < DISP_DURATION_TIME) {
        disp.count++;
        if(disp.count == DISP_DURATION_TIME) {
            disp.count = DISP_DURATION_TIME + 1;
            event_put(DISP_EVENT_SWITCH);
        }
    }
}

AT(.disp_seg)
void disp_menu(enum disp_menu_e menu, uint32_t val0, uint32_t va11)
{
    switch (menu) {
    case MENU_SCENE_MAIN:
    case MENU_NUM:
    case MENU_INPUT_NUM:
    case MENU_VOLUME:
    case MENU_EQ:
    case MENU_REPEAT:
    case MENU_WAITING:
    case MENU_CLOCK_TIME:
    case MENU_FM_CHANNEL:
        disp.menu = menu;
        disp.count = 0;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_WELCOME:
        disp.menu = menu;
        disp.count = 0xFF;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_FM_FREQ:
        disp.menu = MENU_SCENE_MAIN;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_MUSIC_STOP:
        disp.menu = menu;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_MUSIC_PLAY:
    case MENU_RECORD_PLAY:
    case MENU_RECORD_ING:
        if (disp.menu == MENU_SCENE_MAIN) {
#if LED_DIGITAL_EN
            disp_menu_led_digital(menu, val0, va11);
#endif
        }
        break;
    }
}

AT(.disp_seg)
void disp_state(enum disp_state_e state)
{
#if LED_DIODE_EN
    disp_state_led_diode(state);
#endif
}

AT(.disp_seg)
void disp_sleep(bool state)
{
#if (LED_DIGITAL_EN && DEBUG_DEEP_SLEEP_EN)
    disp_sleep_digital(state);
#endif
}

