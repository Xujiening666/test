#include "app_afx_config.h"
#include "afx_other_settings/ignore/hlflt.c"

#define TEST_AFX_DEMO 0

#if TEST_AFX_DEMO
#define AFX_PROC_VC (0xFF<<MAIN_MASK_BIAS)
#define AFX_PROC_VC_CMD_SETRT  0xFFFFFFFF
#endif

afx_work_flow awf_plbk[] = {
#if TEST_AFX_DEMO
    {
        AFX_PROC_VC | AFX_PROC_TYPE_COMP_32BIT_INPUT,
        AFX_FLOW_MAJOR
    },
#endif
#if AUDIO_PLAYBACK_TSPS
    {
        AFX_PROC_TSPS | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_MUSIC,
        AFX_FLOW_MAJOR
    },
#endif

#if AUDIO_PLAYBACK_TBVC
    {
        AFX_PROC_TBVC | AFX_PROC_TYPE_COMP_32BIT_INPUT,
        AFX_FLOW_MAJOR
    },
#endif

#if AUDIO_PLAYBACK_EQ
    {
        AFX_PROC_EQ_SEC_14 | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_COMP_RUNIN_CPU0 | AFX_PROC_TYPE_PGE_MUSIC,
        AFX_FLOW_MAJOR
    },
#endif

#if AUDIO_PLAYBACK_DRC
    {
        AFX_PROC_DRC | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_MUSIC,
        AFX_FLOW_MAJOR
    },
#endif

#if AUDIO_PLAYBACK_VSS
    #if AUDIO_AFX_VSS_SWITCH_FADEINOUT
        {
            AFX_PROC_MIXER_BUFFER_SPLIT_STEREO32 | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_COMP_PRE_VSS,
            AFX_FLOW_SPLIT
        },
    #endif
    {
        AFX_PROC_VSS | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_VSS,
        AFX_FLOW_MAJOR
    },
    #if AUDIO_AFX_VSS_SWITCH_FADEINOUT
        {
            AFX_PROC_MIXER_CROSSFADE_STEREO32 | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_COMP_POST_VSS,
            AFX_FLOW_COMBINE
        },
    #endif
#endif

#if AUDIO_AFX_PLBK_POST_PROC_EN
    #if AUDIO_PLAYBACK_DR
        {
            AFX_PROC_DR | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_OUTPUT1 | AFX_PROC_TYPE_PGE_OUTPUT2,
            AFX_FLOW_MAJOR
        },
    #endif

    #if AUDIO_PLAYBACK_DRC
        {
            AFX_PROC_MIXER_STEREO32_INTERLEAVE_TO_NONINTERLEAVE,
            AFX_FLOW_MAJOR
        },
        {
            AFX_PROC_DRC | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_COMP_MONO_INPUT | AFX_PROC_TYPE_PGE_OUTPUT1,
            AFX_FLOW_MAJOR
        },
        {
            AFX_PROC_DRC | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_COMP_MONO_INPUT | AFX_PROC_TYPE_PGE_OUTPUT2,
            AFX_FLOW_MAJOR
        },
        {
            AFX_PROC_MIXER_STEREO32_NONINTERLEAVE_TO_INTERLEAVE,
            AFX_FLOW_MAJOR
        },
    #endif

    #if AUDIO_PLAYBACK_EQ
        {
            AFX_PROC_EQ_SEC_14 | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_OUTPUT1 | AFX_PROC_TYPE_PGE_OUTPUT2,
            AFX_FLOW_MAJOR
        },
    #endif
#endif
};

afx_work_flow awf_rec[] = {
#if AUDIO_KARAOKE_MODE
    {
        AFX_PROC_MIXER_MONO16_TO_MONO32,
        AFX_FLOW_MAJOR
    },

#if AUDIO_MIC_TSPS
    {
        AFX_PROC_TSPS | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_MIC,
        AFX_FLOW_MAJOR
    },
#if AUDIO_AFX_REC_FORMANT_EN
    {
        AFX_PROC_FC | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_MIC,
        AFX_FLOW_MAJOR
    },
#endif
#endif

#if AUDIO_MIC_EQ
    {
        AFX_PROC_EQ_SEC_14 | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_MIC,
        AFX_FLOW_MAJOR
    },
#endif

#if AUDIO_MIC_DRC
    {
        AFX_PROC_DRC | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_MIC,
        AFX_FLOW_MAJOR
    },
#endif

#if AUDIO_MIC_ECHO
    {
        AFX_PROC_ECHO | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_EFFECT,
        AFX_FLOW_MAJOR
    },

#if AUDIO_AFX_POST_ECHO_EQ_EN
    {
        AFX_PROC_EQ_SEC_5 | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_COMP_POST_ECHO | AFX_PROC_TYPE_PGE_EFFECT,
        AFX_FLOW_MAJOR
    },
#endif
#endif

#if AUDIO_MIC_REVERB
    {
        AFX_PROC_MIXER_MONO32_TO_STEREO16 | AFX_PROC_TYPE_COMP_PRE_REVB,
        AFX_FLOW_MAJOR
    },
    {
        AFX_PROC_REVERB | AFX_PROC_TYPE_COMP_FORMAT_DYNAMIC | AFX_PROC_TYPE_COMP_32BIT_INPUT | AFX_PROC_TYPE_PGE_EFFECT,
        AFX_FLOW_MAJOR
    },
#endif

    {
        AFX_PROC_MIXER_MONO32_TO_STEREO16 | AFX_PROC_TYPE_COMP_POST_REVB,
        AFX_FLOW_MAJOR
    },
#if AUDIO_MIC_REVERB
#if AUDIO_AFX_POST_REVB_EQ_EN
    {
        AFX_PROC_EQ_SEC_5 | AFX_PROC_TYPE_COMP_16BIT_INPUT | AFX_PROC_TYPE_COMP_STEREO_INPUT | AFX_PROC_TYPE_COMP_POST_REVB | AFX_PROC_TYPE_PGE_EFFECT,
        AFX_FLOW_MAJOR
    },
#endif
#endif
#endif
};

int32_t getPlbkAWFNum()
{
    return sizeof(awf_plbk)?sizeof(awf_plbk)/sizeof(awf_plbk[0]):0;
}

int32_t getRecAWFNum()
{
    return sizeof(awf_rec)?sizeof(awf_rec)/sizeof(awf_rec[0]):0;
}

uint8_t TBVC_VOL_SYNC_PRIRY = (1<<AUDIO_AFX_TBVC_VOL_SYNC_PRIRY);

bool AFX_FDNREV_PREDLY_EN[2] = {
    AUDIO_AFX_REC_FDNREV_PREDLY_EN,
    AUDIO_AFX_PLBK_FDNREV_PREDLY_EN
};

bool AFX_FDNREV_SEMI_FS_EN[2] = {
    AUDIO_AFX_REC_FDNREV_SEMI_FS_EN,
    AUDIO_AFX_PLBK_FDNREV_SEMI_FS_EN
};

bool AFX_FDNREV_INPUT_DIFF_EN[2] = {
    AUDIO_AFX_REC_FDNREV_INPUT_DIFF_EN,
    AUDIO_AFX_PLBK_FDNREV_INPUT_DIFF_EN
};

#if AUDIO_AFX_REC_FDNREV_CUSTDOWN

AT(.cache_seg)
__attribute__((weak))
float user_scale_rec = 0.5f;

#endif

#if AUDIO_AFX_PLBK_FDNREV_CUSTDOWN

AT(.cache_seg)
float user_scale_plbk = 0.5f;

#endif

uint32_t AFX_REC_TSPS_PWOLA_LEN = AUDIO_AFX_REC_TSPS_PWOLA_LEN;
uint32_t AFX_PLBK_TSPS_PWOLA_LEN = AUDIO_AFX_PLBK_TSPS_PWOLA_LEN;
bool AFX_REC_FORMANT_EN = AUDIO_AFX_REC_FORMANT_EN;
bool AFX_PLBK_POST_PROC = AUDIO_AFX_PLBK_POST_PROC_EN;
bool AFX_REC_AUTOTUNE_EN = AUDIO_AFX_REC_AUTOTUNE_EN;
bool AFX_VSS_EQ_ENABLE  = AUDIO_AFX_VSS_EQ_ENABLE;
bool AFX_HLF_EN = AUDIO_AFX_HLF_EN;
uint16_t AFX_ECHO_MAX_DLY = AUDIO_AFX_ECHO_MAX_DLY;
bool AFX_ECHO_SEMI_FS = AUDIO_AFX_ECHO_CUSTDOWN;
bool vss_default_switch  = AUDIO_AFX_VSS_DEFAULT_SWITCH;
bool vss_default_hp_mode = AUDIO_AFX_VSS_DEFAULT_HP_MODE;

#if (!AUDIO_AFX_REC_AMIC01_MIX)
bool amic01_mix_enable = false;
#else
bool amic01_mix_enable = true;
#endif

AT(.cache_seg)
__attribute__((aligned(4)))
int backup_buffer1[AUDIO_CPUX_ASYNC_BUF_SIZE];
AT(.cache_seg)
__attribute__((aligned(4)))
int backup_buffer2[AUDIO_CPUX_ASYNC_BUF_SIZE];

typedef struct
{
    /* real-time Np */
    int8_t Np;
    /* real-time Nf */
    int8_t Nf;
    /* absolute thd in dB */
    int8_t Tptpr;
    /* peak to averag thd in dB */
    int8_t Tpapr;
    /* peak to harmonic thd in dB */
    int8_t Tphpr;
    /* peak to neighbor thd in dB */
    int8_t Tpnpr;
    /* drop threshold in dB */
    int8_t Tdrop;
    /* high freq relax in phpr */
    int8_t Att;
    /* Test drop time in ms */
    uint16_t tdrop;
    /* history check length in ms */
    uint16_t thist;
    /* hold time after drop time expired */
    uint16_t thold;
    /* release threshold in dB */
    int8_t Trel;
    /* tone threshold in dB */
    float Ttone;
    /* decrese step in dB */
    int8_t Gdstep;
    /* gain floor */
    int8_t Gfloor;
    /* average time constant in ms */
    uint16_t tav;
    /* descend hold time in ms */
    uint16_t tdes;
    /* ascend hold time in ms */
    uint16_t taes;
    /* enter count time in ms */
    uint16_t tent;
    /* smooth time of filter coeffs in ms, 0~50 */
    uint16_t tsmooth;
}nhs_rtprms_t;

const nhs_rtprms_t app_nhs_init=
{
    /* real-time Np 频率极值点数，1~10 */
    10,
    /* real-time Nf 啸叫频点数目，2~12 */
    12,
    /* absolute thd in dBfs，静音门限 -60~-20 */
    -60,
    /* peak to averag thd in dB，啸叫频点与平均能量比值，10~50 */
    6,
    /* peak to harmonic thd in dB，啸叫频点与谐波能量比值，10~50 */
    26,
    /* peak to neighbor thd in dB，啸叫频点与邻域频点能量比值，1~15 */
    3,
    /* drop threshold in dB，啸叫测试阈值，-15~0 */
    -3,
    /* high freq relax in phpr，高频啸叫频点放松阈值，-20~0 */
    -15,
    /* Test drop time in ms，啸叫测试时间 10~500*/
    500,
    /* history check length in ms，啸叫持续时间 10~200 */
    200,
    /* hold time after drop time expired，啸叫锁住时间 10~200 */
    200,
    /* release threshold in dB，啸叫释放测试阈值，-30~-6 */
    -10,
    /* tone threshold in dB，单频判定阈值，-3~0 */
    -1,
    /* decrese step in dB，增益衰减步长，-12~-1 */
    -6,
    /* gain floor in dB，最小增益，-60~-6 */
    -30,
    /* average time constant in ms，能量平均时间常数，500~5000 */
    2000,
    /* descend hold time in ms，增益衰减锁住时间，50~300 */
    100,
    /* ascend hold time in ms，增益恢复锁住时间，50~300 */
    100,
    /* enter count time in ms，啸叫频点进入判定时间， 50~300 */
    100,
    /* smooth time of filter coeffs in ms, 陷波器系数平滑时间常数 0~50 */
    40
};

nhs_rtprms_t app_nhsrtprm=
{
    /* real-time Np 频率极值点数，1~10 */
    10,
    /* real-time Nf 啸叫频点数目，2~12 */
    12,
    /* absolute thd in dBfs，静音门限 -60~-20 */
    -60,
    /* peak to averag thd in dB，啸叫频点与平均能量比值，10~50 */
    6,
    /* peak to harmonic thd in dB，啸叫频点与谐波能量比值，10~50 */
    26,
    /* peak to neighbor thd in dB，啸叫频点与邻域频点能量比值，1~15 */
    3,
    /* drop threshold in dB，啸叫测试阈值，-15~0 */
    -3,
    /* high freq relax in phpr，高频啸叫频点放松阈值，-20~0 */
    -15,
    /* Test drop time in ms，啸叫测试时间 10~500*/
    500,
    /* history check length in ms，啸叫持续时间 10~200 */
    200,
    /* hold time after drop time expired，啸叫锁住时间 10~200 */
    200,
    /* release threshold in dB，啸叫释放测试阈值，-30~-6 */
    -10,
    /* tone threshold in dB，单频判定阈值，-3~0 */
    -1,
    /* decrese step in dB，增益衰减步长，-12~-1 */
    -6,
    /* gain floor in dB，最小增益，-60~-6 */
    -30,
    /* average time constant in ms，能量平均时间常数，500~5000 */
    2000,
    /* descend hold time in ms，增益衰减锁住时间，50~300 */
    100,
    /* ascend hold time in ms，增益恢复锁住时间，50~300 */
    100,
    /* enter count time in ms，啸叫频点进入判定时间， 50~300 */
    100,
    /* smooth time of filter coeffs in ms, 陷波器系数平滑时间常数 0~50 */
    40
};

typedef struct
{
    /* shift freq in Hz (0~10Hz) */
    int8_t deltafreq;
}fs_rtprm_t;

const fs_rtprm_t app_fs_init =
{
    /* 移频调制频率 */
    3,
};

fs_rtprm_t app_fsrtprm =
{
    /* 移频调制频率 */
    3,
};

#if (AUDIO_PLAYBACK_VSS)

typedef struct {
    /* rt60 in ms */
    int16_t revb_time;
    /* damping freq  for low freq in Hz */
    uint16_t damp_freq_lo;
    /* damping freq for high freq in Hz */
    uint16_t damp_freq_hi;
    /* output equalizer freq (lowpass) */
    uint16_t lpf_freq;
    /* output equalizer freq (high pass) */
    uint16_t hpf_freq;
    /* predelay in ms*/
    uint16_t pre_delay;
    /* diffusion */
    float diff;
    /* damping factor for low freq */
    float damp_ratio_lo;
    /* damping factor for high freq */
    float damp_ratio_hi;
    /* rev gain */
    float g_wet;
    /* dry gain */
    float g_dry;
    /* input diff flag */
    int8_t input_diff;
    /* programmable channel num */
    int8_t rt_ch;
    int8_t Qbits;
    bool buffer_install;
} fdnrev_rtprm_t;

AT(.cache_seg)
bool sdpro_input_diff_en  = (0 && AUDIO_AFX_PLBK_FDNREV_INPUT_DIFF_EN);
AT(.cache_seg)
bool sdpro_rev_semi_fs_en = (1 && AUDIO_AFX_PLBK_FDNREV_SEMI_FS_EN);
AT(.cache_rodata_seg)
const fdnrev_rtprm_t sd_fndrev_rt_init = {
    2750,//revb_time;
    300,//damp_freq_lo;
    5600,//damp_freq_hi;
    10000,//lpf_freq;
    4,//hpf_freq;
    0,//predelay in ms
    0.6,//diff;
    0.7,//damp_ratio_lo;
    0.5,//damp_ratio_hi;
    1.0,//g_wet;
    0.0,//g_dry;
    1,//input diff flag,
    2,//process chan num
    31,
    true,
};
#endif

#if (AUDIO_AFX_SCO_TSPS_EN)

typedef struct {
    float alpha_ts;
    float alpha_ps;
    int8_t buffer_install;
} tsps_rtprm_t;

tsps_rtprm_t sco_tsps_rt_init = {
    1.0,
    1.4,
    true
};

#endif

#if (AUDIO_PLAYBACK_SPECTRUM | AUDIO_PR_MIX_SPECTRUM)
#define SPECTRUM_BANDNUM    6
#define SPECTRUM_SAMPLE_CNT 100
AT(.cache_seg)
uint8_t spec_buf[SPECTRUM_BANDNUM];  //range: [0~31]
AT(.cache_seg)
float bands[SPECTRUM_BANDNUM] = {
    31.5,
    126.0,
    504.0,
    1008.0,
    4032.0,
    16128.0
};
AT(.cache_seg)
float *bands_for_spec = bands;
#else
AT(.cache_seg)
float *bands_for_spec = NULL;
#endif

/*
        the spec value should be 0 ~ 30
        we now scale (0, 30) to (0, 128) for led display
*/
AT(.cache_seg)
float SCALE_FACTOR = (float)4.2666666666666666666666666666667;

#if (AUDIO_KARAOKE_MODE && AUDIO_KARAOKE_USE_FS)

#if AUDIO_AFX_FREQ_SHIFT_IMPROVE

extern const void *freq_shift_high_lib;
AT(.com_seg)
const void *freq_shift_normal_lib = NULL;

#else

extern const void *freq_shift_normal_lib;
AT(.com_seg)
const void *freq_shift_high_lib = NULL;

#endif

#else

AT(.com_seg)
const void *freq_shift_high_lib = NULL;
AT(.com_seg)
const void *freq_shift_normal_lib = NULL;

#endif

const void* freq_shift_lib_assign()
{
    if(freq_shift_normal_lib)
        return freq_shift_normal_lib;
    if(freq_shift_high_lib)
        return freq_shift_high_lib;
    return NULL;
}

#define FS_CPUX_MASK_BIAS  0
#define NHS_CPUX_MASK_BIAS 1

uint32_t PreProcCpuxMask = ((!!AUDIO_AFX_FREQ_SHIFT_RUN_CPU1) << FS_CPUX_MASK_BIAS) |
                        ((!!AUDIO_AFX_NHS_RUN_CPU1) << NHS_CPUX_MASK_BIAS);

AT(.audio_effects_seg)
bool AsignfsRunningCpu1()
{
    return !!(PreProcCpuxMask & (1 << FS_CPUX_MASK_BIAS));
}

AT(.audio_effects_seg)
bool AsignNhsRunningCpu1()
{
    return !!(PreProcCpuxMask & (1 << NHS_CPUX_MASK_BIAS));
}

bool IsThizStreamCpuxAsync(int8_t io)
{
    return io?AUDIO_PLBK_CPUX_ASYNC:AUDIO_MIC_REC_CPUX_ASYNC;
}

#if TEST_AFX_DEMO
//----- 示例音效实现代码 ----- start
typedef struct _vol_ctl_init {
    int8_t Q_bits;
    int8_t ch;
}vol_ctl_init;

typedef struct _vol_ctl {
    int8_t Q_bits;
    int8_t ch;
    float  amp;
}vol_ctl;

typedef struct _vol_ctl_rt {
    float db;
}vol_ctl_rt;

extern void *general_malloc(uint32_t size);
extern void *general_calloc(uint32_t size);
extern void general_free(void *ptr);

void* vol_ctl_open(vol_ctl_init* para)
{
    vol_ctl* vc = general_malloc(sizeof(vol_ctl));
    if(vc) {
        vc->Q_bits = para->Q_bits;
        vc->ch     = para->ch;
        vc->amp    = 1.0f;
    }
    return vc;
}

AT(.audio_effects_seg)
void* vol_ctl_proc(void* handle, int32_t *in, int32_t* out, int32_t samples)
{
    vol_ctl* vc = (vol_ctl*)handle;
    int8_t ch   = vc->ch;
    float  f    = vc->amp;

    for(int s = 0; s < samples; s++) {
        for(int c = 0; c < ch; c++) {
            out[ch*s + c] = (int32_t)((float)in[ch*s + c]*f);
        }
    }
    return vc;
}

int32_t vol_ctl_cmd(void* handle, uint32_t cmd, uint32_t val)
{
extern float sp_pow2(float x);
    vol_ctl* vc = (vol_ctl*)handle;
    switch(cmd) {
        case AFX_PROC_VC_CMD_SETRT:
        {
            vol_ctl_rt* rtprm = (vol_ctl_rt*)val;
            vc->amp = sp_pow2(3.3219280948873623478703194294894f *rtprm->db/20.0f);
            break;
        }
    }
    return 0;
}

void vol_ctl_close(void* handle)
{
    vol_ctl* vc = (vol_ctl*)handle;
    general_free(vc);
}

//----- 示例音效实现代码 ----- end

//----- 示例音效外部控制代码 ----- start

//实时参数结构体
typedef struct _vol_ctl_setting {
    //变化标志位，标志内部实时参数结构体有变化，可以load到音效模块中
    int8_t onChange;
    vol_ctl_rt vcr;
}vol_ctl_setting;

//初始化实时参数结构体
void vc_para_init(void* cookie)
{
    vol_ctl_setting* SettingPara = (vol_ctl_setting*)cookie;
    SettingPara->vcr.db = 0.0f;
}

//实时参数结构体有改变，则加载到音效内部
void vc_para_load(afx_hook_defines* node, bool refreshCpuLoad)
{
    vol_ctl_setting *vcs = (vol_ctl_setting*)node->SettingPara;
    //状态监测
    if(vcs->onChange) {
        //锁定工作流
        uint32_t prior = 0;
        node->listener(node->user, AFX_CMD_LOCK_WORK_FLOW, (uint32_t)&prior);

        //设置实现
        node->cmd(node->handle, AFX_PROC_VC_CMD_SETRT, (uint32_t)&vcs->vcr);

        //锁定结束
        node->listener(node->user, AFX_CMD_UNLOCK_WORK_FLOW, (uint32_t)&prior);
        if(refreshCpuLoad) {
            node->listener(node->user, AFX_CMD_REFRESH_CPU_LOAD, 0);
        }
        //状态监测归位
        vcs->onChange = 0;
    }
}

//强制刷新实时参数结构体状态，强制加载到音效内部
void vc_para_refresh(void* SettingPara)
{
    vol_ctl_setting *vcs = (vol_ctl_setting*)SettingPara;
    vcs->onChange = 1;
}
#endif
/*
    根据你新添加的Owner ID， 音效的参数，打开额外的音效
    系统自带音效遍历完毕，就会走到这里
    参数：
    owner ：            Owner ID
*/
void afx_open_callback(afx_hook_defines* ahd, afx_chain_init_params_t *chain_params)
{
#if TEST_AFX_DEMO
/*
    Dummy code:
*/
    uint32_t main_type = ahd->owner & AFX_PROC_MAIN_TYPE_MASK;
    __maybe_unused uint32_t comp_type = ahd->owner & AFX_PROC_TYPE_COMP_TYPE_MASK;

    if(main_type == AFX_PROC_VC) {
        vol_ctl_init vci = {0};
        vci.Q_bits = 8*chain_params->sample_size - 1;
        vci.ch     = chain_params->ch;
        ahd->handle = vol_ctl_open(&vci);

        if(ahd->handle) {
            ahd->hook   = (afx_hook_node_proc)vol_ctl_proc;
            ahd->cmd    = (afx_hook_node_cmd)vol_ctl_cmd;
            ahd->close  = (afx_hook_node_close)vol_ctl_close;
            ahd->cpu_load = 10;
        }
    }
#endif
}
/*
    根据你新添加的Owner ID， 给出设置域结构体地址
    参数：
    owner ：            Owner ID
*/
void* getPrivSettingParaCallback(afx_hook_node_proc_types owner)
{
#if TEST_AFX_DEMO
/*
    Demo code:
*/
    uint32_t main_type = owner & AFX_PROC_MAIN_TYPE_MASK;
    void*    ret = 0;

    if(main_type == AFX_PROC_VC) {
        ret = general_calloc(sizeof(vol_ctl_setting));
    }
    return ret;
#else
    return NULL;
#endif
}

/*
    根据你新添加的Owner ID， 给出设置域结构体初始化函数
    参数：
    owner ：            Owner ID
*/
afx_hook_node_para_init getPrivSettingParaInitHookCallback(afx_hook_node_proc_types owner)
{
#if TEST_AFX_DEMO
/*
    Demo code:
*/
    uint32_t main_type = owner & AFX_PROC_MAIN_TYPE_MASK;
    afx_hook_node_para_init ret = 0;

    if(main_type == AFX_PROC_VC) {
        ret = vc_para_init;
    }

    return ret;
#else
    return NULL;
#endif
}
/*
    根据你新添加的Owner ID， 给出设置域结构体同步到底层的函数
    参数：
    owner ：            Owner ID
*/
afx_hook_node_para_load getPrivSettingParaLoadingHookCallback(afx_hook_node_proc_types owner)
{
#if TEST_AFX_DEMO
/*
    Demo code:
*/
    uint32_t main_type = owner & AFX_PROC_MAIN_TYPE_MASK;
    afx_hook_node_para_load ret = 0;

    if(main_type == AFX_PROC_VC) {
        ret = vc_para_load;
    }

    return ret;
#else
    return NULL;
#endif
}

afx_hook_node_para_refresh getPrivSettingParaRefreshHookCallback(afx_hook_node_proc_types owner)
{
#if TEST_AFX_DEMO
/*
    Demo code:
*/
    uint32_t main_type = owner & AFX_PROC_MAIN_TYPE_MASK;
    afx_hook_node_para_refresh ret = 0;

    if(main_type == AFX_PROC_VC) {
        ret = vc_para_refresh;
    }

    return ret;
#else
    return NULL;
#endif
}
/*
    新增PLAYBACK音效参数设置示例

    AFX_CHAIN_CMD_UNIVERSE_SETTING
    当你想通过指令协议去改变，同步音效实时参数时，你需要把指令协议放在buffer里面，再把buffer地址传下去，
    由底层去解析（前提是底层支持解析，底层支持解析的cmd见 TypeClass 定义）

    如果你想直接改变参数，底层只需同步时，传空的buffer即可
*/
#if TEST_AFX_DEMO
/*
    DEMO code
*/
int setVolCtl(float db)
{
    void *chain = audio_service_get_playback_afx_chain();

    afx_hook_defines* ahd = getAfxProcNodeForOwner(chain, AFX_PROC_VC, false);
    vol_ctl_setting*   settingPara = (vol_ctl_setting*)ahd->SettingPara;

    settingPara->vcr.db   = db;
    settingPara->onChange = 1;

    afx_chain_cmd(chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, 0);
    return 0;
}
#endif
