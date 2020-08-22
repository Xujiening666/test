#define LOG_LEV     4
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "twi.h"
#include "codec.h"
#include "audiopath_conf.h"
#include "audio_recorder.h"


fm_mode_t fm_mode;
int fm_play_vol;


AT(.mode_fm_seg)
void fm_audio_init(void)
{
#if FM_TO_CODEC
    adda_init(MODE_FM);
#else
    /* 硬件直连方式只需解mute功放 */
    snd_spk_unmute();
#endif
}

AT(.mode_fm_seg)
void fm_audio_deinit(void)
{
#if FM_TO_CODEC
    adda_deinit(MODE_FM);
#endif
}

AT(.mode_fm_seg)
void fm_mode_scene_save(void)
{
#if RECORD_FM_FUNC
    if (REC_STATUS_RECORDING == rec_context->status) {
        stop_record();
        /* 重置状态为REC_STATUS_PLAYING，
         * fm_mode_scene_recovery()恢复播放时根据此状态播放录音文件
         */
        rec_context->status = REC_STATUS_PLAYING;
    } else if (REC_STATUS_PLAYING == rec_context->status) {
        rec_context->breakpoint_time = player_get_current_play_time();
        stop_play_record_file();
        /* 重置状态为REC_STATUS_PLAYING，
         * fm_mode_scene_recovery()恢复播放时根据此状态播放录音文件
         */
        rec_context->status = REC_STATUS_PLAYING;
    }
#endif
    fm_audio_deinit();
}

AT(.mode_fm_seg)
void fm_mode_scene_recovery(void)
{
#if RECORD_FM_FUNC
    if (REC_STATUS_PLAYING == rec_context->status) {
        linein_record_play_rec_file(MODE_FM);  //恢复播放录音文件
    } else {
        fm_audio_init(); //恢复播放linein
    }
#else
    fm_audio_init();
#endif
}

AT(.mode_fm_seg)
void fm_info_save(void)
{
    uint8_t freq;
    freq = fm_mode.freq - FM_FREQ_MIN;
    info_set(INFO_FM_FREQ, (void *)&freq, 1);
    info_set(INFO_FM_TOTAL, (void *)&fm_mode.ch_total, 1);
    info_set(INFO_FM_CH_CUR, (void *)&fm_mode.ch_cur, 1);
    info_set(INFO_FM_CH, (void *)&fm_mode.ch[0], FM_CH_MAX);
#if (!BT_BACKGROUND_EN)
    info_write_page();
#endif
}

AT(.mode_fm_seg)
void fm_get_freq(uint8_t direct)
{
    if (direct) {
        fm_mode.freq += FM_FREQ_STEP;
    } else {
        fm_mode.freq -= FM_FREQ_STEP;
    }

    if (fm_mode.freq < FM_FREQ_MIN ) {
        fm_mode.freq = FM_FREQ_MAX;
    } else if(fm_mode.freq > FM_FREQ_MAX ) {
        fm_mode.freq = FM_FREQ_MIN;
    }
}

AT(.mode_fm_seg)
void fm_seek_semiauto(uint8_t direct)
{
    uint16_t start;

    start = fm_mode.freq;
    while(1) {
        fm_get_freq(direct);

        if (start == fm_mode.freq) {
            break;
        }

        disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
        if (fm_seek(fm_mode.freq)) {
            break;
        }
    }
    fm_info_save();

    logi("freq:%d", fm_mode.freq);
}

AT(.mode_fm_seg)
void fm_set_channel(uint8_t direct)
{
    if (fm_mode.ch_total == 0) {
        return;
    }

    if (direct) {
        fm_mode.ch_cur++;
    } else {
        fm_mode.ch_cur--;
    }
    if (fm_mode.ch_cur > fm_mode.ch_total) {
        fm_mode.ch_cur = 1;
    } else if (fm_mode.ch_cur == 0) {
        fm_mode.ch_cur = fm_mode.ch_total;
    }
    fm_mode.freq = fm_mode.ch[fm_mode.ch_cur-1] + FM_FREQ_MIN;
    logi("ch:%d, freq:%d", fm_mode.ch_cur,fm_mode.freq);
    disp_menu(MENU_FM_CHANNEL, fm_mode.ch_cur, 0);

    fm_tune2ch(fm_mode.freq);

    fm_info_save();
}

AT(.mode_fm_seg)
void fm_seek_auto(void)
{
    uint16_t start;
    uint8_t n;

    fm_mode.freq = FM_FREQ_MIN;
    fm_mode.ch_total = 0;
    fm_mode.ch_cur = 0;

    start = fm_mode.freq;
    n = 0;
    logi("seek start:");
    while(1) {
        printf_debug(".");
        disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
        fm_set_vol(0);
        if (fm_seek(fm_mode.freq)) {
            n++;
            if (n > FM_CH_MAX) {
                break;
            }
            fm_mode.ch[n-1] = fm_mode.freq - FM_FREQ_MIN;

            fm_tune2ch(fm_mode.freq);
            fm_set_vol(FM_VOLUME);
            logi("\n(%d):%d", n,fm_mode.freq);
            disp_menu(MENU_FM_CHANNEL, n, 0);
            mdelay(1000);
            fm_set_vol(0);
        }

        fm_get_freq(1);
        if (start == fm_mode.freq) {
            break;
        }

        if (event_clear(KEY_EVENT_SU | KEY_PLAY)) {
            break;
        }
    }
    fm_mode.ch_total = n;
    logi("seek end,total:%d", fm_mode.ch_total);

    fm_mode.ch_cur = 1;
    fm_mode.freq = fm_mode.ch[fm_mode.ch_cur-1] + FM_FREQ_MIN;
    disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
    fm_tune2ch(fm_mode.freq);
    fm_set_vol(FM_VOLUME);
    logi("cur freq:%d", fm_mode.freq);

    fm_info_save();
}

AT(.mode_fm_seg)
void fm_event_process(uint32_t event)
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
#if RECORD_FM_FUNC
        if (REC_STATUS_IDLE == rec_context->status) {
            logd("start rec");
            stop_play_record_file();
            start_record();              //start record
        }
#endif
        break;

    case KEY_EVENT_SU | KEY_PLAY:
#if RECORD_FM_FUNC
        if (REC_STATUS_RECORDING == rec_context->status) {
            logd("stop rec");
            linein_record_play_rec_file(MODE_FM);
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            linein_stop_play_rec_file(MODE_FM);
        } else
#endif
        {
            fm_seek_auto();
        }
        break;

#if RECORD_FM_FUNC
    case AR_EVENT_ERROR:
        logd("record error event");
        stop_record();
#if SD_EN
        if (device_get_num() == DEV_SDCARD) {
            sd_detect_set(1);
        }
#endif
        if (SLTK_STYPE_FLASH_RECORD == rec_context->streamtype) {
            linein_record_play_rec_file(MODE_FM);
        }
        break;

#if SD_EN
    case DEVICE_EVENT_SD_IN:
    case DEVICE_EVENT_SD_OUT:
        linein_record_proc_sd_event(MODE_FM, event);
        break;
#endif //SD_EN

#if USB_DISK_EN
    case DEVICE_EVENT_UDISK_IN:
    case DEVICE_EVENT_UDISK_OUT:
        linein_record_proc_udisk_event(MODE_FM, event);
        break;
#endif //USB_DISK_EN
#endif //RECORD_FM_FUNC

    case KEY_EVENT_SU | KEY_VOL_DOWN:   //volume down
    case KEY_EVENT_H | KEY_VOL_DOWN:
    case KEY_EVENT_H | KEY_PREV_VOLDOWN:
        logi("vol-");
 #if RECORD_FM_FUNC
        if (REC_STATUS_PLAYING == rec_context->status && RECORD_FM_FUNC == 3){
            fm_play_vol--;
            if (fm_play_vol <= 0){
                fm_play_vol = 0;
               audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最小音量提示音 */
            }
            audio_service_set_playback_volume(fm_play_vol);
        } else
 #endif
        {
            volume_down();
            disp_menu(MENU_VOLUME, np.volume, 0);
        }
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:     //volume up
    case KEY_EVENT_H | KEY_VOL_UP:
    case KEY_EVENT_H | KEY_NEXT_VOLUP:
        logi("vol+");
#if RECORD_FM_FUNC
        if (REC_STATUS_PLAYING == rec_context->status && RECORD_FM_FUNC == 3){
            fm_play_vol++;
            if (fm_play_vol >= 100){
                fm_play_vol = 100;
                audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最大音量提示音 */
            }
            audio_service_set_playback_volume(fm_play_vol);
        } else
 #endif
        {
            volume_up();
            disp_menu(MENU_VOLUME, np.volume, 0);
        }
        break;

    case KEY_EVENT_SU | KEY_PREV:
    case KEY_EVENT_SU | KEY_PREV_VOLDOWN:
        fm_set_channel(0);
        break;

    case KEY_EVENT_SU | KEY_NEXT:
    case KEY_EVENT_SU | KEY_NEXT_VOLUP:
        fm_set_channel(1);
        break;
#if 0
    case KEY_EVENT_L | KEY_PREV:
        fm_seek_semiauto(0);
        break;

    case KEY_EVENT_L | KEY_NEXT:
        fm_seek_semiauto(1);
        break;
#endif

    case KEY_EVENT_SU | KEY_CH_DOWN:
        fm_get_freq(0);
        disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
        fm_tune2ch(fm_mode.freq);
        logi("freq:%d", fm_mode.freq);
        break;

    case KEY_EVENT_SU | KEY_CH_UP:
        fm_get_freq(1);
        disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
        fm_tune2ch(fm_mode.freq);
        logi("freq:%d", fm_mode.freq);
        break;

#if DISP_EN
    case DISP_EVENT_SWITCH:
        if (disp.menu != MENU_SCENE_MAIN) {
            disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
        }
        break;
#endif

#if DISP_EN
#if RECORD_FM_FUNC
    case DISP_EVENT_UPDATE:
        record_linein_update_disp_rec_time();
        break;
#endif
#endif

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_fm_seg)
void mode_fm(void)
{
    logi("fm mode.");

    mode_fm_enter();
    mode_fm_loop();
    mode_fm_exit();
}

AT(.mode_fm_seg)
void mode_fm_enter(void)
{
    sleep_prevent_set(PS_APP_PROCESS);

    twi_init();
    fm_get_id();
    if (fm_mode.id == FM_ID_NONE) {
        mode_switch_auto();
        logi("fm not online");
        return;
    }

#if (TONE_EN && TONE_FM_MODE_EN)
    play_tone(TONE_FM_MODE, true);
#endif

    disp_menu(MENU_FM_FREQ, fm_mode.freq, 0);
    disp_state(STATE_FM_PLAY);

    fm_init();
    logi("freq:%d", fm_mode.freq);
    fm_tune2ch(fm_mode.freq);
    fm_set_vol(FM_VOLUME);

    player_mode_switch_proc(MODE_FM, true); /* 初始化FM播放 */

#if RECORD_FM_FUNC
    linein_record_init(RECORD_FM_FUNC);
#endif
}

AT(.mode_fm_seg)
void mode_fm_loop(void)
{
    uint32_t msg;
    while(scene_mode.sta == MODE_FM) {
        msg = event_get(true);
        fm_event_process(msg);
    }
}

AT(.mode_fm_seg)
void mode_fm_exit(void)
{
    scene_mode.last_sta = MODE_FM;

#if RECORD_FM_FUNC
    linein_record_deinit();
#endif

    player_mode_switch_proc(MODE_FM, false); /* 退出FM播放 */

    fm_deinit();
    twi_deinit();

    sleep_prevent_clear(PS_APP_PROCESS);

    logi("fm exit.");
}


