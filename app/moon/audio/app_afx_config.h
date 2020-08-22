#ifndef APP_AFX_CONFIG_H
#define APP_AFX_CONFIG_H

#include "sys_inc.h"
#include "user_config.h"
#include "audio_effects.h"
#include "audio_service.h"

/***************************************************************************
                 音效内部设置参数
 ***************************************************************************/
/*
    0：移频算法使用CPU0跑
    1：移频算法使用CPU1跑
*/
#define AUDIO_AFX_FREQ_SHIFT_RUN_CPU1                   0
/*
    0：NHS算法使用CPU0跑
    1: NHS算法使用CPU1跑
*/
#define AUDIO_AFX_NHS_RUN_CPU1                          1
/*
    移频算法高低阶开关
    低阶RAM和CPU消耗较低
    高阶低频衰减程度小
    低阶够用了！
*/
#define AUDIO_AFX_FREQ_SHIFT_IMPROVE                0
/*
    播放一路音效左右通道输出后处理开关
*/
#define AUDIO_AFX_PLBK_POST_PROC_EN                 0
/*
    播放一路左右通道（输出1，输出2）高低通滤波器使能开关
*/
#define AUDIO_AFX_HLF_EN                            (1 && AUDIO_AFX_PLBK_POST_PROC_EN)
/*
    MIC一路fdn混响预延时开关
*/
#define AUDIO_AFX_REC_FDNREV_PREDLY_EN              0
/*
    播放一路fdn混响预延时开关
*/
#define AUDIO_AFX_PLBK_FDNREV_PREDLY_EN             0
/*
    MIC一路fdn混响半采样开关
*/
#define AUDIO_AFX_REC_FDNREV_SEMI_FS_EN             1
/*
    播放一路fdn混响半采样开关
*/
#define AUDIO_AFX_PLBK_FDNREV_SEMI_FS_EN            1
/*
    MIC一路fdn混响输入扩散开关
*/
#define AUDIO_AFX_REC_FDNREV_INPUT_DIFF_EN          0
/*
    播放一路fdn混响输入扩散开关
*/
#define AUDIO_AFX_PLBK_FDNREV_INPUT_DIFF_EN         0
/*
    MIC一路混响效果CUSTDOWN开关，减小RAM开销
*/
#define AUDIO_AFX_REC_FDNREV_CUSTDOWN               0
/*
    播放一路混响效果CUSTDOWN开关，减小RAM开销
*/
#define AUDIO_AFX_PLBK_FDNREV_CUSTDOWN              1
/*
    回声最大延时设置
*/
#define AUDIO_AFX_ECHO_MAX_DLY                      300
/*
    回声CUSTDOWN开关，减小RAM开销
*/
#define AUDIO_AFX_ECHO_CUSTDOWN                     1
/*
    录音变声pwola算法混叠长度( 320 ~ 480 ) */
#define AUDIO_AFX_REC_TSPS_PWOLA_LEN                360
/*
    播放变声pwola算法混叠长度( 320 ~ 480 ) */
#define AUDIO_AFX_PLBK_TSPS_PWOLA_LEN               280
/*
    变声formant开关
*/
#define AUDIO_AFX_REC_FORMANT_EN                    (0 && AUDIO_MIC_TSPS)
/*
    电音开关
*/
#define AUDIO_AFX_REC_AUTOTUNE_EN                   (0 && AUDIO_MIC_TSPS)
/*
    通话变调开关
*/
#define AUDIO_AFX_SCO_TSPS_EN                       0
/*
    MIC一路ECHO后置EQ开关
*/
#define AUDIO_AFX_POST_ECHO_EQ_EN                   (0 && AUDIO_MIC_ECHO)
/*
    MIC一路混响后置EQ开关
*/
#define AUDIO_AFX_POST_REVB_EQ_EN                   (0 && AUDIO_MIC_REVERB)
/*
    3D默认开，否则关
*/
#define AUDIO_AFX_VSS_DEFAULT_SWITCH                1
/*

    3D默认耳机模式，否则扬声器模式
*/
#define AUDIO_AFX_VSS_DEFAULT_HP_MODE               0
/*
    3D开关是否启用淡入淡出
*/
#define AUDIO_AFX_VSS_SWITCH_FADEINOUT              1
/*
    3D内部EQ开关
*/
#define AUDIO_AFX_VSS_EQ_ENABLE                     1
/*
    等响算法同步外部音量频率，0-7，大小与频率成反比
*/
#define AUDIO_AFX_TBVC_VOL_SYNC_PRIRY               7
/*
    AMIC0, AMIC1混音开关
*/
#define AUDIO_AFX_REC_AMIC01_MIX                    0
/*
    MIC一路的音效采用CPU0/1异步模式
*/
#define AUDIO_MIC_REC_CPUX_ASYNC                    (1 && AUDIO_KARAOKE_MODE && (AUDIO_MIC_TSPS || AUDIO_MIC_REVERB))
/*
    播放一路的音效采用CPU0/1异步模式
*/
#define AUDIO_PLBK_CPUX_ASYNC                       (1 && !AUDIO_KARAOKE_MODE && AUDIO_PLAYBACK_VSS)
/*
    高阶消原音算法使能
*/
#define BETTER_VOCAL_CUT                            (0)

/***************************************************************************
                 音效系统设置参数，客户勿动
 ***************************************************************************

    MIC一路的输入samples
*/
#define AFX_REC_IN_PUT_SIZE                         256
/*
    播放一路的输入samples
*/
#define AFX_PLBK_IN_PUT_SZIE                        256

#if (AFX_REC_IN_PUT_SIZE != 256)
#error "Do not edit the Mic input size!"
#endif
#if (AFX_PLBK_IN_PUT_SZIE != 256)
#error "Do not edit the Plbk input size!"
#endif
/*
    通话变调CPU频率
*/
#define AUDIO_AFX_SCO_TSPS_CPUFRQ                   ((uint32_t)96e6)

#define AUDIO_CPUX_ASYNC_BUF_SIZE (1 + (AUDIO_MIC_REC_CPUX_ASYNC + 2*AUDIO_PLBK_CPUX_ASYNC)*AFX_REC_IN_PUT_SIZE)

#if (AUDIO_PLAYBACK_SPECTRUM | AUDIO_PR_MIX_SPECTRUM)
#define SPECTRUM_BANDNUM    6
#define SPECTRUM_SAMPLE_CNT 100
extern uint8_t spec_buf[SPECTRUM_BANDNUM];
#endif

extern float *bands_for_spec;

extern afx_work_flow awf_plbk[];
extern afx_work_flow awf_rec[];

int32_t getPlbkAWFNum();
int32_t getRecAWFNum();
const void* freq_shift_lib_assign();
bool AsignfsRunningCpu1();
bool AsignNhsRunningCpu1();
bool IsThizStreamCpuxAsync(int8_t io);

/*
    音效app层接口测试使能
*/
#define EQ_AFX_API_TEST             0

#endif
