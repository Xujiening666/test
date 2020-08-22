#define LOG_LEV     5
#define SLTK_LOG_TAG "spdif mode"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "spdifaudio.h"
#include "spdif.h"
#include "spdif_mode.h"
#include "music.h"
#include "usb.h"
#include "task_main.h"


#if MODE_SPDIF_EN

bool spdif_muted;

AT(.mode_spdif_seg)
void spdif_play_init(void)
{
#if SPDIF_EN
    spdif_init();
    create_spdifaudio(spdif_muted);
#endif
}

AT(.mode_spdif_seg)
void spdif_play_deinit(void)
{
    destroy_spdifaudio();
    spdif_deinit();
}

AT(.mode_spdif_seg)
void spdif_play_mute_or_unmute(void)
{
    if (spdifaudio_is_muted()) {
        logi("unmute");
        spdifaudio_mute(0);
        spdif_muted = false;
    } else {
        logi("mute");
        spdifaudio_mute(1);

        spdif_muted = true;
    }
}

AT(.mode_spdif_seg)
void spdif_event_process(uint32_t event)
{
    switch (event) {
    case PLAYER_EVENT_TONE_END_OF_FILE:
        if (common_tone_is_playing()) {
            uint32_t trigger_event = get_common_tone_trigger_event();
            common_tone_status_clear();
            switch (trigger_event) {
            default:
                audio_scene_recovery(); //恢复之前的播放
                break;
            }
        }
        break;

    case KEY_EVENT_L | KEY_PLAY:
        break;

    case KEY_EVENT_SU | KEY_PLAY:
        logi("p/p");
        spdif_play_mute_or_unmute();
        break;

    case KEY_EVENT_SU | KEY_VOL_DOWN:   //volume down
    case KEY_EVENT_H | KEY_VOL_DOWN:
    case KEY_EVENT_H | KEY_PREV_VOLDOWN:
        logi("vol-");
        volume_down();
        disp_menu(MENU_VOLUME, np.volume, 0);
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:     //volume up
    case KEY_EVENT_H | KEY_VOL_UP:
    case KEY_EVENT_H | KEY_NEXT_VOLUP:
        logi("vol+");
        volume_up();
        disp_menu(MENU_VOLUME, np.volume, 0);
        break;

    case KEY_EVENT_SU | KEY_PREV:
    case KEY_EVENT_SU | KEY_PREV_VOLDOWN:
        logi("prev");
        break;

    case KEY_EVENT_SU | KEY_NEXT:
    case KEY_EVENT_SU | KEY_NEXT_VOLUP:
        logi("next");
        break;

    case KEY_EVENT_SU | KEY_CH_DOWN:
        break;

    case KEY_EVENT_SU | KEY_CH_UP:
        break;

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_spdif_seg)
void mode_spdif(void)
{
    logi("spdif mode.");

    mode_spdif_enter();
    mode_spdif_loop();
    mode_spdif_exit();
}

AT(.mode_spdif_seg)
void mode_spdif_enter(void)
{
    sleep_prevent_set(PS_APP_PROCESS);

    disp_menu(MENU_SCENE_MAIN, scene_mode.sta, 0);

//#if (TONE_EN && TONE_FM_MODE_EN)
    //play_tone(TONE_FM_MODE, true); //FIXME
//#endif

    spdif_muted = false;

    player_mode_switch_proc(MODE_SPDIF, true);

#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_enter();
#endif
}

AT(.mode_spdif_seg)
void mode_spdif_loop(void)
{
    uint32_t msg;
    while(scene_mode.sta == MODE_SPDIF) {
        msg = event_get(true);
        spdif_event_process(msg);
    }
}

AT(.mode_spdif_seg)
void mode_spdif_exit(void)
{
    scene_mode.last_sta = MODE_SPDIF;

    player_mode_switch_proc(MODE_SPDIF, false);

#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_exit();
#endif

    sleep_prevent_clear(PS_APP_PROCESS);
    logi("spdif exit.");
}

#endif
