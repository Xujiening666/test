//#define LOG_LEV 5
#define SLTK_LOG_TAG "player"
#include <stdlib.h>
#include "sys_inc.h"
#include "user_config.h"
#include "device.h"
#include "player.h"
#include "record.h"
#include "tone.h"
#include "event.h"
#include "info_mem.h"
#include "libc_common.h"
#include "audio_service.h"
#include "audiopath_conf.h"
#include "trng.h"
#include "sdmmc_io.h"
#include "codec.h"
#include "i2s.h"
#include "hal_dma_lib.h"
#include "audio_effects.h"
#include "Bt_host_api.h"
#include "update.h"
#include "audio_out.h"
#include "eq_process.h"
#include "app_afx_config.h"
#include "timer.h"

#if (0 == MUSIC_WAV)
const void *get_wav_parser_plugin(void){return NULL;}
const void *get_pcm_decoder_plugin(void){return NULL;}
#endif

#if (0 == MUSIC_MP3)
const void *get_mp3_parser_plugin(void){return NULL;}
const void *get_mp3_decoder_plugin(void){return NULL;}
#endif

#if ((0 == MUSIC_WMA) )
const void *get_wma_parser_plugin(void){return NULL;}
const void *get_wma_decoder_plugin(void){return NULL;}
#endif

#if ((0 == MUSIC_AAC) || AUDIO_PLAYBACK_VSS )
const void *get_aac_parser_plugin(void){return NULL;}
const void *get_m4a_parser_plugin(void){return NULL;}
const void *get_aac_decoder_plugin(void){return NULL;}
#endif

#if ((0 == MUSIC_APE) )
const void *get_ape_parser_plugin(void){return NULL;}
const void *get_ape_decoder_plugin(void){return NULL;}
#endif

#if ((0 == MUSIC_FLAC) || AUDIO_PLAYBACK_VSS )
const void *get_flac_parser_plugin(void){return NULL;}
const void *get_flac_decoder_plugin(void){return NULL;}
#endif

#if ((0 == MUSIC_OGG) || AUDIO_PLAYBACK_VSS )
const void *get_ogg_parser_plugin(void){return NULL;}
const void *get_ogg_decoder_plugin(void){return NULL;}
#endif

#if ((0 == MUSIC_SBC) && (2 != RECORD_FORMAT_SEL))
const void *get_sbc_parser_plugin(void){return NULL;}
const void *get_sbc_decoder_plugin(void){return NULL;}
#endif

struct native_player np;

#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC)
/* Play all dir, except "record" */
const dir_list_item_t dir_list0[] = {
    {.type = DIR_TYPE_BLACKLIST, .name.ascii = RECORD_FILE_DIR},  /* 搜索排除 RECORD_FILE_DIR 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};
#endif

#if 0
//"儿歌"的unicode; '儿' unicode是0x513F， '歌'的unicode是0x6B4C, '\0'的unicode是0x0000
const uint16_t song_unicode[] = {0x513F, 0x6B4C, 0x0000};
// "故事"的unicode; '故' unicode是0x6545， '事'的unicode是0x4E8B, '\0'的unicode是0x0000
const uint16_t story_unicode[] = {0x6545, 0x4E8B, 0x0000};
/* only play song dir */
const dir_list_item_t dir_song[] = {
    {.type = DIR_TYPE_UNICODE_WHITELIST, .name.unicode = song_unicode}, /* 只搜索 "儿歌" 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};

/* only play story dir */
const dir_list_item_t dir_story[] = {
    {.type = DIR_TYPE_UNICODE_WHITELIST, .name.unicode = story_unicode}, /* 只搜索 "故事" 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};
#endif

#if 0
/* only play dir0 dir */
const dir_list_item_t dir_list3[] = {
    {.type = DIR_TYPE_WHITELIST, .name.ascii = "dir0"}, /* 只搜索 "dir0" 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};
#endif


/*
 * 文件后缀名白名单；只有白名单文件可以被检索
 */
const char *file_suffix_wl[] = {
#if MUSIC_WAV
    ".wav",
#endif

#if MUSIC_MP3
    ".mp3",
    ".mp2",
#endif

#if MUSIC_SBC
    ".sbc",
#endif

//#if (!AUDIO_KARAOKE_MODE)
#if (MUSIC_OGG && (0 == AUDIO_PLAYBACK_VSS))
    ".ogg",
#endif
#if MUSIC_WMA
    ".wma",
#endif

#if MUSIC_APE
    ".ape",
#endif

#if (MUSIC_AAC && (0 == AUDIO_PLAYBACK_VSS))
    ".aac",
    ".m4a",
#endif

#if (MUSIC_FLAC && (0 == AUDIO_PLAYBACK_VSS))
    ".flac",
#endif
//#endif

     NULL
};

//AT(.com_seg)
void auto_set_cpux_freq(void)
{
    uint32_t focus;
    uint32_t features;
    uint32_t freq = (uint32_t)32e6;
    uint32_t cpu1_div = 1;

    /*if (AUDIO_KARAOKE_MODE || AUDIO_ADJUST_HPLR_LINEOUTLR) {
        //do not set cpu freq for karaoke
        return;
    }*/

    features = audio_service_get_features();
    focus = get_audiofocus_holder();

    switch (focus) {
    case AUDIOFOCUS_HOLDER_AUDIOPLAYER: //local play or tone play
        if (features & AUDIO_FEATURE_PLAYBACK_EFFECTS) {
            freq = DVFS_EFFECTS_FREQ;
        } else {
            if (features & AUDIO_FEATURE_TONEMIXER) {
                freq = DVFS_TONEMIXER_FREQ;
            } else {
                freq = DVFS_LOCAL_DECODE_WAV_FREQ;
            }
        }
        if (AUDIO_DECODER_TYPE_WAV != audio_service_get_decoder_type()) {
            freq = DVFS_LOCAL_DECODE_MP3_FREQ;
        }
        break;

    case AUDIOFOCUS_HOLDER_A2DPAUDIO: //bt music
        if (features & AUDIO_FEATURE_PLAYBACK_EFFECTS) {
            freq = DVFS_EFFECTS_FREQ;
        } else {
            if (features & (AUDIO_FEATURE_TONEMIXER | AUDIO_FEATURE_A2DP_FADE)) {
                freq = DVFS_TONEMIXER_FREQ;
            } else {
                freq = DVFS_A2DPAUDIO_FREQ;
            }
        }
        break;

    case AUDIOFOCUS_HOLDER_SCOAUDIO: //bt phone
        freq = DVFS_SCOAUDIO_FREQ;
        if (features & AUDIO_FEATURE_SPEAKER_AENC) {
            freq = DVFS_SCOAENC_FREQ;
        }
        if (features & AUDIO_FEATURE_SCO_TSPS) {
            freq = AUDIO_AFX_SCO_TSPS_CPUFRQ;
        }
        break;

    case AUDIOFOCUS_HOLDER_USBAUDIO: //usb audio
        if (features & AUDIO_FEATURE_PLAYBACK_EFFECTS) {
            freq = DVFS_EFFECTS_FREQ;
        } else {
            freq = DVFS_USBAUDIO_FREQ;
        }
        break;

    case AUDIOFOCUS_HOLDER_LINEINAUDIO: //linein or fm play
        if (features & AUDIO_FEATURE_PLAYBACK_EFFECTS) {
            freq = DVFS_EFFECTS_FREQ;
        } else {
            freq = DVFS_LINEINAUDIO_FREQ;
        }
        break;

    case AUDIOFOCUS_HOLDER_SPDIFAUDIO: //spdif audio
        if (features & AUDIO_FEATURE_PLAYBACK_EFFECTS) {
            freq = DVFS_EFFECTS_FREQ;
        } else {
            freq = DVFS_DEFAULT_FREQ;
        }
        break;

    case AUDIOFOCUS_HOLDER_NONE:
        freq = DVFS_DEFAULT_FREQ;
        break;

    default:
        freq = DVFS_DEFAULT_FREQ;
        break;
    }

    if (features & AUDIO_FEATURE_RECORD_ENCODER) {
        uint32_t enc_type = audio_service_get_encoder_type();
        switch (enc_type) {
        case AUDIO_ENCODER_TYPE_WAV:
            freq = (freq < DVFS_LOCAL_ENCODE_WAV_FREQ) ? DVFS_LOCAL_ENCODE_WAV_FREQ : freq;
            break;
        case AUDIO_ENCODER_TYPE_MP2:
        case AUDIO_ENCODER_TYPE_MP3:
            freq = (freq < DVFS_LOCAL_ENCODE_MP3_FREQ) ? DVFS_LOCAL_ENCODE_MP3_FREQ : freq;
            break;
        default:
            break;
        }
    }

    if(scene_mode.sta == MODE_USBDEV){
        freq = DVFS_USBAUDIO_FREQ;
    }

    clk_set_cpux_freq(freq, cpu1_div);

    //logd("[features:%#x, focus:%#x]: auto set cpux freq:%d, cpu1_div:%d",
    //    features, focus, freq, cpu1_div);
}

AT(.com_seg)
bool app_need_fast_exit_scan(void)
{
    if (key_mode) {
        key_mode = false;
        key_mode_exit = true;
        return true;
    }

    return false;
}

AT(.player_seg)
int file_manager_init(struct file_manager *m, char *root)
{
    /* 设置文件后缀名白名单,仅白名单的文件被检索 */
    xfs_cmd(XFS_CMD_CONFIG_FILE_SUFFIX, (uint32_t)file_suffix_wl);


    /* 设置文件夹黑白名单
     * NOTE：如果设置了白名单，则仅白名单文件夹被检索；
     *       如果设置了黑名单， 则除黑名单外的所有文件夹都被检索；
     *       如果同时设置白名单和黑名单，则黑名单设置无效。
     */
    //xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_song);  /* 只搜索"儿歌"文件夹 */
    //xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_story); /* 只搜索"故事"文件夹 */
    //xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_list3); /* 只搜索"dir0"文件夹 */
#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC)
#if MUSIC_REC_FOLDER_IGNORE
    xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_list0); /* 不播放录音文件夹 */
#endif
#endif

    sleep_delay_set();
    sleep_prevent_set(PS_APP_PROCESS);

    key_mode = false;
    key_mode_exit = false;
    if (device_get_num() == DEV_SDCARD) {
        m->sdcard_total_file = xfscan();
        m->total_file = m->sdcard_total_file;
    } else if (device_get_num() == DEV_UDISK) {
        m->udisk_total_file = xfscan();
        m->total_file = m->udisk_total_file;
    }

    sleep_delay_set();
    sleep_prevent_clear(PS_APP_PROCESS);

    m->limit_min_num = 1;
    m->limit_max_num = m->total_file;

    xfs_get_total_num(&np.total_dir_num, (uint32_t*)&m->total_file);
    logi("total %d dir, %d file", np.total_dir_num, m->total_file);
    return FMG_OK;
}

/* metadata（ID3）保存函数
 * 此函数由audio库调用； 播放器调用此函数以key-value键值对方式给出 metadata信息；
 * @param ctype: 解码类型
 * @param key: metadata key值
 * @param val: metadata value值
 */
AT(.player_seg)
void metadata_save(uint32_t ctype,
    const char *key, const char *val)
{
    //TODO: save id3
}


/* 文件名保存函数
 * 此函数由audio库调用； 播放器获取到文件编号对应的文件名时调用此函数给出文件名
 * @param name: 文件名
 */
AT(.player_seg)
void upload_media_name(const char* name)
{
    //TODO:save file name
}

AT(.player_seg)
int file_manager_deinit(struct file_manager *m)
{
    return FMG_OK;
}

void startup_audio_service(void)
{
    audio_service_init_params_t params;

    dma_init();

    codec_user_config();

    /* 关闭所有输入源 */
    audio_service_disable_input_source(AUDIO_INPUT_DMIC0F_R);
    audio_service_disable_input_source(AUDIO_INPUT_AMIC0);
    audio_service_disable_input_source(AUDIO_INPUT_AMIC1_2);
    audio_service_disable_input_source(AUDIO_INPUT_LINEIN0_1);

#if AUDIO_KARAOKE_MODE
    clk_set_cpux_freq(DVFS_KARAOKE_FREQ, 1);
#endif

    int awf_plbk_num = getPlbkAWFNum();
    int awf_rec_num  = getRecAWFNum();

    params.music_features = 0;
    params.music_features |= MUSIC_METADATA ? MUSIC_FEATURE_METADATA : 0;
    params.music_features |= MUSIC_WAV ? MUSIC_FEATURE_WAV : 0;
    params.music_features |= MUSIC_MP3 ? MUSIC_FEATURE_MP3 : 0;
    params.music_features |= MUSIC_WMA ? MUSIC_FEATURE_WMA : 0;
    params.music_features |= MUSIC_FLAC ? MUSIC_FEATURE_FLAC : 0;
    params.music_features |= MUSIC_AAC ? MUSIC_FEATURE_AAC : 0;
    params.music_features |= MUSIC_APE ? MUSIC_FEATURE_APE : 0;
    params.music_features |= MUSIC_OGG ? MUSIC_FEATURE_OGG : 0;
    params.karaoke_mode = AUDIO_KARAOKE_MODE;
    params.spiflash_sel = 0;
    params.spiflash_sel |= SPI_FLASH_AUXTRACK_SEL << SPIFLASH_AUXTRACK_NUM;
    params.spiflash_sel |= SPI_FLASH_TONE_SEL << SPIFLASH_TONE_NUM;

    audio_service_init(&params
        , awf_rec, awf_rec_num, awf_plbk, awf_plbk_num
    );

    eq_load_internal();

#if AUDIO_ADJUST_HPLR_LINEOUTLR
    extern void lineout_hp_gain_init(void);
    lineout_hp_gain_init();
    extern void anc_task_init(void);
    anc_task_init();
#endif
}

/* 播放事件监听器
 * 此函数被audio库解码线程调用，
 * @param player: 播放器句柄
 * @param event: 播放事件， 参考@player_event_t
 */
AT(.player_seg)
void player_event_listener(void *player, uint32_t event)
{
    uint32_t type;

    if (PLAYER_EVENT_NONE == event) {
        return;
    }

    switch (event) {
    case PLAYER_EVENT_ERR:
        type = player_get_error_type();
        logw("play error (type=%d)", type);
        switch (type) {
        case APERR_FORMAT_PROBE:
        case APERR_CREATE_PARSER:
        case APERR_OPEN_INVALID_PARAMS:
        case APERR_CREATE_DEC:
        case APERR_OPEN_DEC:
        case APERR_OPEN_TRACK:
            #if MODE_MUSIC_EN
            if (np.is_music_mode) {
                if (device_get_num() == DEV_SDCARD) {
                    if (DEV_OK == device_get_sta(DEV_SDCARD)) {
                        ++(np.manager.sdcard_file_fail_count);
                    } else {
                        np.manager.sdcard_file_fail_count = 0;
                    }
                } else if (device_get_num() == DEV_UDISK) {
                    if (DEV_OK == device_get_sta(DEV_UDISK)) {
                        ++(np.manager.udisk_file_fail_count);
                    } else {
                        np.manager.udisk_file_fail_count = 0;
                    }
                }
            }
            #endif
            break;

        case APERR_FRAME_DEC:
            #if MODE_MUSIC_EN
            /* 设备插拔播放断点保存 */
            if (np.is_music_mode) {
                get_play_time();
                play_time_write();
                player_dump_seek_helper();
            }
            #endif
            break;

        default:
            break;
        }
        event_force_put(event); /* 发送事件给app线程 */
        break;

    case PLAYER_EVENT_OPEN_COMPLETED:
        #if MODE_MUSIC_EN
        if (np.is_music_mode) {
            play_num_write();
            #if BT_BACKGROUND_EN
            xfsdisk_delay_for_bt_enable(FALSE);
            #endif
            if (device_get_num() == DEV_SDCARD) {
                /* SD卡中有文件可以被播放， sdcard_file_fail_count清0 */
                np.manager.sdcard_file_fail_count = 0;
            } else if (device_get_num() == DEV_UDISK) {
                /* 说明U盘中有文件可以被播放， udisk_file_fail_count清0 */
                np.manager.udisk_file_fail_count = 0;
            }
            player_bp_seek_helper();
            if (np.file_num_dirty) { //文件夹切换，需要更新文件编号
                uint32_t cur_dir, cur_file_num;
                xfs_get_current_num(&cur_dir, &cur_file_num);
                np.file_num_dirty = false;
                np.file_num = cur_file_num;
                logd("update file_num:%d", np.file_num);
            }
        }
        #elif MODE_RECORD_EN
        if (MODE_RECORD == scene_mode.sta) {
            rec_context->file_fail_count = 0;
        }
        #endif
        break;

    case PLAYER_EVENT_END_OF_FILE:
        event_force_put(event); /* 发送事件给app线程 */
        break;

    case PLAYER_EVENT_PAUSE_COMPLETED:
#if SD_EN
        if (DEV_SDCARD == device_get_num()) {
            while(disk_is_lock()) {
                if (RES_OK != sd_get_status()) {
                    break;
                }
                mdelay(1);
            }
            sd_detect_set(1);
        }
        /* 由于在DEC_TASK线程中有使用sd_detect_set，以上内容不可以在app线程中处理 */
#endif
        event_force_put(event); /* 发送事件给app线程 */
        break;

    case PLAYER_EVENT_STOP_COMPLETED:
        #if MODE_MUSIC_EN
        if (np.is_music_mode) {
            if (np.breakpoint_wr == 1) {
                get_play_time();
                play_time_write();
                player_dump_seek_helper();
            } else if (np.breakpoint_wr == 2) {
                np.ptime.ms = 0;
                play_time_write();
            }
        }
        #endif
        break;

    case PLAYER_EVENT_TONE_END_OF_FILE:
        switch (scene_mode.sta) {
        case MODE_MUSIC:
        case MODE_USBDEV:
        case MODE_LINEIN:
        case MODE_FM:
        case MODE_SPDIF:
        case MODE_CLOCK:
            if (COMMON_TONE_STATUS_PLAYING == np.common_tone_status) {
                event_force_put(PLAYER_EVENT_TONE_END_OF_FILE);
            }
            break;

        #if MODE_BT_EN
        case MODE_BT:
            if (bt_mode.tone.status == TONE_STATUS_FORWARD) {
                bt_audio_server_start();
                bt_mode.tone.status = TONE_STATUS_IDLE;
            } else if (bt_mode.tone.status == TONE_STATUS_BACKWARD) {
                bt_audio_server_start();
                bt_mode.tone.status = TONE_STATUS_IDLE;
            } else if (bt_mode.tone.status == TONE_STATUS_COMMON) {
                bt_audio_server_start();
                bt_mode.tone.status = TONE_STATUS_IDLE;
            }
            break;
        #endif

        default:
            break;
        }
        return;

    default:
        break;
    }
}

AT(.player_seg)
int player_init()
{
#if MODE_MUSIC_EN
    set_play_direction(PLAY_NEXT); /* 配置播放完成或者播放出错后自动播放下一首 */
#endif
    player_load_priv_data_from_flash(); /* 从flash中读取上次解码器状态数据 */
    np.mute_for_fast = MUSIC_MUTE_FOR_FAST; /* 设置快进快退时不播放音乐 */

    return 0;
}

AT(.player_seg)
void player_exit_music_mode(void)
{
    np.paused = false;
    np.fast = false;
    np.breakpoint_wr = 1;
    np.common_tone_status = COMMON_TONE_STATUS_IDLE;
    player_close(0); //立刻停止播放，然后关闭播放器
}

AT(.player_seg)
void player_deinit(void)
{
    player_close(0); //立刻停止本地播放，然后关闭播放器
}

/*
 * 等待通用提示音播放完成
 */
void player_wait_common_tone_done(void)
{
    np.common_tone_status = COMMON_TONE_STATUS_IDLE;
    np.common_tone_trigger_event = 0;

    /* 立刻停止播放，关闭播放器 */
    player_close(0);
}

/* 切换模式时，播放器处理函数；
 * 此函数完成切换模式时播放器的清除或者初始化工作；
 * @param mode: 当前模式；
 * @param enter: true:表示进入该模式； false:表示退出该模式；
 */
void player_mode_switch_proc(uint8_t mode, bool enter)
{
    switch (mode) {
#if MODE_MUSIC_EN
    case MODE_MUSIC:
        if (enter) {
            np.is_music_mode = true;
        } else {
            np.common_tone_status = COMMON_TONE_STATUS_IDLE;
            player_exit_music_mode();
            np.is_music_mode = false;
        }
        break;
#endif

#if MODE_BT_EN
    case MODE_BT:
        if (enter) {
        } else {
            np.common_tone_status = COMMON_TONE_STATUS_IDLE;
            player_close(0); //close player immediately
        }
        break;
#endif

#if MODE_USBDEV_EN && USB_AUDIO_EN
    case MODE_USBDEV:
        if (enter) {
            usbaudio_init();
        } else {
            usbaudio_deinit();
            player_wait_common_tone_done();
        }
        break;
#endif

#if MODE_LINEIN_EN
    case MODE_LINEIN:
        if (enter) {
            linein_muted = false;
            adda_init(MODE_LINEIN);
        } else {
            player_wait_common_tone_done();
            adda_deinit(MODE_LINEIN);
        }
        break;
#endif

#if MODE_FM_EN
    case MODE_FM:
        if (enter) {
            fm_audio_init();
        } else {
            fm_audio_deinit();
            player_wait_common_tone_done();
        }
        break;
#endif

#if MODE_SPDIF_EN
    case MODE_SPDIF:
        if (enter) {
            spdif_play_init();
        } else {
            spdif_play_deinit();
            player_wait_common_tone_done();
        }
        break;
#endif

#if MODE_CLOCK_EN
    case MODE_CLOCK:
        if (enter) {
        } else {
            player_wait_common_tone_done();
        }
        break;
#endif

    default:
        break;
    }
}

void audio_scene_save(void)
{
    switch (scene_mode.sta) {
#if MODE_MUSIC_EN
    case MODE_MUSIC:
        if (SLTK_STYPE_FATFS == player_get_stream_source()) {
            np.breakpoint_wr = 1;
            player_close(0);
            np.breakpoint_rd = 1;
            get_play_time();
            play_time_write();
        }
        break;
#endif

#if MODE_BT_EN
    case MODE_BT:
        bt_audio_server_stop();
        break;
#endif

#if MODE_RECORD_EN
    case MODE_RECORD:
        record_mode_scene_save();
        break;
#endif

#if MODE_USBDEV_EN
    case MODE_USBDEV:
        usbaudio_deinit();
        break;
#endif

#if MODE_LINEIN_EN
    case MODE_LINEIN:
        linein_mode_scene_save();
        break;
#endif

#if MODE_FM_EN
    case MODE_FM:
        fm_mode_scene_save();
        break;
#endif

#if MODE_SPDIF_EN
    case MODE_SPDIF:
        spdif_play_deinit();
        break;
#endif

#if MODE_CLOCK_EN
    case MODE_CLOCK:
        break;
#endif

    default:
        break;
    }
}

void audio_scene_recovery(void)
{
    switch (scene_mode.sta) {
#if MODE_MUSIC_EN
    case MODE_MUSIC:
        play_file(np.file_num, false);
        break;
#endif

#if MODE_BT_EN
    case MODE_BT:
        bt_audio_server_start();
        break;
#endif

#if MODE_RECORD_EN
    case MODE_RECORD:
        record_mode_scene_recovery();
        break;
#endif

#if MODE_USBDEV_EN
    case MODE_USBDEV:
#if USB_AUDIO_EN
        usbaudio_init();
        break;
#endif
#endif

#if MODE_LINEIN_EN
    case MODE_LINEIN:
        linein_mode_scene_recovery();
        break;
#endif

#if MODE_FM_EN
    case MODE_FM:
        fm_mode_scene_recovery();
        break;
#endif

#if MODE_SPDIF_EN
    case MODE_SPDIF:
        spdif_play_init();
        break;
#endif

#if MODE_CLOCK_EN
    case MODE_CLOCK:
        //TODO
        break;
#endif

    default:
        break;
    }
}

/* 通用提示音播放；
 * 此函数可在各个模式下接受某一个消息时播放提示音； 提示音播放完成后自动恢复先前的播放；
 * NOTE: mode提示音等标准SDK提示音使用的是play_tone, 不需要使用此接口;
 * @param tone_num: 提示音编号， 参考“tone.h"提示音编号定义；
 * @param trigger_event: 触发此提示音播放的事件（播放完成后可通过
 *                       get_common_tone_trigger_event()获取触发提示音是什么事件
 *                       根据不同事件做不同的事情
 * @return: 成功返回0； 失败返回负值错误码;
 */
int play_common_tone(uint8_t tone_num, uint32_t trigger_event)
{
    switch (scene_mode.sta) {
#if MODE_BT_EN
    case MODE_BT:
        if (TONE_STATUS_COMMON == bt_mode.tone.status ||
            TONE_STATUS_IDLE == bt_mode.tone.status) {
            bt_mode.tone.status = TONE_STATUS_COMMON;
            bt_play_tone(tone_num, false, false);
        }
        break;
#endif

    default:
        if (np.common_tone_status != COMMON_TONE_STATUS_PLAYING) {
            np.common_tone_trigger_event = trigger_event;
            np.common_tone_status = COMMON_TONE_STATUS_PLAYING;
            audio_scene_save();
            play_tone(tone_num, false);
        }
        break;
    }

    return 0;
}

uint32_t get_common_tone_trigger_event(void)
{
    return np.common_tone_trigger_event;
}

bool common_tone_is_playing(void)
{
    return (np.common_tone_status == COMMON_TONE_STATUS_PLAYING) ? true : false;
}

void common_tone_status_clear(void)
{
    np.common_tone_status = COMMON_TONE_STATUS_IDLE;
    np.common_tone_status = 0;
}

/* 播放提示音
 * @param tone_num: 提示音编号， 参考“tone.h"提示音编号定义；
 * @param block: true：阻塞直到按键切模式或者播放完成才退出此函数；
 *               false: 不等播放完成就退出此函数；
 * @return: 成功返回0； 失败返回负值错误码
 */
AT(.player_seg)
int play_tone(uint8_t tone_num, bool block)
{
    key_mode = false;

    source_info_t info;
    memset(&info, 0, sizeof(info));
    info.type = SLTK_STYPE_FLASH_TONE;
    info.num = tone_num;
    player_open(&info);
    if (block) {
        player_wait_command_processed();
        while (AP_STATUS_PLAYING == player_get_status()) {
            msleep(1);
            if (key_mode) {
                key_mode = false;
                break;
            }
        }
        player_close(0); //close player immediately
    }

    return 0;
}

/* 播放提示音链
 * 来电报号使用此函数实现
 * @param set_next_tone_name: 设置下一个提示音文件的函数；set_next_tone_name会
 * 由audio lib回调去自动获取下一个提示音进行播放
 * @return: 成功返回0； 失败返回负值错误码
 */
int play_tone_chain(int (*set_next_tone_name)(source_info_t *))
{
    source_info_t info;

    memset(&info, 0, sizeof(info));
    set_next_tone_name(&info);

    player_set_next_file_func(set_next_tone_name);
    player_open(&info);

    return 0;
}

#if MODE_MUSIC_EN

AT(.player_seg)
void player_proc_event(uint32_t event)
{
    switch (event) {
    case PLAYER_EVENT_END_OF_FILE:
    case PLAYER_EVENT_ERR:
        //自动播放上(下）一首
        if (auto_play_file(0) == 0) {
            disp_menu(MENU_NUM, np.file_num, 0);
        }
        break;
    }
}

AT(.player_seg)
int play_file(int index, bool reset_pause_sta)
{
    if (common_tone_is_playing()) {
        return 0;
    }

    np.file_num = index;

    if ((np.file_num == 0) || (np.file_num > np.manager.total_file)) {
        return 0;
    }

    if (reset_pause_sta) {
        np.paused = false;
    } 
    np.fast = false;

    source_info_t info;
    memset(&info, 0, sizeof(info));
    info.type = SLTK_STYPE_FATFS;
    info.dev_type = (DEV_UDISK == device_get_num()) ? PLAYER_DEV_UDISK : PLAYER_DEV_SDCARD;
    info.num = np.file_num;
    info.seek_time = get_breakpoint_time();
    info.pause = np.paused;
    logd("play file num:%d, seek_time:%d  np.paused=%d", info.num, info.seek_time, np.paused);
    player_open(&info);

    return 0;
}

AT(.player_seg)
void set_play_direction(enum play_direction_e play_direction)
{
    if (np.repeat_mode == REPEAT_SINGLE) {
        np.play_direction = PLAY_SELECT;
    } else {
        np.play_direction = play_direction;
    }
}

AT(.player_seg)
void play_switch_file(bool next_file)
{
    if (common_tone_is_playing()) {
        return;
    }

    if (device_get_num() == DEV_SDCARD) {
        np.manager.sdcard_file_fail_count = 0;
    } else if (device_get_num() == DEV_UDISK) {
        np.manager.udisk_file_fail_count = 0;
    }

    set_play_direction(next_file ? PLAY_NEXT : PLAY_PREV);
    /* 不跳文件夹播放 */
    if (auto_play_file(0) == 0) {
        disp_menu(MENU_NUM, np.file_num, 0);
    }
}

AT(.player_seg)
void play_switch_folder(bool next_folder)
{
    if (common_tone_is_playing()) {
        return;
    }

    if (device_get_num() == DEV_SDCARD) {
        np.manager.sdcard_file_fail_count = 0;
    } else if (device_get_num() == DEV_UDISK) {
        np.manager.udisk_file_fail_count = 0;
    }

    set_play_direction(PLAY_NEXT);

    /* 跳文件夹播放 */
    if (auto_play_file(next_folder ? 1 : -1) < 0) {
    } else {
        disp_menu(MENU_NUM, np.file_num, 0);
    }
}

AT(.player_seg)
int auto_play_file(int32_t dir_inc)
{
    source_info_t info;
    int step = 1;

    if (common_tone_is_playing()) {
        return 0;
    }

#if MUSIC_NUM_STEP10
    bool flag_n10 = false;
    if (np.n10) {
        np.n10 = false;
        flag_n10 = true;
    }
#endif

    if (device_get_sta(device_get_num()) < DEV_OK) {
        if (play_switch_device(PLAY_SELECT, false) < 0) {
            device_set_num(DEV_NONE);
            loge("no dev1");
            mode_switch_auto();
            return -1;
        }

        disp_state(STATE_MUSIC_PLAY);
        return 0;
    }

    if (dir_inc) {
        /* 切上一个文件夹或者下一个文件夹 */
        if (np.dir_num == 0 && dir_inc < 0) {
            np.dir_num = np.total_dir_num - 1;
        } else if (np.dir_num == np.total_dir_num - 1 && dir_inc > 0) {
            np.dir_num = 0;
        } else {
            np.dir_num = (int32_t)np.dir_num + dir_inc;
        }

        goto change_dir;
    }

    switch(np.repeat_mode) {
    case REPEAT_ALL: /* 所有文件重复播放模式 */
#if MUSIC_DEVICE_SWITCH
        if ((np.play_direction == PLAY_PREV && np.file_num == 1) ||
            (np.play_direction == PLAY_NEXT && np.file_num == np.manager.total_file)) {
            /* 切下一个设备播放 */
            if (play_switch_device(np.play_direction, true) < 0) {
                player_close(0);
                device_set_num(DEV_NONE);
                loge("no dev0");
                mode_switch_auto();
                return -1;
            }
            disp_state(STATE_MUSIC_PLAY);
            return 0;
        }
#endif

        /* 获取新的文件编号 */
        if (np.play_direction == PLAY_NEXT) {
#if MUSIC_NUM_STEP10
            if (flag_n10) { /* 下10首 */
                np.file_num += 10;
                if (np.file_num > np.manager.total_file) {
                    np.file_num -= np.manager.total_file;
                }
            } else
#endif
            {
                np.file_num++; /* 下一首 */
            }

        } else if (np.play_direction == PLAY_PREV) {
#if MUSIC_NUM_STEP10
            if (flag_n10) {  /* 上10首 */
                if (np.file_num <= 10) {
                    np.file_num = np.manager.total_file - 10 + np.file_num;
                } else {
                    np.file_num -= 10;
                }
            } else
#endif
            {
                np.file_num--;  /* 上一首 */
            }
        }
        if (np.file_num == 0) {
            np.file_num = np.manager.total_file;
        } else if (np.file_num > np.manager.total_file) {
            np.file_num = 1;
        }
        break;

    case REPEAT_SINGLE: /* 单曲循环播放模式 */
        np.play_direction = PLAY_SELECT;
        break;

    case REPEAT_FOLDER: /* 文件夹内循环播放模式 */
        if (np.wait_get_folder_range) {
            uint32_t min_num;
            uint32_t count;
            uint32_t res;

            /* 先停止播放，然后获取当前文件夹起始文件编号以及当前文件夹总文件个数 */
            player_close(0);
            res = xfs_get_folder_file_num_range(np.file_num, &min_num, &count);
            if (FR_OK == res) {
                logv("##cur:%d, min_num:%d, count:%d",
                    np.last_num, min_num, count);
                np.manager.limit_min_num = min_num;
                np.manager.limit_max_num = min_num + count - 1;
            } else {
                logv("##root folder, cur:%d, min_num:%d, count:%d",
                    np.last_num, 1, np.manager.total_file);
                np.manager.limit_min_num = 1;
                np.manager.limit_max_num = np.manager.total_file;
            }
            np.wait_get_folder_range = false;
        }

        /* 检查文件夹的文件编号范围 */
        if (np.play_direction == PLAY_NEXT) {
            if (np.file_num + step > np.manager.limit_max_num) {
                np.file_num = (int)np.manager.limit_min_num +
                    (int)np.file_num + step -
                    (int)np.manager.limit_max_num - 1;
            } else {
                np.file_num += step;
            }
        } else if (np.play_direction == PLAY_PREV) {
            if ((int)np.file_num - step < (int)np.manager.limit_min_num) {
                np.file_num = (int)np.manager.limit_max_num +
                    (int)np.file_num + 1 - (int)np.manager.limit_min_num - step;
            } else {
                np.file_num -= step;
            }
        }
        break;

    case REPEAT_RANDOM: /* 随机播放模式 */
        np.file_num = trng_get_random_data() % np.manager.total_file;
        break;
    }

change_dir:
    if (np.manager.total_file == 0) {
        loge("no file");
        return -1;
    }

    np.paused = false;
    np.fast = false;

    if (dir_inc) { /* 切文件夹播放 */
        logd("seek dir:%d", np.dir_num);
        /* 跳转文件夹播放 */
        np.file_num_dirty = true; //表示文件编号已经过时，需要在OPEN完成后获取最新的文件编号
        memset(&info, 0, sizeof(info));
        info.type = SLTK_STYPE_FATFS;
        info.dev_type = (DEV_UDISK == device_get_num()) ? PLAYER_DEV_UDISK : PLAYER_DEV_SDCARD;
        info.num = np.dir_num;
        info.dir = true; //表明info.num是文件夹编号
        info.seek_time = get_breakpoint_time();
        logd("2 play dir num:%d, seek_time:%d", info.num, info.seek_time);
        player_open(&info);
    } else { /* 切文件播放 */
        memset(&info, 0, sizeof(info));
        info.type = SLTK_STYPE_FATFS;
        info.dev_type = (DEV_UDISK == device_get_num()) ? PLAYER_DEV_UDISK : PLAYER_DEV_SDCARD;
        info.num = np.file_num;
        info.seek_time = get_breakpoint_time();

        logd("2 play file num:%d, seek_time:%d", info.num, info.seek_time);
        player_open(&info);
    }

    disp_state(STATE_MUSIC_PLAY);

    return 0;
}

AT(.player_seg)
int play_new_device(uint32_t dev_num, uint32_t direction, bool sd, bool scan)
{
#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    //如果PC在线，不能尝试去操作U盘，PC会断开
    if((dev_num == DEV_UDISK) && (device_get_sta(DEV_PC) != DEV_OFFLINE)) {
        return PLAYER_RET_DEV_OFFLINE;
    }
#endif
    int ret = PLAYER_RET_SUCCESS;
    //logi("%s,%d,%d", __func__,dev_num,direction);

    disp_menu(MENU_WAITING, 0, 0);

    /* Before mounting the file system must ensure
     * that the player has closed.
     */
    if (SLTK_STYPE_FATFS == player_get_stream_source()) {
        if (direction == PLAY_SELECT) {
            np.breakpoint_wr = 1;
        } else {
            np.breakpoint_wr = 2;
        }

        player_close(0); //close player immediately

        xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
#if SD_EN
        if (sd) {       //if last device is sdcard, stop sd.
            sd_stop();
        }
#endif
    }

    event_clear(DEVICE_EVENT_SD_FAIL);
    event_clear(PLAYER_EVENT_ERR);

    if (device_try(dev_num) == 0) {
        if (file_scan(scan) == 0) {
            if (np.manager.total_file == 0) {
                loge("no valid file");
                xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
#if SD_EN
                if (dev_num == DEV_SDCARD) {
                    sd_detect_set(1);
                }
#endif
                #if DISP_EN
                    disp_state(STATE_MUSIC_NODEV);
                #endif
                return PLAYER_RET_DEV_NOFILE;
            } else {
                if (direction == PLAY_NEXT) {
                    np.file_num = 1;
                    np.ptime.ms = 0;
                    np.breakpoint_rd = 0;
                } else if (direction == PLAY_PREV) {
                    np.file_num = np.manager.total_file;
                    np.ptime.ms = 0;
                    np.breakpoint_rd = 0;
                } else {
                    play_num_read();
                }
                if (scan) {
                    if (dev_num == DEV_SDCARD) {
                        //播放"SD卡/TF卡模式"提示音
                    } else {
                        //播放"U盘模式"提示音
                    }
                }
#if (BT_BACKGROUND_EN && USB_DISK_EN)
                if (dev_num == DEV_UDISK) {
                    xfsdisk_delay_for_bt_enable(TRUE);
                }
#endif
                play_file(np.file_num, true);
                disp_menu(MENU_NUM, np.file_num, 0);
                disp_state(STATE_MUSIC_PLAY);
#if SD_EN
                if (dev_num != DEV_SDCARD) {
                    sd_detect_set(1);
                }
#endif
            }
            ret = PLAYER_RET_SUCCESS;
        } else {
            device_set_sta(dev_num, DEV_FS_FAIL);
#if SD_EN
            if (dev_num == DEV_SDCARD) {
                sd_detect_set(1);
            }
#endif
#if USB_DISK_EN
            if(dev_num == DEV_UDISK) {
                usb_host_deinit();
                usb_detect_set(USB_DETECT);
            }
#endif
            ret = PLAYER_RET_DEV_NOFILE;
        }
    } else {
#if SD_EN
        if (dev_num == DEV_SDCARD) {
            sd_detect_set(1);
        }
#endif
#if USB_DISK_EN
        if(dev_num == DEV_UDISK) {
            usb_host_deinit();
            usb_detect_set(USB_DETECT);
        }
#endif
        ret = PLAYER_RET_DEV_OFFLINE;
    }

    //disp_state(STATE_MUSIC_STOP);
    return ret;
}

AT(.player_seg)
int play_switch_device(uint32_t direction, bool back)
{
    int ret;
    bool scan;

    if (device_get_num() == DEV_UDISK) {
        ret = play_new_device(DEV_SDCARD, direction, 0, true);
        if (ret != PLAYER_RET_SUCCESS) {
            if (back) {
                scan = (PLAYER_RET_DEV_OFFLINE == ret) ? false : true;
                ret = play_new_device(DEV_UDISK, direction, 0, scan);
                if (ret < 0) {
                    return ret;
                }
            } else {
                return ret;
            }
        }
    } else if (device_get_num() == DEV_SDCARD) {
        ret = play_new_device(DEV_UDISK, direction, 0, true);
        if (ret != PLAYER_RET_SUCCESS) {
            if (back) {
                msleep(10);
                scan = (PLAYER_RET_DEV_OFFLINE == ret) ? false : true;
                ret = play_new_device(DEV_SDCARD, direction, 0, scan);
                if (ret < 0) {
                    return ret;
                }
            } else {
                return ret;
            }
        }
    } else {
        return PLAYER_RET_DEV_OFFLINE;
    }

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int play_or_pause(void)
{
    if (COMMON_TONE_STATUS_PLAYING == np.common_tone_status) {
        return PLAYER_RET_SUCCESS;
    }

    if (!np.paused && AP_STATUS_PLAYING == player_get_status()) {
        player_pause(0);
        np.paused = true;
        disp_state(STATE_MUSIC_STOP);
		auto_shutdown(true);
	} else {
        player_resume(0);
        np.paused = false;
        disp_state(STATE_MUSIC_PLAY);

        disp.menu = MENU_SCENE_MAIN;
        get_play_time();
        disp_menu(MENU_MUSIC_PLAY, np.ptime.min, np.ptime.sec);
		auto_shutdown(false);
    }

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int fast_step(int32_t step_ms)
{
    media_info_t info;
    int32_t t;

    np.fast = true;

    if (np.mute_for_fast) {
        int32_t ret;
        ret = player_get_media_info(&info);
        if (ret < 0) {
            return PLAYER_RET_SUCCESS;
        }

        if (!np.paused) {
            player_pause(portMAX_DELAY);
            np.paused = true;
            np.resume_for_fast = true;
        }

        if (np.fast_ms == 0) {
            np.last_time = player_get_current_play_time();
        }

        np.fast_ms += step_ms;
        t = np.last_time + np.fast_ms;
        if ( t < 0) {
            t = 0;
        } else if (t > info.total_time) {
            t = 0;
            player_seek(t);
            /* auto play next file */
            np.play_direction = PLAY_NEXT;
            np.fast_ms = 0;
            np.last_time = 0;
            np.fast = false;
            auto_play_file(0);

            np.paused = false;
            np.resume_for_fast = false;
        }

        /* 第一次按键一秒后才开始调用seek, 避免淡出时seek */
        if (np.fast_ms / step_ms > 1000 / FAST_EVENT_PERIOD) {
            player_seek(t);
        }
    } else {
        t = player_get_current_play_time();
        t = t + step_ms;
        t = (t < 0) ? 0 : t;
        /* If the time is greater than the total time,
         * the PLAYER_EVENT_END_OF_FILE event will be triggered,
         * and the next file will be played automatically.
         */
        player_seek(t);
    }

    logi("seek to %ds%dms", t / 1000, t % 1000);

    t = t / 1000;
    disp_menu(MENU_MUSIC_PLAY, t/60, t%60);

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
void fast_skip(bool forward)
{
    sleep_prevent_set(PS_APP_PROCESS);

    uint32_t t = timestamp_to_ms(get_timestamp());
    if (t - np.fast_event_timestamp >= FAST_EVENT_PERIOD) {
        np.fast_event_timestamp = t;
        fast_step(forward ? FAST_STEP_MS : -FAST_STEP_MS);
    }
}

AT(.player_seg)
int fast_end(void)
{
    if (np.mute_for_fast) {
        media_info_t info;
        int32_t t;
        player_get_media_info(&info);
        t = np.last_time + np.fast_ms;
        if ( t < 0) {
            t = 0;
        } else if (t > info.total_time) {
            t = info.total_time;
        }
        player_seek(t);

        /* If is playing before, it will continue to play.
         * Otherwise, hold pause status.
         */
        if (np.resume_for_fast) {
            player_resume(0);
            np.paused = false;
        }
        np.resume_for_fast = false;
    }

    np.fast_ms = 0;
    np.fast = false;

    sleep_prevent_clear(PS_APP_PROCESS);

    return PLAYER_RET_SUCCESS;
}
#endif

AT(.player_seg)
int volume_set(uint8_t vol)
{
    np.volume = vol;
    audio_service_set_playback_volume(np.volume);
    info_set(INFO_VOLUME, (void *)&np.volume, 1);

    return PLAYER_RET_SUCCESS;
}


AT(.player_seg)
int volume_down(void)
{
    if (np.volume > VOLUME_STEP) {
        np.volume -= VOLUME_STEP;
        audio_service_set_playback_volume(np.volume);
        info_set(INFO_VOLUME, (void *)&np.volume, 1);
    } /*else if (np.volume != 0) {  //PC mode np.volume step is 2, because PC have 50 steps
        np.volume = 0;
        audio_service_set_playback_volume(np.volume);
        info_set(INFO_VOLUME, (void *)&np.volume, 1);
    }*/

#if (TONE_EN && TONE_MIN_VOLUME_EN)
    if (np.volume == VOLUME_STEP) {
        audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最小音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int volume_up(void)
{
    if (np.volume <= VOLUME_MAX - VOLUME_STEP) {
        np.volume += VOLUME_STEP;
        audio_service_set_playback_volume(np.volume);
        info_set(INFO_VOLUME, (void *)&np.volume, 1);
    } else if (np.volume != VOLUME_MAX) {  //PC mode np.volume step is 2, because PC have 50 steps
        np.volume = VOLUME_MAX;
        audio_service_set_playback_volume(np.volume);
        info_set(INFO_VOLUME, (void *)&np.volume, 1);
    }

#if (TONE_EN && TONE_MAX_VOLUME_EN)
    if (np.volume == VOLUME_MAX) {
        audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最大音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int bt_call_volume_down(void)
{
    if (np.btcall_volume >= VOLUME_STEP) {
        np.btcall_volume -= VOLUME_STEP;
        audio_service_set_call_volume(np.btcall_volume);
        info_set(INFO_BTCALL_VOLUME, (void *)&np.btcall_volume, 1);
    }

#if (TONE_EN && TONE_MIN_VOLUME_EN)
    if (np.btcall_volume == 0) {
        audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最小音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int bt_call_volume_up(void)
{
    if (np.btcall_volume <= VOLUME_MAX - VOLUME_STEP) {
        np.btcall_volume += VOLUME_STEP;
        audio_service_set_call_volume(np.btcall_volume);
        info_set(INFO_BTCALL_VOLUME, (void *)&np.btcall_volume, 1);
    }

#if (TONE_EN && TONE_MAX_VOLUME_EN)
    if (np.btcall_volume == VOLUME_MAX) {
        audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT); /* 播放最大音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int set_repeat(void)
{
    np.repeat_mode++;
    if (np.repeat_mode > REPEAT_MAX) {
        np.repeat_mode = REPEAT_ALL;
    }

    if (np.repeat_mode != REPEAT_FOLDER) {
        np.manager.limit_min_num = 1;
        np.manager.limit_max_num = np.manager.total_file;
        np.wait_get_folder_range = false;
    } else {
        np.wait_get_folder_range = true;
    }

    disp_menu(MENU_REPEAT, np.repeat_mode, 0);

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int set_eq(void)
{
    //FIXME: config eq params
    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
int get_play_time(void)
{
    uint32_t tt;
    tt = player_get_current_play_time();
    np.ptime.ms = tt;
    tt = tt / 1000;
    np.ptime.min = tt / 60;
    np.ptime.sec = tt % 60;
    //logi("t get %d:%d, %d", np.ptime.min,np.ptime.sec,np.ptime.ms);

    return PLAYER_RET_SUCCESS;
}

AT(.player_seg)
uint32_t get_breakpoint_time(void)
{
    //logi("b:%d", np.breakpoint_rd);
    if (np.breakpoint_rd) {
        play_time_read();
    } else {
        np.ptime.ms = 0;
    }
    np.breakpoint_rd = 0;
    np.breakpoint_wr = 0;

    //logi("t set %d", np.ptime.ms);

    return np.ptime.ms;
}

AT(.com_seg)
uint8_t spec_get_info(uint8_t *buf)
{
    uint16_t sum = 0;
#if (AUDIO_PLAYBACK_SPECTRUM | AUDIO_PR_MIX_SPECTRUM)
    static uint32_t spec_cnt = 0;
    spec_cnt++;
    if (spec_cnt % SPECTRUM_SAMPLE_CNT == 0) {
        libc_memcpy(spec_buf, buf, SPECTRUM_BANDNUM);
#if 0
        for(int i = 0; i < SPECTRUM_BANDNUM; i++){
            if((SPECTRUM_BANDNUM - 1) == i){
                printf_debug("%d\n", spec_buf[i]);
            }
            else{
                printf_debug("%d, ", spec_buf[i]);
            }
        }
#endif
    }
    for(int i = 0; i < SPECTRUM_BANDNUM; i++){
        sum += buf[i];;
    }
    sum /= SPECTRUM_BANDNUM;
#endif
    return sum;
}

AT(.player_seg)
void play_num_read(void)
{
#if (MUSIC_INFO_SAVE_MODE == 0 || MUSIC_INFO_SAVE_MODE == 1)
    uint32_t dev_num;
    uint32_t checksum_cur;
    uint32_t checksum_last;

    xfs_cmd(XFS_CMD_GET_FILES_CHECKSUM, (uint32_t)&checksum_cur);

    dev_num = device_get_num();
#if (MUSIC_INFO_SAVE_MODE == 0)
    np.file_num = rtc_memory_read(INFO_MUSIC_FNUM+dev_num);
    checksum_last = rtc_memory_read(INFO_FILE_CHECKSUM+dev_num);
#elif (MUSIC_INFO_SAVE_MODE == 1)
    info_get(INFO_SDCARD_NUM+dev_num*4, (void *)&np.file_num, 4);
    info_get(INFO_SDCARD_CHECKSUM+dev_num*4, (void *)&checksum_last, 4);
#endif
    //logd("files checksum:cur%#x,last%#x", checksum_cur,checksum_last);
    if (checksum_cur != checksum_last) {
        np.file_num = 1;
        np.ptime.ms = 0;
        np.breakpoint_rd = 0;
#if (MUSIC_INFO_SAVE_MODE == 0)
        rtc_memory_write(INFO_FILE_CHECKSUM+dev_num, checksum_cur);
#elif (MUSIC_INFO_SAVE_MODE == 1)
        info_set(INFO_SDCARD_CHECKSUM+dev_num*4, (void *)&checksum_cur, 4);
#endif
    } else {
        np.breakpoint_rd = 1;
    }

    np.file_num = np.file_num ? np.file_num : 1;
    //logd("play num read:%d,%d", dev_num,np.file_num);
#endif
}

AT(.player_seg)
void play_num_write(void)
{
#if (MUSIC_INFO_SAVE_MODE == 0 || MUSIC_INFO_SAVE_MODE == 1)
    uint32_t dev_num;

    dev_num = device_get_num();
    //logd("p num write:%d,%d", dev_num,np.file_num);
#if (MUSIC_INFO_SAVE_MODE == 0)
    rtc_memory_write(INFO_MUSIC_FNUM+dev_num, np.file_num);
#elif (MUSIC_INFO_SAVE_MODE == 1)
    info_set(INFO_SDCARD_NUM+dev_num*4, (void *)&np.file_num, 4);
    info_write_page();
#endif

#endif
}

AT(.player_seg)
void play_time_read(void)
{
#if (MUSIC_INFO_SAVE_MODE == 0)
    uint32_t dev_num;

    dev_num = device_get_num();
    np.ptime.ms = rtc_memory_read(INFO_MUSIC_PTIME+dev_num);
#else

#endif
    //logd("play time read:%d", np.ptime.ms);
}

AT(.player_seg)
void play_time_write(void)
{
#if (MUSIC_INFO_SAVE_MODE == 0)
    uint32_t dev_num;

    dev_num = device_get_num();
    rtc_memory_write(INFO_MUSIC_PTIME+dev_num, np.ptime.ms);
#else

#endif
    //logd("play time write:%d", np.ptime.ms);
}

AT(.player_seg)
void play_info_write_when_poweroff(void)
{
#if (MUSIC_INFO_SAVE_MODE == 2)
    //logi("poweroff vol:%u num:%u min:%u, sec:%u", np.volume, np.file_num, np.ptime.min, np.ptime.sec);
    info_set(INFO_SDCARD_NUM, (void *)&np.file_num, 4);
    info_set(INFO_SDCARD_TIME, (void *)&np.ptime.ms, 4);
    /*这里只更新buf，在mode_poweroff中统一写flash*/
#endif
}
