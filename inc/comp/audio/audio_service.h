#ifndef _AUDIO_SERVICE_H_
#define _AUDIO_SERVICE_H_

#include "sys_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "audio_effects.h"

#define REC_IN   0
#define PLBK_OUT 1

/*
 * pcm数据流信息结构体
 */
typedef struct {
    void *buf;              /* pcm 数据buffer */
    uint32_t fs;            /* 采样率（e.g. 48000) */
    uint16_t samples;         /* buffer数据长度(samples) */
    uint8_t channels;       /* 声道数 */
    uint8_t sample_size;    /* 采样点数据大小（e.g. 16bits大小为2， 32bits大小为4） */
} audio_stream_info_t;

/*
 * 录音类型
 */
enum audio_capture_type_e {
    AUDIO_CAPTURE_TYPE_NONE,                /* 不录音 */
    AUDIO_CAPTURE_TYPE_MUSIC_AFX_OUTPUT,    /* 录制音乐音效输出的数据 */
    AUDIO_CAPTURE_TYPE_MIC_AFX_INPUT,       /* 录MIC音效前的数据 */
    AUDIO_CAPTURE_TYPE_MIC_AFX_OUTPUT,      /* 录MIC音效后的数据 */
    AUDIO_CAPTURE_TYPE_MUSIC_MIC_MIX,       /* 录MIC和音乐混音后的数据（不包含USBaudio IN） */
    AUDIO_CAPTURE_TYPE_ALL,                 /* 录所有声音混音的数据（包含USBaudio IN） */
};

/*
 * 音频服务初始化参数
 */
typedef struct {
    uint32_t music_features;
    bool karaoke_mode;
    uint32_t spiflash_sel;
} audio_service_init_params_t;


/*
 * 音频服务cmd
 */
enum audio_service_cmd_e {
    AS_CMD_NONE,

    /* vocal cut(消除人声功能); (karaoke mode command)
     * The command argument indicates enable/disable vocal cut
     * Example:
     *    //enable vocal cut
     *    audio_service_cmd(AS_CMD_VOCAL_CUT, 1);
     *    //disable vocal cut
     *    audio_service_cmd(AS_CMD_VOCAL_CUT, 0);
     */
    AS_CMD_VOCAL_CUT,

    /* audio duck(闪避功能); (karaoke mode command)
     * The command argument indicates enable/disable duck
     * Example:
     *    //enable duck
     *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 1);
     *    //disable duck
     *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 0);
     */
    AS_CMD_DUCK_ENABLE,

    /* config duck attack time; (karaoke mode command)
     * The command argument indicates attack time
     * Example:
     */
    AS_CMD_DUCK_SETTING_ATTACK_TIME,

    /* config duck release time; (karaoke mode command)
     * The command argument indicates release time
     * Example:
     */
    AS_CMD_DUCK_SETTING_RELEASE_TIME,

    /* config duck gain; (karaoke mode command)
     * The command argument indicates duck gain
     * Example:
     */
    AS_CMD_DUCK_SETTING_DUCK_GAIN,

    /* config duck thd; (karaoke mode command)
     * The command argument indicates duck thd
     * Example:
     */
    AS_CMD_DUCK_SETTING_DUCK_THD,
    AS_CMD_DUCK_GETTING_ATTACK_TIME,
    AS_CMD_DUCK_GETTING_RELEASE_TIME,
    AS_CMD_DUCK_GETTING_DUCK_GAIN,
    AS_CMD_DUCK_GETTING_DUCK_THD,

    /* play auxtrack（开始播放SBC提示音）; (karaoke mode command)
     * The command argument indicates auxtrack number
     * Example:
     *    //play auxtrack num 4
     *    audio_service_cmd(AS_CMD_PLAY_AUXTRACK, 4);
     */
    AS_CMD_PLAY_AUXTRACK,

    /* stop auxtrack（停止播放SBC提示音）; (karaoke mode command)
    * The command argument is ignored
    * Example:
    * audio_service_cmd(AS_CMD_STOP_AUXTRACK, 0);
    */
    AS_CMD_STOP_AUXTRACK,

    /* set auxtrack volume（设置SBC提示音音量）; (karaoke mode command)
     * The command argument indicates auxtrack volume(0~100)
     * Example:
     *    //set auxtrack volume 80
     *    audio_service_cmd(AS_CMD_SET_AUXTRACK_VOLUME, 80);
     */
    AS_CMD_SET_AUXTRACK_VOLUME,

    /* get auxtrack playing status（获取SBC提示音播放状态）; (karaoke mode command)
     * The command argument is a @uint32_t pointer
     * Example:
     *    uint32_t sta;
     *    audio_service_cmd(AS_CMD_GET_AUXTRACK_STATUS, (uint32_t)&sta);
     *    if (sta) {
     *        //auxtrack playing
     *    } else {
     *        //auxtrack idle
     *    }
     */
    AS_CMD_GET_AUXTRACK_STATUS,


    /* set mic channels mask; (karaoke mode command)
     * The command argument indicates channels mask(bit0-ADC0, bit1-ADC1, bit2-ADC2)
     * Example:
     *    //ADC0
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x1);
     *    //ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x2);
     *    //ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x4);
     *    //ADC0,ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x3);
     *    //ADC0,ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x5);
     */
    AS_CMD_SET_KARAOKE_MIC_CH_MASK,

    /* set linein channels mask; (karaoke mode command)
     * The command argument indicates channels mask(bit0-ADC0, bit1-ADC1, bit2-ADC2)
     * Example:
     *    //ADC0
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x1);
     *    //ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x2);
     *    //ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x4);
     *    //ADC0,ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x3);
     *    //ADC0,ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x5);
     */
    AS_CMD_SET_KARAOKE_LINEIN_CH_MASK,

    /* set self mix mode; (karaoke mode command)
     * The command argument is ignored
     * Example:
     *    //set self mix mode
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_SELF_MIX_MODE, 1);
     */
    AS_CMD_SET_KARAOKE_SELF_MIX_MODE,


    /* set direct mix mode; (karaoke mode command)
     * The command argument is ignored
     * Example:
     *    //set direct mix mode
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_DIRECT_MIX_MODE, 1);
     */
    AS_CMD_SET_KARAOKE_DIRECT_MIX_MODE,


    /* mute/unmute karaoke mic when play music; (karaoke mode command)
     * The command argument indicates mute/unmute
     * Example:
     *    //mute mic when play music
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_REC_SWITCH, 1);
     *    //unmute mic when play music
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_REC_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_REC_SWITCH,


    /* enable/disabel mic0/mic1; (karaoke mode command)
     * The command argument indicates enable/disable
     * Example:
     *    //enable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC01_SWITCH, 1);
     *    //disable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC01_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_AMIC01_SWITCH,

    /* enable/disabel linein0/linein1; (karaoke mode command)
     * The command argument indicates enable/disable
     * Example:
     *    //enable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_LINE01_SWITCH, 1);
     *    //disable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_LINE01_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_LINE01_SWITCH,


    /* enable/disabel mic2; (karaoke mode command)
     * The command argument indicates enable/disable
     * Example:
     *    //enable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC2_SWITCH, 1);
     *    //disable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC2_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_AMIC2_SWITCH,


    /* Once freq shift's runtime prm has been settled, it should
     * be called by user to sync prm with driver
     * Example:
     *    audio_service_cmd(AS_CMD_NOTIFY_FS_LOAD_RTPRM, 0);
     */
    AS_CMD_NOTIFY_FS_LOAD_RTPRM,


    /* Once NHS's runtime prm has been settled, it should
     * be called by user to sync prm with driver
     * Example:
     *    audio_service_cmd(AS_CMD_NOTIFY_NHS_LOAD_RTPRM, 0);
     */
    AS_CMD_NOTIFY_NHS_LOAD_RTPRM,

    AS_CMD_CMD_STOP_ADMOMINI,
};

/*
 * audio input source define
 */
enum audio_input_source_e {
    AUDIO_INPUT_AMIC0           =  0, /* 模拟MIC0 */
    AUDIO_INPUT_AMIC1           =  1, /* 模拟MIC1 */
    AUDIO_INPUT_AMIC2           =  2, /* 模拟MIC2 */
    AUDIO_INPUT_DMIC0F          =  3, /* 数字MIC0下降沿(falling edge) */
    AUDIO_INPUT_DMIC0R          =  4, /* 数字MIC0上升沿(rising edge) */
    AUDIO_INPUT_AMIC0_1         =  5, /* 模拟MIC0和模拟MIC1 */
    AUDIO_INPUT_AMIC0_2         =  6, /* 模拟MIC0和模拟MIC2 */
    AUDIO_INPUT_AMIC1_2         =  7, /* 模拟MIC1和模拟MIC2 */
    AUDIO_INPUT_DMIC0F_R        =  8, /* 数字MIC0下降沿和下降沿 */
    AUDIO_INPUT_LINEIN0         =  9, /* linein0 */
    AUDIO_INPUT_LINEIN1         = 10, /* linein1 */
    AUDIO_INPUT_LINEIN0_1       = 11, /* linein0和linein1 */
    AUDIO_INPUT_AMIC_ALL        = 12,
};

/*
 * 音频焦点拥有者，表示当前谁在使用音频资源
 */
enum audiofocus_holder_e {
    AUDIOFOCUS_HOLDER_NONE,
    AUDIOFOCUS_HOLDER_AUDIOPLAYER,  /* 表示本地播放或者提示音播放 */
    AUDIOFOCUS_HOLDER_A2DPAUDIO,    /* 表示蓝牙音乐 */
    AUDIOFOCUS_HOLDER_SCOAUDIO,     /* 表示蓝牙通话 */
    AUDIOFOCUS_HOLDER_USBAUDIO,     /* 表示usb audio */
    AUDIOFOCUS_HOLDER_LINEINAUDIO,  /* 表示linein播放 */
    AUDIOFOCUS_HOLDER_SPDIFAUDIO,   /* 表示spdif播放 */
};

/*
 * 音频特性
 */
enum audio_feature_e {
    AUDIO_FEATURE_NONE                  = 0,
    AUDIO_FEATURE_PLAYBACK_DECODER      = (1 << 0),  /* 表示有解码器在工作 */
    AUDIO_FEATURE_PLAYBACK_EFFECTS      = (1 << 1),  /* 表示有播放音效在工作 */
    AUDIO_FEATURE_RECORD_ENCODER        = (1 << 2),  /* 表示有编码器在工作 */
    AUDIO_FEATURE_RECORD_EFFECTS        = (1 << 3),  /* 表示有录音（MIC）音效在工作 */
    AUDIO_FEATURE_SPEAKER_AENC          = (1 << 4),  /* 表示扬声器类型的通话算法在工作 */
    AUDIO_FEATURE_HEADSET_AENC          = (1 << 5),  /* 表示耳机类型的通话算法在工作 */
    AUDIO_FEATURE_TONEMIXER             = (1 << 6),  /* 表示正弦波提示音在工作 */
    AUDIO_FEATURE_AUXTRACK              = (1 << 7),  /* 表示auxtrack提示音在工作 */
    AUDIO_FEATURE_A2DP_FADE             = (1 << 8),  /* 表示蓝牙音乐正在进行淡入淡出处理 */
    AUDIO_FEATURE_SCO_TSPS              = (1 << 9),  /* 表示通话变调处理在工作 */
};

enum music_feature_e {
    MUSIC_FEATURE_METADATA  = (1 << 0),
    MUSIC_FEATURE_WAV       = (1 << 1),
    MUSIC_FEATURE_MP3       = (1 << 2),
    MUSIC_FEATURE_WMA       = (1 << 3),
    MUSIC_FEATURE_FLAC      = (1 << 4),
    MUSIC_FEATURE_AAC       = (1 << 5),
    MUSIC_FEATURE_APE       = (1 << 6),
    MUSIC_FEATURE_OGG       = (1 << 7),
};

/*
 * 解码器类型
 */
enum audio_decoder_type_e {
    AUDIO_DECODER_TYPE_NONE,
    AUDIO_DECODER_TYPE_WAV,
    AUDIO_DECODER_TYPE_MP3,
    AUDIO_DECODER_TYPE_WMA,
    AUDIO_DECODER_TYPE_FLAC,
    AUDIO_DECODER_TYPE_AAC,
    AUDIO_DECODER_TYPE_APE,
    AUDIO_DECODER_TYPE_OGG,
    AUDIO_DECODER_TYPE_SBC,
    AUDIO_DECODER_TYPE_CVSD,
};

/*
 * 编码器类型
 */
enum audio_encoder_type_e {
    AUDIO_ENCODER_TYPE_NONE,
    AUDIO_ENCODER_TYPE_WAV,
    AUDIO_ENCODER_TYPE_MP3,
    AUDIO_ENCODER_TYPE_SBC,
    AUDIO_ENCODER_TYPE_CVSD,
    AUDIO_ENCODER_TYPE_MP2
};

/*
 * 正弦波提示音类型
 */
enum audio_sin_tone_type_e {
    SIN_TONE_VOLUME_LIMIT, /* 最大最小音量提示音 */
    SIN_TONE_BAT_LOW,      /* 低电提示音 */
    SIN_TONE_APP,          /* app用户自定义的提示音 */
};

/* Must be synchronized with internal header files */
enum phone_featrues_e {
    /* acoustic echo cancellaltion (adaptive filter) mask */
    PHONE_AEC        = (1 << 0),
    /* acoutic echo suppression  mask */
    PHONE_AES        = (1 << 1),
    /* ambient noise suppression mask */
    PHONE_NS         = (1 << 2),
    /* dereverberation mask */
    PHONE_DR         = (1 << 3),
    /* noise-dependent volume control of Rx mask */
    PHONE_AVC        = (1 << 4),
    /* automatic gain control for Tx mask */
    PHONE_TXAGC      = (1 << 5),
    /* automatic gain control for Rx mask */
    PHONE_RXAGC      = (1 << 6),
    /* comfort noise generation mask */
    PHONE_CNG        = (1 << 7),
    /* deemphasis mask */
    PHONE_EMP        = (1 << 8),
    /* dc notch of tx mask */
    PHONE_TXNOT      = (1 << 9),
    /* gain smooth mask */
    PHONE_GSMOOTH    = (1 << 10),
};


#define SPIFLASH_TYPE_MASK (0x0F)

/*
 * flash类型
 */
enum spiflash_type_e {
    SPIFLASH_TYPE_INT       = 0,   /* 内部flash */
    SPIFLASH_TYPE_EXT_SPI0  = 1,   /* 外部spi0 flash */
    SPIFLASH_TYPE_EXT_SPI1  = 2,   /* 外部spi1 flash */
};

/*
 * flash 提示音资源类型
 */
enum spiflash_tone_type_e {
    SPIFLASH_TONE_NUM       = 0,  /* mp3提示音(此类型提示音不能和音乐同时播放) */
    SPIFLASH_AUXTRACK_NUM   = 4,  /* 目前仅在卡拉OK模式可用，支持SBC格式（此类型支持提示音和音乐同时播放）*/
};

/*
 * 基本的通话降噪算法参数
 */
typedef struct {
    uint16_t func_mask;
    float alpha_slow;
    float alpha_fast;
    float pf_GHmin;

    float res_overest;

    float dt_thd_norm;

    float pf_Xi_opt;
    float Npsd_bias;

    float pf_Gcn;

    float avq_reftx;
    float avq_refrx;
    float avq_vadthd;
    float avq_offset;
    float avq_Gmax;

    float gmax;
    float gmin;
    float xtgt;
} phone_aenc_config_params_t;

/*
 * 其他通话降噪算法参数
 */
typedef struct {
    float gain_smooth_thd;
    float gain_smooth_width;
    float SNR_s_Min;
    float SNR_ns_Min;
    float psd_Min;
    float pf_alpha_Xis;
    float pf_alpha_Xins;

    float alpha_fast_resG;
    float alpha_slow_resG;
    float dt_thd_limit;
    float dt_init_thd;
    float dt_steprise_decvar;
    float dt_stepfall_decvar;

    uint8_t finit;
    float pf_ph1_thd;
    float pf_alpha_npsd;
    float pf_alpha_PH1;

    float pf_alpha_Nmin_f;
    float pf_inc_Nmin_t;

    float pf_alpha_z;
    float pf_decay;
    float decay_lthd;
    float alpha_decay;
    float decay_km;
    float rise_km;
    float thd_km;

    float avq_dec_step;
    float avq_sf_norm;
    float avq_alpha_fref;
    float avq_alpha_sref;
    float avq_alpha_fn;
    float avq_alpha_sn;

    float alpha_fr;
    float alpha_ff;
    float alpha_sr;
    float alpha_sf;
    float vad_alpha_short;
    float vad_alpha_long;
    float vad_alpha_ratio;
    float vad_log_ratio_uthd;
    float vad_log_ratio_lthd;
    float vad_log_ratio_min;
    float vad_log_ratio_max;
    float vad_var_min;
    float amp_min;
} phone_aenc_params_t;

/*
 * 获取本地播放特性
 */
uint32_t audio_service_get_music_features(void);

/*
 * 获取audio_player组件句柄
 */
void *audio_service_get_audio_player(void);

/*
 * 获取audio_track组件句柄
 */
void *audio_service_get_audio_track(void);

/*
 * 获取karaoke组件句柄
 */
void *audio_service_get_karaoke(int8_t from_rec);

/*
 * 获取ain组件句柄
 */
void *audio_service_get_ain(void);

/*
 * 获取recorder_ain组件句柄
 */
void *audio_service_get_recorder_ain(void);

/*
 * 获取aout组件句柄
 */
void *audio_service_get_aout(void);

/*
 * 获取dac_aout组件句柄
 */
void *audio_service_get_dac_aout(void);

/*
 * 获取录音（MIC)音效链组件句柄
 */
void *audio_service_get_record_afx_chain(void);

/*
 * 获取音乐音效链组件句柄
 */
void *audio_service_get_playback_afx_chain(void);

uint8_t audio_service_get_afx_chain_attach_status(uint8_t io);

/* 获取tone提示音flash类型；
 * @return: 返回值参考@enum spiflash_type_e
 */
uint8_t audio_service_get_tone_spiflash_type(void);

/* 获取auxtrack提示音flash类型；
 * @return: 返回值参考@enum spiflash_type_e
 */
uint8_t audio_service_get_auxtrack_spiflash_type(void);

/*
 * 初始化音频服务
 */
int audio_service_init(audio_service_init_params_t *params
    , afx_work_flow* awf_rec, int32_t awf_rec_num, afx_work_flow* awf_plbk, int32_t awf_plbk_num);
/*
    * 关闭音频服务
 */
void audio_service_deinit(void);

/*
 * 创建卡拉OK组件
 */
int audio_service_create_karaoke(void);
/*
 * 销毁卡拉OK组件
 */
int audio_service_destroy_karaoke(void);

/* 添加音频特性；
 * 此函数一般由audio库调用
 * @param features: 参考@enum audio_feature_e
 * 返回 ：预设音频特性是否已经存在
 */
bool audio_service_add_features(uint32_t features);

/* 清除音频特性；
 * 此函数一般由audio库调用
 * @param features: 参考@enum audio_feature_e
 * 返回 ：预清音频特性是否已经存在
 */
bool audio_service_clear_features(uint32_t features);


/* 获取当前的音频特性；
 * @return: 返回当前音频特性，参考@enum audio_feature_e
 */
uint32_t audio_service_get_features(void);

/* 设置编码器类型；
 * 此函数一般由audio库调用
 * @param type: 编码器类型，参考@enum audio_encoder_type_e
 */
void audio_service_set_encoder_type(uint32_t type);

/* 获取编码器类型；
 * @return: 返回当前编码器类型，参考@enum audio_encoder_type_e
 */
uint32_t audio_service_get_encoder_type(void);

/* 设置解码器类型；
 * 此函数一般由audio库调用
 * @param type: 解码器类型，参考@enum audio_decoder_type_e
 */
void audio_service_set_decoder_type(uint32_t type);


/* 获取解码器类型；
 * @return: 返回当前解码器类型，参考@enum audio_decoder_type_e
 */
uint32_t audio_service_get_decoder_type(void);

/*
 * 请求音频焦点（由audio库内部调用）
 */
int audio_service_request_audiofocus(uint32_t focus_holder);

/*
 * 释放音频焦点（由audio库内部调用）
 */
void audio_service_free_audiofocus(uint32_t holder);

/*
 * 卡拉OK退出
 */
void audio_service_karaoke_exit(void);

void audio_service_stop_karaoke(void);

void audio_service_start_karaoke(void);
void audio_service_stop_admomini(void);


/* 获取音频焦点拥有者；
 * @return: 返回音频焦点拥有者，含义参考 @enum audiofocus_holder_e
 */
uint32_t get_audiofocus_holder(void);

/* 淡出DAC输出 */
void audio_serivce_fadeout_dac_aout(uint32_t fadeout_ms);

/* 淡入DAC输出 */
void audio_serivce_fadein_dac_aout(uint32_t fadein_ms);

/* 获取播放时间戳（单位：微秒）;
 * 调用此接口时会返回初始化播放到现在的播放点数转换成时间（微秒）
 * @return: 返回时间戳
 */
int64_t get_play_timestamp(void);

/* 获取播放状态;
 * NOTE:此接口获取得到的状态是底层驱动的状态;
 * @return: 返回值含义参考 @enum pcm_hw_state
 */
uint8_t get_play_status(void);

/* USB audio 播放内部接口 */
void usb_audio_source_init(void);
void usb_audio_source_deinit(void);
void usb_audio_source_pull_data(void *buf, uint32_t samples);

/* USB audio 录音内部接口 */
void usb_audio_sink_init(void);
void usb_audio_sink_deinit(void);
void usb_audio_sink_push_data(void *interpbuf, void *buf,
    uint32_t samples, uint8_t capture_type);
void usb_audio_sink_update_capture_type(void);

/* linein播放内部接口 */
void linein_audio_source_init(void);
void linein_audio_source_deinit(void);
void linein_audio_source_push_data(void *buf, uint32_t samples);
void linein_audio_source_pull_data(void *buf, uint32_t samples);

/* 输入源转换成ADC channels mask
 * @param input: 输入源， 参考@enum audio_input_source_e;
 * @return: return ADC channels(bit0-ADC0, bit1-ADC1, bit2-ADC2) mask;
 *
 * Example:
 *      uint32_t amic_ch_mask;
 *      amic_ch_mask = audio_service_get_input_channels_mask(AUDIO_INPUT_AMIC2);
 *      switch (amic_ch_mask) {
 *      case 1: logi("channel 0"); break;
 *      case 2: logi("channel 1"); break;
 *      case 3: logi("channel 0 and channel 1"); break;
 *      case 4: logi("channel 2"); break;
 *      case 5: logi("channel 0 and channel 2"); break;
 *      case 6: logi("channel 1 and channel 2"); break;
 *      }
 */
uint32_t audio_service_get_input_channels_mask(enum audio_input_source_e input);

/* 设置输入源的模拟音量
 * @param input: audio input source, see @enum audio_input_source_e
 * @param gain: analog gain, range see "user_config.h 'analog gain range' ""
 *
 * Example:
 *      //set amic2 analog gain as 4
 *      audio_service_set_input_analog_gain(AUDIO_INPUT_AMIC2, 4);
 *      // set linein0 linein1 analog gain as 6
 *      audio_service_set_input_analog_gain(AUDIO_INPUT_LINEIN0_1, 6);
 */
void audio_service_set_input_analog_gain(enum audio_input_source_e input, uint32_t gain);

/* 设置输入源的数字音量
 * @param input: audio input source, see @enum audio_input_source_e;
 * @param gain: digital gain, range see "user_config.h 'digital gain' ""
 *
 * Example:
 *      //set amic2 digital gain as 0
 *      audio_service_set_input_digital_gain(AUDIO_INPUT_AMIC2, 0);
 *      //set linein0 linein1 digital gain as -6
 *      audio_service_set_input_digital_gain(AUDIO_INPUT_LINEIN0_1, -6);
 */
void audio_service_set_input_digital_gain(enum audio_input_source_e input, int32_t gain);

/* 打开输入源
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //enable amic2
 *      audio_service_enable_input_source(AUDIO_INPUT_AMIC2);
 *      //enable linein0 linein1
 *      audio_service_enable_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_enable_input_source(enum audio_input_source_e input);

/* 关闭输入源
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //disable amic2
 *      audio_service_disable_input_source(AUDIO_INPUT_AMIC2);
 *      //disable linein0 linein1
 *      audio_service_disable_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_disable_input_source(enum audio_input_source_e input);

/* 选择和打开mic(record, call)
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //select amic2 as mic(record, call) input
 *      audio_service_disable_input_source(AUDIO_INPUT_AMIC2);
 *      //select linein0 linein1 as mic(record, call) input
 *      audio_service_disable_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_set_mic_input_source(enum audio_input_source_e input);

/* 获取当前MIC输入
 * @return: return current mic input source
 *
 * Example:
 *      enum audio_input_source_e src;
 *      src = audio_service_get_mic_input_source();
 *      switch (src) {
 *      case AUDIO_INPUT_AMIC0: logi("current mic input is analog mic 0"); break;
 *      case AUDIO_INPUT_AMIC1: logi("current mic input is analog mic 1"); break;
 *      case AUDIO_INPUT_AMIC2: logi("current mic input is analog mic 2"); break;
 *      case AUDIO_INPUT_DMIC0F: logi("current mic input is digital mic0(DMICDATA0) falling edge data"); break;
 *      ...
 *      }
 */
enum audio_input_source_e audio_service_get_mic_input_source(void);

/* 选择和打开linein（模式）输入
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //select amic2 as linein(mode) input
 *      audio_service_set_linein_input_source(AUDIO_INPUT_AMIC2);
 *      //select linein0 linein1 as linein(mode) input
 *      audio_service_set_linein_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_set_linein_input_source(enum audio_input_source_e input);

/* 关闭linein（模式）输入
 * @return: return current linein input source
 *
 * Example:
 *      enum audio_input_source_e src;
 *      src = audio_service_get_linein_input_source();
 *      switch (src) {
 *      case AUDIO_INPUT_AMIC0: logi("current linein(mode) input is analog mic 0"); break;
 *      case AUDIO_INPUT_AMIC1: logi("current linein(mode) input is analog mic 1"); break;
 *      case AUDIO_INPUT_AMIC2: logi("current linein(mode) input is analog mic 2"); break;
 *      case AUDIO_INPUT_DMIC0F: logi("current linein(mode) input is digital mic0(DMICDATA0) falling edge data"); break;
 *      ...
 *      }
 */
enum audio_input_source_e audio_service_get_linein_input_source(void);


/* 发送一个命令给audio服务
 * @param cmd: audio_service command, see @enum audio_service_cmd_e
 * @param arg: audio_service command argument
 * @return: return zero on success, a negative errno value on failure
 *
 * Example:
 *    //enable duck
 *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 1);
 *    //disable duck
 *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 0);
 */
int audio_service_cmd(uint32_t cmd, uint32_t val);

/* 设置录音（MIC)音量
 * @param volume: record(mic) volume: 0 ~ 100
 *
 * Example:
 *    //set record(mic) volume as 50
 *    audio_service_set_record_volume(50);
 */
void audio_service_set_record_volume(uint8_t volume);

/* 获取录音（MIC)音量
 * @return: current record(mic) volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_record_volume();
 *    logi("current record(mic) volume is %d", vol);
 */
uint8_t audio_service_get_record_volume(void);

/* 设置播放（音乐）音量
 * @param volume: playback(music) volume: 0 ~ 100
 * NOTE: udisk/sdcard playback, a2dp playback, linein/fm/usbaudio/spdif playback will use this volume
 *
 * Example:
 *    //set playback volume as 50
 *    audio_service_set_playback_volume(50);
 */
void audio_service_set_playback_volume(uint8_t volume);

/*  获取播放（音乐）音量
 * @return: current playback(music) volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_playback_volume();
 *    logi("current playback volume is %d", vol);
 */
uint8_t audio_service_get_playback_volume(void);

/* 设置通话音量
 * @param volume: call volume: 0 ~ 100
 *
 * Example:
 *    //set call volume as 50
 *    audio_service_set_call_volume(50);
 */
void audio_service_set_call_volume(uint8_t volume);

/* 获取通话音量
 * @return: current call volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_call_volume();
 *    logi("current call volume is %d", vol);
 */
uint8_t audio_service_get_call_volume(void);


/* 设置卡拉OK MIC混音音量
 * @param volume: karaoke mic mix volume: 0 ~ 100
 *
 * Example:
 *    //set mic mix volume as 50
 *    audio_service_set_karaoke_mic_mix_volume(50);
 */
void audio_service_set_karaoke_mic_mix_volume(uint8_t volume);

/* 获取卡拉OK MIC混音音量
 * @return: karaoke mic mix volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_karaoke_mic_mix_volume();
 *    logi("current mic mix volume is %d", vol);
 */
uint8_t audio_service_get_karaoke_mic_mix_volume(void);


/* 设置卡拉OK MIC+MUSIC混音后的输出音量
 * @param volume: mix out volume: 0 ~ 100
 *
 * Example:
 *    //set mic mix out volume as 50
 *    audio_service_set_karaoke_mixout_volume(50);
 */
void audio_service_set_karaoke_mixout_volume(uint8_t volume);

/* 获取卡拉OK MIC+MUSIC混音后的输出音量
 * @return: current mix out volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_karaoke_mixout_volume();
 *    logi("current mic mix out volume is %d", vol);
 */
uint8_t audio_service_get_karaoke_mixout_volume(void);

/* 设置tone提示音音量
 * @param volume: tone volume: 0 ~ 100
 *
 * Example:
 *    //set mic tone volume as 50
 *    audio_service_set_tone_volume(50);
 */
void audio_service_set_tone_volume(uint8_t volume);

/* 获取tone提示音音量
 * @return: current tone volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_tone_volume();
 *    logi("current tone volume is %d", vol);
 */
uint8_t audio_service_get_tone_volume(void);

/* 播放正弦波提示音
 * @param tone_type: sine wave tone type, see @enum audio_sin_tone_type_e
 *
 * Example:
 *    //play volume limit sin tone
 *    audio_service_play_sin_tone(SIN_TONE_VOLUME_LIMIT);
 */
void audio_service_play_sin_tone(uint32_t tone_type);

/*
 * 本地/蓝牙音乐预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
void app_music_process(audio_stream_info_t *info);

/* 单独设置本地播放 蓝牙音乐音量
 * NOTE: 用于声卡方案本地/蓝牙音乐音量和linein， USB IN音量分开调节， 其他方案不需要调用此接口。
 * @param volume: local/bt music volume(0~100)
 *
 * Example:
 *    //set local/bt music volume as 50
 *    music_ctrl_set_volume(50);
 */
void music_ctrl_set_volume(uint8_t volume);

/* 获取单独设置本地播放 蓝牙音乐音量
 * @return: current local/bt music volume(0~100)
 * NOTE: 用于声卡方案获取本地/蓝牙音乐音量， 其他方案不需要调用此接口。
 *
 * Example:
 *    uint8_t vol = music_ctrl_get_volume();
 *    logi("current local/bt music volume is %d", vol);
 */
uint8_t music_ctrl_get_volume(void);

/* 单独mute/unmute本地播放 蓝牙音乐；
 * NOTE: 用于声卡方案， 其他方案不需要调用此接口；
 * @param mute: false: unmute local/bt music; true: mute local/bt music
 *
 * Example:
 *    //mute local/bt music
 *    music_ctrl_set_mute(true);
 *    //unmute local/bt music
 *    music_ctrl_set_mute(false);
 */
void music_ctrl_set_mute(bool mute);

/* 获取单独mute/unmute本地播放 蓝牙音乐状态
 * NOTE: 用于声卡方案， 其他方案不需要调用此接口；
 * @return: false: current is unmute; true: current is mute
 *
 * Example:
 *    bool muted = music_ctrl_get_mute();
 *    logi("current local/bt music is muted: %d", muted);
 */
bool music_ctrl_get_mute(void);


#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_SERVICE_H_ */

