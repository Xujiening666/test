#define LOG_LEV     4
#define SLTK_LOG_TAG "clock"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"


#if MODE_CLOCK_EN

clock_mode_t clock_mode;


AT(.mode_clock_seg)
void clock_alarm_play_tone(void)
{
#if (TONE_EN && TONE_ALARM_EN)
    if (clock_mode.sub_status != CLOCK_ALARM_PENDING_MODE) {
        clock_mode.tone_status = TONE_STATUS_COMMON;
        return;
    }
    clock_mode.tone_status = TONE_STATUS_ALARM;
    play_tone(TONE_ALARM, false);
#endif
}

AT(.mode_clock_seg)
void clock_disp_time_param(void)
{
    switch (rtc.params_select) {
    case SEL_YEAR:
        disp_menu(MENU_NUM, BASE_YEAR + rtc.cfg_t.year, 0);
        break;

    case SEL_MONTH:
        disp_menu(MENU_NUM, rtc.cfg_t.month, 0);
        break;

    case SEL_DAY:
        disp_menu(MENU_NUM, rtc.cfg_t.day, 0);
        break;

    case SEL_HOUR:
        disp_menu(MENU_NUM, rtc.cfg_t.hour, 0);
        break;

    case SEL_MINUTE:
        disp_menu(MENU_NUM, rtc.cfg_t.minute, 0);
        break;

    case SEL_SECOND:
        disp_menu(MENU_NUM, rtc.cfg_t.second, 0);
        break;
    }
}

AT(.mode_clock_seg)
void clock_disp_alarm_param(void)
{
    switch (rtc.params_select) {
    case SEL_HOUR:
        disp_menu(MENU_NUM, rtc.cfg_alarm_t.hour, 0);
        break;

    case SEL_MINUTE:
        disp_menu(MENU_NUM, rtc.cfg_alarm_t.minute, 0);
        break;
    }
}

AT(.mode_clock_seg)
void clock_time_display_event_process(uint32_t event)
{
    switch (event) {
    case RTC_EVENT_SECOND:
        rtc_get_time(&rtc.t);
        if (0 == rtc.t.second) {
            logi("aaartc:%04d-%02d-%02d, %02d:%02d:%02d",
                rtc.t.year+BASE_YEAR, rtc.t.month, rtc.t.day,
                rtc.t.hour, rtc.t.minute, rtc.t.second);
            if (CLOCK_TIME_DISPLAY_MODE == clock_mode.sub_status) {
                disp_menu(MENU_CLOCK_TIME,rtc.t.hour, rtc.t.minute);
            }
        }
        break;

    case KEY_EVENT_SU | KEY_PLAY:    //goto alarm_display_mode
        rtc_alarm_get();
        clock_mode.sub_status = CLOCK_ALARM_DISPLAY_MODE;
        disp_menu(MENU_CLOCK_TIME, rtc.alarm_time.hour, rtc.alarm_time.minute);
        logv("alarm time:[%d:%d:%d]",
            rtc.alarm_time.hour,
            rtc.alarm_time.minute,
            rtc.alarm_time.second);
        break;

    case KEY_EVENT_L | KEY_PLAY:    //goto time_setting_mode
        rtc.cfg_t = rtc.t;
        clock_mode.sub_status = CLOCK_TIME_SETTING_MODE;
        rtc.params_select = SEL_YEAR;
        clock_disp_time_param();
        logv("start set year");
        break;

    case KEY_EVENT_SU | KEY_VOL_UP: //enable/disable alarm
        rtc_alarm_switch();
        break;

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_clock_seg)
void clock_alarm_display_event_process(uint32_t event)
{
    switch (event) {
    case KEY_EVENT_SU | KEY_PLAY:   //goto time_display_mode
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
        disp_menu(MENU_CLOCK_TIME, rtc.t.hour, rtc.t.minute);
        logi("time:[%d-%d-%d, %d:%d:%d]",
            BASE_YEAR + rtc.t.year, rtc.t.month, rtc.t.day,
            rtc.t.hour, rtc.t.minute, rtc.t.second);
        break;

    case KEY_EVENT_L | KEY_PLAY:    //goto alarm_setting_mode
        rtc.cfg_alarm_t = rtc.alarm_time;
        clock_mode.sub_status = CLOCK_ALARM_SETTING_MODE;
        rtc.params_select = SEL_HOUR;
        clock_disp_alarm_param();
        logv("alarm setting mode, start set alarm Hours");
        break;

    case KEY_EVENT_SU | KEY_VOL_UP: //enable/disable alarm
        rtc_alarm_switch();
        break;

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_clock_seg)
void clock_time_setting_event_process(uint32_t event)
{
    switch (event) {
    case KEY_EVENT_SU | KEY_PLAY:   //goto set year, month, day, hour, minute, second
        rtc_select_next_time_param();
        clock_disp_time_param();
        logv("config next time params:%d", rtc.params_select);
        break;

    case KEY_EVENT_L | KEY_PLAY:    //set time, and goto time_display_mode
        rtc.t = rtc.cfg_t;
        rtc_set_time(&rtc.t);
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
        disp_menu(MENU_CLOCK_TIME, rtc.t.hour, rtc.t.minute);
        logi("save time");
        break;

    case KEY_EVENT_SU | KEY_PREV:   //-1 params
    case KEY_EVENT_H | KEY_PREV:
        rtc_config_time_param(-1);
        clock_disp_time_param();
        break;

    case KEY_EVENT_SU | KEY_NEXT:   //+1 params
    case KEY_EVENT_H | KEY_NEXT:
        rtc_config_time_param(1);
        clock_disp_time_param();
        break;

    case KEY_EVENT_SU | KEY_VOL_DOWN:   //do not save time, and goto time_display_mode
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
        disp_menu(MENU_CLOCK_TIME, rtc.t.hour, rtc.t.minute);
        logi("do not save time");
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:     // enable/disable alarm
        rtc_alarm_switch();
        break;

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_clock_seg)
void clock_alarm_setting_event_process(uint32_t event)
{
    switch (event) {
    case KEY_EVENT_SU | KEY_PLAY:       //goto set alarm hour, minute
        rtc_select_next_alarm_param();
        clock_disp_alarm_param();
        logv("config next alarm params:%d", rtc.params_select);
        break;

    case KEY_EVENT_L | KEY_PLAY:        //set alarm, and goto time_display_mode
        rtc.alarm_time = rtc.cfg_alarm_t;
        rtc.alarm_enable = true;
        rtc_alarm_set();
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
        disp_menu(MENU_CLOCK_TIME, rtc.t.hour, rtc.t.minute);
        logi("save alarm");
        break;

    case KEY_EVENT_SU | KEY_PREV:       //-1 params
    case KEY_EVENT_H | KEY_PREV:
        rtc_config_alarm_param(-1);
        clock_disp_alarm_param();
        break;

    case KEY_EVENT_SU | KEY_NEXT:       //+1 params
    case KEY_EVENT_H | KEY_NEXT:
        rtc_config_alarm_param(1);
        clock_disp_alarm_param();
        break;

    case KEY_EVENT_SU | KEY_VOL_DOWN:   //do not set alarm, and goto time_display_mode
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
        disp_menu(MENU_CLOCK_TIME, rtc.t.hour, rtc.t.minute);
        logi("do not save alarm");
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:     //enable/disable alarm
        rtc_alarm_switch();
        break;

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_clock_seg)
void clock_alarm_pending_event_process(uint32_t event)
{
    switch (event) {
    case RTC_EVENT_SECOND:
        rtc_get_time(&rtc.t);
        if (0 == rtc.t.second) {
            logi("rtc:%04d-%02d-%02d, %02d:%02d:%02d",
                rtc.t.year+BASE_YEAR, rtc.t.month, rtc.t.day,
                rtc.t.hour, rtc.t.minute, rtc.t.second);
            //disp_menu(MENU_CLOCK_TIME,rtc.t.hour, rtc.t.minute);
        }
        break;

    case KEY_EVENT_SU | KEY_PLAY:
        logi("exit.");
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
        break;
    }
}

AT(.mode_clock_seg)
void clock_event_process(uint32_t event)
{
    switch (clock_mode.sub_status) {
    case CLOCK_TIME_DISPLAY_MODE:
        clock_time_display_event_process(event);
        break;

    case CLOCK_TIME_SETTING_MODE:
        clock_time_setting_event_process(event);
        break;

#if (RTC_ALARM0_EN || RTC_ALARM1_EN)
    case CLOCK_ALARM_DISPLAY_MODE:
        clock_alarm_display_event_process(event);
        break;

    case CLOCK_ALARM_SETTING_MODE:
        clock_alarm_setting_event_process(event);
        break;

    case CLOCK_ALARM_PENDING_MODE:
        clock_alarm_pending_event_process(event);
        break;
#endif

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_clock_seg)
void mode_clock(void)
{
    logi("clock mode.");
    mode_clock_enter();
    mode_clock_loop();
    mode_clock_exit();
}

AT(.mode_clock_seg)
void mode_clock_enter(void)
{
    if (clock_mode.sub_status == CLOCK_ALARM_PENDING_MODE) {
        clock_alarm_play_tone();
    } else {
#if (TONE_EN && TONE_CLOCK_MODE_EN)
        play_tone(TONE_CLOCK_MODE, true);
#endif
        clock_mode.sub_status = CLOCK_TIME_DISPLAY_MODE;
    }

    rtc_get_time(&rtc.t);
    disp_menu(MENU_CLOCK_TIME, rtc.t.hour, rtc.t.minute);
    player_mode_switch_proc(MODE_CLOCK, true);
}

AT(.mode_clock_seg)
void mode_clock_loop(void)
{
    uint32_t event;
    while(scene_mode.sta == MODE_CLOCK) {
        event = event_get(true);
        clock_event_process(event);
    }
}

AT(.mode_clock_seg)
void mode_clock_exit(void)
{
    clock_mode.sub_status = CLOCK_IDLE_MODE;
    scene_mode.last_sta = MODE_CLOCK;
    player_mode_switch_proc(MODE_CLOCK, false);
}
#endif

