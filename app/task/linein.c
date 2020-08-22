#define LOG_LEV     4
#define SLTK_LOG_TAG "linein"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "linein_audio.h"
#include "music.h"
#include "audiopath_conf.h"
#include "usb.h"
#include "audio_recorder.h"
#include "codec.h"

bool linein_muted;
int linein_play_vol;


/* 初始化ADDA（linein或FM）播放 */
AT(.mode_linein_seg)
void adda_init(uint8_t mode)
{
    switch (mode) {
    case MODE_LINEIN:
        load_digital_gain_config(AUDIO_LINEIN_MODE_INPUT_SEL);
        audio_service_set_linein_input_source(AUDIO_LINEIN_MODE_INPUT_SEL);
        break;

    case MODE_FM:
        load_digital_gain_config(AUDIO_FM_MODE_INPUT_SEL);
        audio_service_set_linein_input_source(AUDIO_FM_MODE_INPUT_SEL);
        break;

    default:
        break;
    }

    create_linein_audio();
    linein_audio_cmd(LINEIN_AUDIO_CMD_START, 0);

#if BT_BACKGROUND_EN
    vTaskPrioritySet(linein_handler, 10);
#endif
}

/* 暂停ADDA（linein或FM）播放 */
AT(.mode_linein_seg)
void adda_pp(uint8_t mode)
{
    if (COMMON_TONE_STATUS_PLAYING == np.common_tone_status) { /* common tone正在播放时调用此接口无效 */
        return;
    }

    adda_init(mode);
    linein_muted = linein_muted ? false : true;
    linein_audio_mute(linein_muted);
}

/* 退出ADDA（linein或FM）播放 */
AT(.mode_linein_seg)
void adda_deinit(uint32_t mode)
{
    destroy_linein_audio();

    switch (mode) {
    case MODE_LINEIN:
        audio_service_disable_input_source(AUDIO_LINEIN_MODE_INPUT_SEL);
        break;

    case MODE_FM:
        audio_service_disable_input_source(AUDIO_FM_MODE_INPUT_SEL);
        break;

    default:
        break;
    }
}

AT(.mode_linein_seg)
void linein_mode_scene_save(void)
{
#if RECORD_LINEIN_FUNC
    if (REC_STATUS_RECORDING == rec_context->status) {
        stop_record();
        /* 重置状态为REC_STATUS_PLAYING，
         * linein_mode_scene_recovery()恢复播放时根据此状态播放录音文件
         */
        rec_context->status = REC_STATUS_PLAYING;
    } else if (REC_STATUS_PLAYING == rec_context->status) {
        rec_context->breakpoint_time = player_get_current_play_time();
        stop_play_record_file();
        /* 重置状态为REC_STATUS_PLAYING，
         * linein_mode_scene_recovery()恢复播放时根据此状态播放录音文件
         */
        rec_context->status = REC_STATUS_PLAYING;
    }
#endif
    adda_deinit(MODE_LINEIN);
}

AT(.mode_linein_seg)
void linein_mode_scene_recovery(void)
{
#if RECORD_LINEIN_FUNC
    if (REC_STATUS_PLAYING == rec_context->status) {
        linein_record_play_rec_file(MODE_LINEIN);  //恢复播放录音文件
    } else {
        if (!linein_muted) {
            adda_init(MODE_LINEIN); //恢复播放linein
        }
    }
#else
    if (!linein_muted) {
        adda_init(MODE_LINEIN);
    }
#endif
}


AT(.mode_linein_seg)
void linein_event_process(uint32_t msg)
{
    switch(msg) {
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
#if RECORD_LINEIN_FUNC
        if (REC_STATUS_IDLE == rec_context->status) {
            logd("start rec");
            stop_play_record_file();
            start_record();              //start record
        }
#endif
        break;

    case KEY_EVENT_SU | KEY_PLAY:
#if RECORD_LINEIN_FUNC
        if (REC_STATUS_RECORDING == rec_context->status) {
            logd("stop rec");
            linein_record_play_rec_file(MODE_LINEIN);
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            linein_stop_play_rec_file(MODE_LINEIN);
        } else
#endif
        {
            adda_pp(MODE_LINEIN);                          //mute/unmute
        }
        break;

#if RECORD_LINEIN_FUNC
    case AR_EVENT_ERROR:
        logd("record error event");
        stop_record();
#if SD_EN
        if (device_get_num() == DEV_SDCARD) {
            sd_detect_set(1);
        }
#endif
        if (SLTK_STYPE_FLASH_RECORD == rec_context->streamtype) {
            linein_record_play_rec_file(MODE_LINEIN);
        }
        break;

#if SD_EN
    case DEVICE_EVENT_SD_IN:
    case DEVICE_EVENT_SD_OUT:
        linein_record_proc_sd_event(MODE_LINEIN, msg);
        break;
#endif //SD_EN

#if USB_DISK_EN
    case DEVICE_EVENT_UDISK_IN:
    case DEVICE_EVENT_UDISK_OUT:
        linein_record_proc_udisk_event(MODE_LINEIN, msg);
        break;
#endif //USB_DISK_EN

#endif //RECORD_LINEIN_FUNC

    case KEY_EVENT_SU | KEY_VOL_DOWN:           //volume down
    case KEY_EVENT_H | KEY_VOL_DOWN:
    case KEY_EVENT_H | KEY_PREV_VOLDOWN:
        logi("vol-");
 #if RECORD_LINEIN_FUNC
        if (REC_STATUS_PLAYING == rec_context->status && RECORD_LINEIN_FUNC == 3){
            linein_play_vol -= 10;
            if (linein_play_vol <= 0){
                linein_play_vol = 0;
               audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最小音量提示音 */
            }
            audio_service_set_playback_volume(linein_play_vol);
        } else
 #endif
        {
            volume_down();
            disp_menu(MENU_VOLUME, np.volume, 0);
        }
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:             //volume up
    case KEY_EVENT_H | KEY_VOL_UP:
    case KEY_EVENT_H | KEY_NEXT_VOLUP:
        logi("vol+");
#if RECORD_LINEIN_FUNC
        if (REC_STATUS_PLAYING == rec_context->status && RECORD_LINEIN_FUNC == 3){
            linein_play_vol += 10;
            if (linein_play_vol >= 100){
                linein_play_vol = 100;
                audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最大音量提示音 */
            }
            audio_service_set_playback_volume(linein_play_vol);
        } else
 #endif
        {
            volume_up();
            disp_menu(MENU_VOLUME, np.volume, 0);
        }
        break;

#if LINEIN_DETECT_EN
    case DEVICE_EVENT_LINE_OUT:
        mode_switch_auto();
        break;
#endif

    case KEY_EVENT_SU | KEY_EQ:                 //eq switch
        logi("eq");
        set_eq();
        break;

#if DISP_EN
#if RECORD_LINEIN_FUNC
    case DISP_EVENT_UPDATE:
        record_linein_update_disp_rec_time();
        break;
#endif
#endif

    default:
       common_event_process(msg);
       break;
    }
}

AT(.mode_linein_seg)
void mode_linein(void)
{
    logi("linein mode.");
    mode_linein_enter();
    mode_linein_loop();
    mode_linein_exit();

}

AT(.mode_linein_seg)
void mode_linein_enter(void)
{
#if LINEIN_DETECT_EN
    if (!dev.line_sta) {
        mode_switch_auto();
        scene_mode.exit_flag = true;
        logi("no line,exit");
        return;
    }
    scene_mode.exit_flag = false;
#endif

    disp_menu(MENU_SCENE_MAIN, scene_mode.sta, 0);
#if (TONE_EN && TONE_LINEIN_MODE_EN)
    play_tone(TONE_LINEIN_MODE, true);
#endif

    player_mode_switch_proc(MODE_LINEIN, true); /* 初始化linein播放 */

#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_enter();
#endif

#if RECORD_LINEIN_FUNC
    linein_record_init(RECORD_LINEIN_FUNC);
#endif
}

AT(.mode_linein_seg)
void mode_linein_loop(void)
{
    uint32_t msg;
    while(scene_mode.sta == MODE_LINEIN) {
        msg = event_get(true);
        linein_event_process(msg);
    }
}

AT(.mode_linein_seg)
void mode_linein_exit(void)
{
#if LINEIN_DETECT_EN
    if (scene_mode.exit_flag) {
        return;
    }
#endif

    scene_mode.last_sta = MODE_LINEIN;

#if RECORD_LINEIN_FUNC
    linein_record_deinit();
#endif

    player_mode_switch_proc(MODE_LINEIN, false); /* 退出linein播放 */
#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_exit();
#endif
}

#if LINEIN_DETECT_EN
AT(.mode_linein_seg)
void linein_detect_init(void)
{
    dev.line_sta = false;
    pin_set_func(LINEIN_DETECT_PIN_NUM, PIN_FUNCINPUT);
    pin_set_pull(LINEIN_DETECT_PIN_NUM, PIN_PULL_DOWN100K);
}

AT(.app_sram_seg)
void linein_detect(void)
{
    static uint8_t line_in_cnt = 0;
    static uint8_t line_out_cnt = 0;

    if (PIN_FUNCINPUT == pin_get_func(LINEIN_DETECT_PIN_NUM)) {
        if (!pin_get_value(LINEIN_DETECT_PIN_NUM)) {
            line_in_cnt = 0;
            if (line_out_cnt < LINE_OUT_DET_CNT) {
                line_out_cnt++;
                if (line_out_cnt == LINE_OUT_DET_CNT) {
                    line_out_cnt = LINE_OUT_DET_CNT + 1;
                    if (dev.line_sta == true) {
                        dev.line_sta = false;
                        if (dev.startup) {
                         //   event_put(DEVICE_EVENT_LINE_OUT);
                        }
                        snd_spk_unmute();
                        logi("line out.");
                    }
                }
            }
        } else {
            line_out_cnt = 0;
            if (line_in_cnt < LINE_IN_DET_CNT) {
                line_in_cnt++;
                if (line_in_cnt == LINE_IN_DET_CNT) {
                    line_in_cnt = LINE_IN_DET_CNT + 1;
                    if (dev.line_sta == false) {
					snd_spk_mute();
                        dev.line_sta = true;
                        if (dev.startup) {
                           // event_put(DEVICE_EVENT_LINE_IN);
                        }
                        logi("line in.");
                    }
                }
            }
        }
    }
}

bool linein_detect_state(void)
{
	return dev.line_sta;
}


#endif

