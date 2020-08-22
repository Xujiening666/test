/* The Alps Open Source Project
 * Copyright (c) 2016 - 2017 Smartlink Technology Co. Ltd. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _AUDIO_EFFECTS_H_
#define _AUDIO_EFFECTS_H_

typedef uint32_t PageClass;
#define NEXT_PAGE(p) ( p + 1 )
#define THIZ_PAGE(p) ( p )

#define PAGE_RESET        0x000000FE
#define PAGE_RESET_WEAK   0x000000FD

#define PAGE_NONE    0xFFFFFFFF
#define PAGE_START   (0)

#define PAGE_MUSIC   THIZ_PAGE(PAGE_START)
#define PAGE_EFFECT  NEXT_PAGE(PAGE_MUSIC)
#define PAGE_MIC     NEXT_PAGE(PAGE_EFFECT)
#define PAGE_OUTPUT1 NEXT_PAGE(PAGE_MIC)
#define PAGE_OUTPUT2 NEXT_PAGE(PAGE_OUTPUT1)
#define PAGE_VSS     NEXT_PAGE(PAGE_OUTPUT2)
#define PAGE_PLG     NEXT_PAGE(PAGE_VSS)

#define PAGE_END     THIZ_PAGE(PAGE_PLG)
#define PAGE_MAX     8

//Check page num...
#if ( PAGE_END >= PAGE_MAX )
#error "pageID overflow..."
#endif
typedef void (*afx_listener)(void* cookie, uint32_t cmd, uint32_t arg);

typedef struct afx_chain_s afx_chain_t;
typedef struct _afx_hook_defines afx_hook_defines;

typedef int32_t (*afx_tail_callback_t)(void *handle,
    int32_t *inbuf, int32_t insamples);

#define ROM_FUNC_PTR_HOOK_USE(func) ((uint32_t)func | 1)

typedef int32_t (*afx_hook_node_proc)(void *handle, int32_t* inb, int32_t* outb, int32_t samples);
typedef int32_t (*afx_hook_node_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void (*afx_hook_node_close)(void *handle);
typedef afx_listener afx_hook_node_listener;
typedef void (*afx_hook_node_para_init)(void* cookie);
typedef void (*afx_hook_node_para_load)(afx_hook_defines* node, bool refreshCpuLoad);
typedef void (*afx_hook_node_para_refresh)(void* SettingPara);

typedef uint32_t afx_hook_node_proc_types;

typedef enum {
    /*
            Means inbuf1 -> inbuf1
      */
    AFX_FLOW_MAJOR = 0,
    /*
            Means inbuf1 -> inbuf2
      */
    AFX_FLOW_SPLIT,
    /*
            Means inbuf2 -> inbuf2
      */
    AFX_FLOW_MINOR,
    /*
            Means inbuf1 + inbuf2 -> inbuf1
      */
    AFX_FLOW_COMBINE
}afx_hook_node_work_mode;

typedef enum {
    /*
            Runnig state
      */
    AFX_HOOK_NODE_STU_WORKING = 0,
    /*
            Idle state
      */
    AFX_HOOK_NODE_STU_IDLE,
}afx_hook_node_stu;

//Max support 256 non-excluded cases
#define MAIN_MASK_BIAS  24
typedef enum {
    AFX_PROC_NONE                                       = 0,

    AFX_PROC_MASS_EFFECTS_START                         = 0x01 << MAIN_MASK_BIAS,

    AFX_PROC_EQ_SEC_14                                  = AFX_PROC_MASS_EFFECTS_START, //Rom eq handle
    AFX_PROC_EQ_SEC_5                                   = 0x02 << MAIN_MASK_BIAS, //eq2 handle
    AFX_PROC_DRC                                        = 0x03 << MAIN_MASK_BIAS,
    AFX_PROC_ECHO                                       = 0x04 << MAIN_MASK_BIAS,
    AFX_PROC_REVERB                                     = 0x05 << MAIN_MASK_BIAS,
    AFX_PROC_TSPS                                       = 0x06 << MAIN_MASK_BIAS,
    AFX_PROC_FC                                         = 0x07 << MAIN_MASK_BIAS,
    AFX_PROC_VSS                                        = 0x08 << MAIN_MASK_BIAS,
    AFX_PROC_DR                                         = 0x09 << MAIN_MASK_BIAS,
    AFX_PROC_TBVC                                       = 0x0A << MAIN_MASK_BIAS,

    AFX_PROC_MASS_EFFECTS_END                           = AFX_PROC_TBVC,

    AFX_PROC_MIXER_MONO16_TO_MONO32                     = 0x0B << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_BUFFER_SPLIT_MONO32                  = 0x0C << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_BUFFER_COMBINE_MONO32                = 0x0D << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_STEREO32_INTERLEAVE_TO_NONINTERLEAVE = 0x0E << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_STEREO32_NONINTERLEAVE_TO_INTERLEAVE = 0x0F << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_MONO32_TO_STEREO16                   = 0x10 << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_CROSSFADE_STEREO32                   = 0x11 << MAIN_MASK_BIAS,
    AFX_PROC_MIXER_BUFFER_SPLIT_STEREO32                = 0x12 << MAIN_MASK_BIAS,
/*
    Todo :
    AFX_PROC_MIXER_MONO32_TO_MONO16,
    AFX_PROC_MIXER_STEREO16_TO_MONO32,
    AFX_PROC_SRATE_CNV_SEMI_DEC,
    AFX_PROC_SRATE_CNV_DOUBLE_INP
*/
    AFX_PROC_MAIN_TYPE_MASS_EFFECTS                     = 0xFF << MAIN_MASK_BIAS,
    AFX_PROC_MAIN_TYPE_MASK                             = 0xFF << MAIN_MASK_BIAS
}afx_hook_node_proc_main_types;

//Max support 16 excluded cases
#define COMP_MASK_BIAS 8
typedef enum
{
    AFX_PROC_TYPE_COMP_NONE              = 0,
    AFX_PROC_TYPE_COMP_PRE_REVB          = (1 << 0) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_POST_REVB         = (1 << 1) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_POST_ECHO         = (1 << 2) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_PRE_VSS           = (1 << 3) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_POST_VSS          = (1 << 4) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_HALF_SRATE        = (1 << 5) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_MONO_INPUT        = (1 << 6) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_STEREO_INPUT      = (1 << 7) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_16BIT_INPUT       = (1 << 8) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_32BIT_INPUT       = (1 << 9) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_FORMAT_DYNAMIC    = (1 << 10) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_RUNIN_CPU0        = (1 << 11) << COMP_MASK_BIAS,
    AFX_PROC_TYPE_COMP_TYPE_MASK         = 0xFFFF   << COMP_MASK_BIAS
}afx_hook_node_proc_compliment_type;

//Max support 8 excluded cases
#define PGE_MASK_BIAS  0
typedef enum
{
    AFX_PROC_TYPE_PGE_NONE               = 0,
    AFX_PROC_TYPE_PGE_MUSIC              = (1 << PAGE_MUSIC)   << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_EFFECT             = (1 << PAGE_EFFECT)  << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_MIC                = (1 << PAGE_MIC)     << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_OUTPUT1            = (1 << PAGE_OUTPUT1) << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_OUTPUT2            = (1 << PAGE_OUTPUT2) << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_VSS                = (1 << PAGE_VSS)     << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_HW                 = (1 << PAGE_PLG)      << PGE_MASK_BIAS,
    AFX_PROC_TYPE_PGE_TYPE_MASK          = 0xFF                << PGE_MASK_BIAS
}afx_hook_node_proc_page_type;

static inline bool PassForSubSetsMask(uint32_t dstMask, uint32_t srcMask)
{
    bool ret = false;
    //Condition : dstMask <= srcMask, special case : dstMask = srcMask = 0
    if((dstMask & srcMask) == dstMask) {
        ret = true;
    }
    return ret;
}

typedef struct
{
    uint32_t request;
    uint32_t complimentary;
    uint32_t respose;
}afx_interact_packet;

typedef void* (*get_afx_chain_hook)(void);
typedef struct {
    uint8_t            page_id;
    get_afx_chain_hook hook;
}pageid_to_afx_hook_map;

typedef struct {
    afx_hook_node_proc_types      apt;
    afx_hook_node_work_mode       ahnt;
}afx_work_flow;

typedef enum
{
    AFX_CMD_NONE,
    AFX_CMD_LOCK_WORK_FLOW,
    AFX_CMD_UNLOCK_WORK_FLOW,
    AFX_CMD_DISABLE_WORK_FLOW_NODE,
    AFX_CMD_ENABLE_WORK_FLOW_NODE,
    AFX_CMD_REFRESH_WORK_FLOW_NODE,
    AFX_CMD_UPLOAD_WORK_FLOW_NODE_CPU_LOAD,
    AFX_CMD_REFRESH_CPU_LOAD,
    AFX_CMD_QUERY_CPU1_LOAD,
    AFX_CMD_GET_WORK_FLOW_NODE_CPU_LOAD,
    AFX_CMD_QUERY_MASS_EFFECTS,
    AFX_CMD_QUERY_AUTOTUNE_STATUS
}afx_cmd;

struct _afx_hook_defines{
    uint8_t                    type;
    uint8_t                    stu;
    uint8_t                    doNoReset;
    uint16_t                   cpu_load;

    afx_hook_node_proc_types   owner;

    void*                      handle;
    void*                      SettingPara;

    void*                      user;
    afx_hook_node_listener     listener;

    afx_hook_node_para_init    initPara;
    afx_hook_node_para_load    loadPara;
    afx_hook_node_para_refresh refreshPara;
    /*
        Due to the difference of opening process, open hook has it own impliment
      */
    afx_hook_node_proc         hook;
    afx_hook_node_cmd          cmd;
    afx_hook_node_close        close;

    afx_hook_defines*          next;
};

typedef void (*user_setting_callback)(void* cookie, uint8_t arg);

struct afx_chain_s {
    afx_tail_callback_t tail_callback;
    void *tail_handle;

    uint16_t cpu_load;
    bool     need_set_cpux;

    bool use_dual_cpu;
    volatile bool output_ack;

    int8_t io;
    int8_t first_frame;

    int8_t afx_reset;
    afx_hook_defines* ahd;
    void*  minor_buffer;

    SemaphoreHandle_t cmd_sem_sync;
    bool   cpux_async;
};

afx_hook_defines *getAfxProcNodeForOwner(afx_chain_t* chain, afx_hook_node_proc_types owner, bool scanCompType);

//eq setting areas start
/*
 offset       name               desc
    0    |    pageId        |   页面索引
    1    |    type           |   类型
    2    |     itemIndex   |    一个类型多个项的 一个索引
    3    |     reserve       |
    4    |    data           |    数据
    5    |
    6    |
    7    |
*/
#define PAGE_ID_OFFSET      0
#define TYPE_OFFSET         1
#define ITEM_INDEX_OFFSET   2

#define SUB_PAGE_ID_OFFSET  3
#define SUB_PAGE_EQ_PROCESSOR_BIT_OFFSET 0
#define SUB_PAGE_EQ_PROCESSOR_BIT_RANGE  1

#define DATA_OFFSET         4
#define BLOCK_SZIE          8
#define GET_F32_VAL(ptr)    (*((float*)(ptr)))
#define GET_S16_FOR_F32(f)  ((int16_t)((int32_t)f))
#define GET_U16_FOR_F32(f)  ((uint16_t)((int32_t)f))
#define GET_U32_FOR_F32(f)  ((uint32_t)f)
#define GET_S32_FOR_F32(f)  ((int32_t)f)
#define GET_S8_FOR_F32(f)   ((int8_t)((int32_t)f))
#define GET_U8_FOR_F32(f)   ((int8_t)((int32_t)f))

#define MAX_BLOCK_PER_PAGE (64)

typedef union _eq_data_type {
    user_setting_callback  clbk;
    float                  data;//float
    uint32_t               ctx;
}eq_data_type;

typedef struct _eq_data {
    uint8_t      page_id;
    uint8_t      item_type;
    uint8_t      item_index;
    uint8_t      reserve;
    eq_data_type fill;
}eq_data;

#define PLBK_CTL  0x01
#define REC_CTL   0x00

#define REC_PAGE(page_id)  (PAGE_MIC == page_id || PAGE_EFFECT == page_id || PAGE_PLG == page_id)
#define PLBK_PAGE(page_id) (PAGE_MUSIC == page_id || PAGE_OUTPUT1 == page_id || \
                            PAGE_OUTPUT2 == page_id || PAGE_VSS == page_id)
#define PAGE_UNKNOWN (-1)

#define MUSIC_OR_MIC_EQ(p) (PAGE_MUSIC == p || PAGE_MIC == p)
#define POST_PROC_EQ(p)    (PAGE_OUTPUT1 == p || PAGE_OUTPUT2 == p)

#define AFX_MAX(A,B) (A>B?A:B)

#define P_MAX_CHANNEL 2
#define R_MAX_CHANNEL 1

#define PR_MAX_CHANNEL AFX_MAX(P_MAX_CHANNEL, R_MAX_CHANNEL)

#define RETURN_REAL_OFFSET_FOR_IO(offset, io) (offset*(io?P_MAX_CHANNEL:R_MAX_CHANNEL))

typedef enum _chan_map
{
    CHANNEL_MAP_LEFT = 0,
    CHANNEL_MAP_RIGHT,
    CHANNEL_MAX
}chan_map;

typedef enum {
    LEFT_CHANNEL = 0,
    RIGHT_CHANNEL,
    MAX_EQ_CHANNELS
}EQ_CHANNEL_TYPE;

typedef enum{
    REC = 0,
    PLBK,
    RP_ALL,
    MAX_STREAM_IO = RP_ALL
}STREAM_IO_TYPE;

typedef struct {
    uint8_t size;
    uint16_t offset;
}data_typesize_and_offset;

typedef struct{
    uint8_t   cmd_type;
    uint8_t   para_idx;
    //data_type dt;
}cmd_type2para_idx;

typedef struct{
    uint8_t   cmd_type;
    uint8_t   para_idx[2];
}cmd_type2doublepara_idx;

#define BIQUAD_LOWSHELVING  0
#define BIQUAD_PEAKNOTCH    1
#define BIQUAD_HIGHSHELVING 2
#define BIQUAD_LOWPASS      3
#define BIQUAD_BANDPASS     4
#define BIQUAD_HIGHPASS     5
#define BIQUAD_START        BIQUAD_LOWSHELVING
#define BIQUAD_END          BIQUAD_HIGHPASS

#define APF2ZF_DFTYPE_NONE       0xffff

#define APF2ZF_DFTYPE_LPF        BIQUAD_LOWPASS
/* highpass filter */
#define APF2ZF_DFTYPE_HPF        BIQUAD_HIGHPASS

/* analog filter prototype type defines */
/* 1st order prototype */
#define APF2ZF_APFTYPE_1STORDER 1
/* butter worth filter */
#define APF2ZF_APFTYPE_BUTTER   2
/* bessel filter */
#define APF2ZF_APFTYPE_BESSEL   3
/* Linkwitz-Riley filter */
#define APF2ZF_APFTYPE_LINK     4

/* filter slope defines */
/* 6 dB/oct */
#define APF2ZF_SLOPE_6DBOCT     1
/* 12 dB/oct */
#define APF2ZF_SLOPE_12DBOCT    2
/* 18 dB/oct */
#define APF2ZF_SLOPE_18DBOCT    3
/* 24 dB/oct */
#define APF2ZF_SLOPE_24DBOCT    4

#define APF2ZF_SECT_NUM(slope) ((slope+1)/2)

#define APF2ZF(dtype, apftype, slope) (( (dtype<<8)&0xf00 ) | ((apftype<<4)&0xf0) | ((slope&0xf)))

#define PROFESSION_LOW_PASS_BAND_BASE  10
#define PROFESSION_HIGH_PASS_BAND_BASE 12

#define MAX_PRESET_BANDS 10
#define MUSIC_TYPE_NUM   6


#define COMMON_PARAS_MASK(para) ( 1 << para )
#define COMMON_PARAS_MASK_EXCLUDE(para) (~( 1 << para ))

typedef enum {
    EQ_BAND_1 = 0,
    EQ_BAND_2,
    EQ_BAND_3,
    EQ_BAND_4,
    EQ_BAND_5,
    MAX_EQ5_BANDS,
    EQ_BAND_6 = MAX_EQ5_BANDS,
    EQ_BAND_7,
    EQ_BAND_8,
    EQ_BAND_9,
    EQ_BAND_10,
    EQ_BAND_11,
    EQ_BAND_12,
    EQ_BAND_13,
    EQ_BAND_14,
    MAX_EQ_BANDS,
    EQ_SUBBAND_NUM = MAX_EQ_BANDS
}EQ_SUBBAND;

typedef enum {
    EQ_PARA_START = 0,
    EQ_FILTER_TYPE = EQ_PARA_START,
    EQ_FC,
    EQ_GAIN,
    EQ_Q_FACTOR,
    EQ_DISABLE,
    EQ_PARA_END = EQ_DISABLE,
    EQ_SBPARA_NUM
}EQ_SUBBAND_PARAS;

#define EQ_SB_ALL_PAPAS_MASK   ( (1 << EQ_SBPARA_NUM) - 1 )

typedef enum {
    EQ_PREAMP_MAIN_FUNC = 0,
    EQ_PREAMP_SUBFUNC_NUM
}EQ_PREAMP_SUBFUNC;

typedef enum {
    EQ_PREAMP_SBPARA_GAIN = 0,
    EQ_PREAMP_SBPARA_NUM
}EQ_PREAMP_SBPARA;

typedef enum {
    DRC_LIMITER = 0,
    DRC_COMPRESSOR,
    DRC_EXPANDOR,
    DRC_NOSIEGATE,
    DRC_SUBFUNC_NUM
}DRC_SUBFUNC;

typedef enum {
    DRC_SFPARA_START = 0,
    DRC_SFPARA_THD = DRC_SFPARA_START,
    DRC_SFPARA_ATT,
    DRC_SFPARA_RATIO,
    DRC_SFPARA_REL,
    DRC_SFPARA_SWITCH,
    DRC_SFPARA_END = DRC_SFPARA_SWITCH,
    DRC_SFPARA_NUM
}DRC_SUBFUNC_PARAS;

#define DRC_SUBFUNC_ALL_MASK  ( (1 << DRC_SFPARA_NUM) - 1 )

typedef enum {
    VSS_MAIN,
    VSS_LEFT_EQ,
    VSS_RIGHT_EQ,
    VSS_DRC,
    VSS_SUB_CMD_NUM
}VSS_SUB_CMD;

typedef enum {
    DR_MAIN_FUNC = 0,
    DR_SUBFUNC_NUM
}DR_SUBFUNC;

typedef enum {
    DR_REV_FACTOR = 0,
    DR_RTDLY,
    DR_PARA_NUM
}DR_PARAS;

typedef enum {
    TSPS_MAIN_FUNC = 0,
    TSPS_SUBFUNC_NUM
}TSPS_SUBFUNC;

typedef enum {
    TSPS_PARA_START = 0,
    TSPS_TS = TSPS_PARA_START,
    TSPS_PS,
    TSPS_ENABLE,
    TSPS_PARA_END = TSPS_ENABLE,
    TSPS_PARA_NUM,
}TSPS_PARAS;

#define TSPS_ALL_PAPAS_MASK   ( (1 << TSPS_PARA_NUM) - 1 )

typedef enum {
    ATUNE_MAIN_FUNC = 0,
    ATUNE_SUBFUNC_NUM
}ATUNE_SUBFUNC;

typedef enum {
    ATUNE_PARA_START = 0,
    ATUNE_NOTE = ATUNE_PARA_START,
    ATUNE_OCT,
    ATUNE_SCALE,
    ATUNE_SPEED,
    ATUNE_ENABLE,
    ATUNE_PARA_END = ATUNE_ENABLE,
    ATUNE_PARA_NUM,
}ATUNE_PARAS;

#define ATUNE_ALL_PAPAS_MASK   ( (1 << ATUNE_PARA_NUM) - 1 )

typedef enum {
    FC_MAIN_FUNC = 0,
    FC_SUBFUNC_NUM
}FC_SUBFUNC;

typedef enum {
    FC_PARA_START = 0,
    FC_FC = FC_PARA_START,
    FC_ENABLE,
    FC_PARA_END = FC_ENABLE,
    FC_PARA_NUM,
}FC_PARAS;

#define FC_ALL_PAPAS_MASK   ( (1 << FC_PARA_NUM) - 1 )

typedef enum {
    ECHO_MAIN_FUNC = 0,
    ECHO_SUBFUNC_NUM
}ECHO_SUBFUNC;

typedef enum
{
    ECHO_PARA_START = 0,
    ECHO_FEEDBACK_COFF = ECHO_PARA_START,
    ECHO_GAIN_DRY,
    ECHO_GAIN_WET,
    ECHO_DLY,
    ECHO_LP_ENABLE,
    ECHO_LP_FREQ,
    ECHO_ENABLE,
    ECHO_PARA_END = ECHO_ENABLE,
    ECHO_PARA_NUM
}ECHO_PARAS;

#define ECHO_ALL_PAPAS_MASK   ( (1 << ECHO_PARA_NUM) - 1 )

typedef enum {
    REVB_MAIN_FUNC = 0,
    REVB_SUBFUNC_NUM
}REVB_SUBFUNC;

typedef enum {
    REVB_PARA_START = 0,

    REVB_PARA_TIME = REVB_PARA_START,
    REVB_PARA_DAMP_FREQ_LO,
    REVB_PARA_DAMP_FREQ_HI,
    REVB_PARA_LPF_FREQ,
    REVB_PARA_HPF_FREQ,
    REVB_PARA_PRE_DLY,
    REVB_PARA_DIFF_RATIO,
    REVB_PARA_DAMP_LO_RATIO,
    REVB_PARA_DAMP_HI_RATIO,
    REVB_PARA_GAIN_WET,
    REVB_PARA_GAIN_DRY,
    REVB_ENABLE,
    REVB_MS_STU,

    REVB_PARA_END = REVB_MS_STU,
    REVB_PARA_NUM
}REVB_PARAS;

#define REVERB_ALL_PAPAS_MASK   ( (1 << REVB_PARA_NUM) - 1 )

typedef enum {
    VSS_MAIN_FUNC = 0,
    VSS_SUBFUNC_NUM
}VSS_SUBFUNC;

typedef enum {
    VSS_PARA_START = 0,
    VSS_PARA_SWITCH = VSS_PARA_START,
    VSS_PARA_MODE_SWITCH,
    VSS_PARA_SPK_SYNTHESIS_WIDTH,
    VSS_PARA_HP_SYNTHESIS_WIDTH,
    VSS_PARA_SPK_SPATIAL_INTENSITY,
    VSS_PARA_HP_SPATIAL_INTENSITY,
    VSS_PARA_SPK_BASS_INTENSITY,
    VSS_PARA_HP_BASS_INTENSITY,
    VSS_PARA_SPK_DEFINITION,
    VSS_PARA_HP_DEFINITION,
    VSS_PARA_SPK_BASS_COFREQ,
    VSS_PARA_HP_BASS_COFREQ,
    VSS_PARA_SPK_SPAN_WIDTH,
    VSS_PARA_HP_DEPTH_WIDTH,
    VSS_PARA_END = VSS_PARA_HP_DEPTH_WIDTH,
    VSS_PARA_NUM,
    VSS_PARA_INVAILD = 0xFF
}VSS_PARAS;

#define VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(mode) (!mode)

typedef struct
{
    int16_t ftype;
    bool   disable;
}apf2zf_section_map;

typedef enum
{
    TYPE_BYPASS = 0,
    TYPE_6DB_1ST_ORDER,
    TYPE_12DB_BESSEL,
    TYPE_12DB_BUTTER,
    TYPE_12DB_LINK,
    TYPE_18DB_BESSEL,
    TYPE_18DB_BUTTER,
    //TYPE_18DB_LINK, ----> fucking none!!!!
    TYPE_24DB_BESSEL,
    TYPE_24DB_BUTTER,
    TYPE_24DB_LINK
}SUB_APF2ZF_TYPE;

typedef enum {
    PROFILE_BYPASS = 0,
    PROFILE_REVERB,
    PROFILE_FEMALE,
    PROFILE_MALE,
    PROFILE_KID,
    PROFILE_ROBOT,
    PROFILE_RADIO,
    PROFILE_NUM
}AFX_PROFILE;

typedef enum _eq_type_t
{
    EQ_BASS=0,
    EQ_MID,
    EQ_TREBLE,
    EQ_TYPE_NUM,
}eq_type_t;

/* note base */
#define ATUNE_KEY_C    0
#define ATUNE_KEY_Db   1
#define ATUNE_KEY_D    2
#define ATUNE_KEY_Eb   3
#define ATUNE_KEY_E    4
#define ATUNE_KEY_F    5
#define ATUNE_KEY_Gb   6
#define ATUNE_KEY_G    7
#define ATUNE_KEY_Ab   8
#define ATUNE_KEY_A    9
#define ATUNE_KEY_Bb   10
#define ATUNE_KEY_B    11

/* octave region */
#define ATUNE_OCT_3     3
#define ATUNE_OCT_4     4
#define ATUNE_OCT_5     5

/* scale style */
#define ATUNE_SCALE_CHROMATIC   0
#define ATUNE_SCALE_MAJOR       1
#define ATUNE_SCALE_MINOR        2

typedef enum _ATUNE_CMD{
    ATUNE_SET_SWITCH,
    ATUNE_SET_KEY,
    ATUNE_SET_SCALE
}ATUNE_CMD;

typedef enum _ATUNE_SWTICH{
    ESWITCH_OFF = 0,
    ESWITCH_SOFT_ON,
    ESWITCH_HARD_ON,
}EFFECT_SWTICH;

typedef struct
{
    /* Notes base */
    int8_t note;
    /* octave idx */
    int8_t oct;
    /* scale style */
    int8_t scale;
    /* atune speed, 1~10 */
    int8_t speed;
    /**/
    int8_t enable;
}atune_setting;

typedef enum {
    EQ_SET_NO_ERR = 0,
    EQ_SET_INVALID_STU = 0x80000000,
    EQ_SET_PATCH_WELCOME = 0x7FFF0000,
    EQ_XET_NONE_RESPONSE
}EQ_SETTING_STU;

#define EQ_SET_DATA true
#define EQ_GET_DATA false

/*
    AFX_PSAL_HOOK:
    Pre-Setting-Async-Loading mode hook, means the internal para has been
    preprocess without syncing to afx-chain-processing, use this hook to sending
    a cmd to afx-chain
*/
#define AFX_PSAL_HOOK ((user_setting_callback)0)

typedef struct {
    /*
            filter type :
                NAME                            VAL          NOTE
                BIQUAD_LOWSHELVING      0            sqrt(2)*Freq point < Fs/2
                BIQUAD_PEAKNOTCH         1
                BIQUAD_HIGHSHELVING      2
                BIQUAD_LOWPASS            3
                BIQUAD_BANDPASS           4
                BIQUAD_HIGHPASS            5
      */
    int16_t filter_type;
    /* center frequency(peak/notch) or crossover frequency(shelving filter ) */
    uint16_t fc;
    /*
            Gain : -12~12 dB, float
       */
    float G;
    /*
            Qfactor : 0.1 ~ 16, float
      */
    float Q_factor;
    /*
            disable
      */
    int8_t disable;
}eq_band_setting;

typedef enum{
    HIGH_PASS = APF2ZF_DFTYPE_HPF,
    LOW_PASS  = APF2ZF_DFTYPE_LPF
}hl_type;

typedef struct {
    /*
            SUB_APF2ZF_TYPE
      */
    SUB_APF2ZF_TYPE sub_type;
    /* fc */
    uint16_t fc;
}hl_filter_setting;

typedef struct {
    /*
            antiphasic factor
            reverse : -1
            normal  :   1
      */
    int8_t   rev_factor;
    /*
            delay
            0 - 200ms, according to runtime ram status
       */
    uint16_t rtdly;
}dly_setting;

typedef struct {
    /*
            start threshold
            -80 - 0db
      */
    int8_t   x_thd;
    /*
            subfunc ratio
            1 - 255
      */
    uint8_t  x_ratio;
    /*
            attack time
            N ms
      */
    uint16_t x_att;
    /*
            release time
            N ms
      */
    uint16_t x_rel;
    /*
            enable switch
      */
    uint8_t  enable;
}drc_subfunc_setting;

typedef struct
{
    /* attack time in ms */
    uint16_t att_time;
    /* release time in ms */
    uint16_t rel_time;
    /* threshold -25db default */
    int8_t  duck_thd;
    /* destination gain -15db default */
    int8_t  duck_gain;
}duck_setting;

typedef struct
{
    //should be 2^(-1) - 2^(1)
    float alpha_ts;
    //should be 2^(-1) - 2^(1)
    float alpha_ps;
    //enable?
    bool  enable;
}tsps_setting;

typedef struct
{
    float alpha_fc;
    //enable?
    bool  enable;
}fc_setting;

typedef struct
{
    /* feedback coefficient, 0<g<0.85 */
    float gFbc;
    /* gain of direct arrival sound */
    float gDry;
    /* gain of echo in the final output */
    float gWet;
    /* dly ms : full band 0 - 200ms, narrow band 0 - 300ms */
    uint16_t dly;
    /* need lp for fbdata */
    uint16_t need_lp;
    /* lp cut-off freq */
    uint16_t cutoff_freq;
    /* enable */
    bool enable;
}echo_setting;

typedef struct
{
    /*
            value range see rerb_setting_check()
      */
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
    /* enable? */
    bool  enable;
    /* switch to stereo? */
    bool  m2s;
}rev_setting;

typedef enum {
    VSS_MODE_DEFAULT = 0,
    VSS_MODE_SPEAKER = VSS_MODE_DEFAULT,
    VSS_MODE_HEADPHONE,
    VSS_MODE_NUM
}vss_mode;

typedef union
{
    int8_t hp_depth;
    int8_t spk_span;
    int8_t thiz;
}vss_modes_sound_field_alias;

typedef struct
{
    /* synthesis width 0~7 */
    int8_t synth;
    /* depth of space, 0~10 */
    vss_modes_sound_field_alias sound_field_width;
    /* spatial intensity, 0~10 */
    int8_t sp_intensity;
    /* bass intensity, 0~10 */
    int8_t bass_intensity;
    /* definition, 0~10 */
    int8_t definition;
    /* bass crossoverfreq */
    int16_t bass_cofreq;
}vss_pro_setting;

typedef struct
{
    eq_band_setting     eq5[MAX_EQ5_BANDS];
    drc_subfunc_setting limiter;
    vss_pro_setting     hp_and_spk[VSS_MODE_NUM];
    int8_t              mode;
}vss_settings;

typedef enum _TypeClass {
    //Total start
    TYPE_REVERB_ECHO_SWITCH = 0xFB,
    TYEE_BAND_STATE_SWITCH = 0xFC,
    TYEE_RESET = 0xFE,

    TYPE_START = 0,
    TYPE_EQ_START = TYPE_START,
    //Eq start
    TYPE_FREQ = TYPE_EQ_START,
    TYPE_Q,
    TYPE_GAIN,
    TYPE_FILTER,
    TYPE_EQ_SETTING_ALL,
    TYPE_EQ_PRE_SETTING,
    //Eq end
    TYPE_EQ_END = TYPE_EQ_PRE_SETTING,
    //Tsps start
    TYPE_TSPS_START = 30,
    TYPE_TSPS_FC_SWITCH = TYPE_TSPS_START,
    TYPE_MAPPING_TSPS_PARA_SETTING_START,
    TYPE_TSPS_PS = TYPE_MAPPING_TSPS_PARA_SETTING_START,
    TYPE_TSPS_TS,
    TYPE_TSPS_FC,
    TYPE_MAPPING_TSPS_PARA_SETTING_END = TYPE_TSPS_FC,
    TYPE_TSPS_ATUNE_SETTINGS,
    TYPE_TSPS_SETTING_ALL,
    TYPE_TSPS_ATUNE_PRE_SETTING,
    TYPE_FC_SETTING_ALL,
    TYPE_FC_PRE_SETTING,

    TYPE_TSPS_END = TYPE_FC_PRE_SETTING,
    //Tsps end
    //Drc start
    TYPE_DRC_START = 60,
    TYPE_DRC_MAKEUP_GAIN = TYPE_DRC_START,
    TYPE_DRC_SUB_PROCESSOR_SWITCH,
    TYPE_MAPPING_DRC_PARA_SETTING_START,
    TYPE_DRC_SUB_PROCESSOR_THRESHOLD = TYPE_MAPPING_DRC_PARA_SETTING_START,
    TYPE_DRC_SUB_PROCESSOR_ATTACK_TIME,
    TYPE_DRC_SUB_PROCESSOR_RATIO,
    TYPE_DRC_SUB_PROCESSOR_RELEASE_TIME,
    TYPE_MAPPING_DRC_PARA_SETTING_END = TYPE_DRC_SUB_PROCESSOR_RELEASE_TIME,
    TYPE_DRC_SETTING_ALL,
    TYPE_DRC_PRE_SETTING,
    //Drc end
    TYPE_DRC_END = TYPE_DRC_PRE_SETTING,
    //Reverb start
    TYPE_REVERB_START = 90,
    TYPE_REVERB_SWITCH = TYPE_REVERB_START,
    TYPE_REVERB_PREDLY,
    TYPE_REVERB_TIME,
    TYPE_REVERB_DIFFUSION,
    TYPE_REVERB_G_WET,
    TYPE_REVERB_G_DRY,
    TYPE_REVERB_DAMPING_FREQ_LO,
    TYPE_REVERB_DAMPING_RATIO_LO,
    TYPE_REVERB_DAMPING_FREQ_HI,
    TYPE_REVERB_DAMPING_RATIO_HI,
    TYPE_REVERB_LP_FREQ,
    TYPE_REVERB_HP_FREQ,
    TYPE_REVERB_REC_MS_SWITCH,
    TYPE_REVERB_SETTING_ALL,
    TYPE_REVERB_PRE_SETTING,
    //Reverb end
    TYPE_REVERB_END = TYPE_REVERB_PRE_SETTING,
    //Echo start
    TYPE_ECHO_START = 120,
    TYPE_ECHO_SWITCH = TYPE_ECHO_START,
    TYPE_ECHO_LP,
    TYPE_ECHO_G_DRY,
    TYPE_ECHO_G_WET,
    TYPE_ECHO_LP_FREQ,
    TYPE_ECHO_DELAY,
    TYPE_ECHO_FEEDBACK_COFF,
    TYPE_ECHO_SETTING_ALL,
    TYPE_ECHO_PRE_SETTING,
    //Echo end
    TYPE_ECHO_END = TYPE_ECHO_PRE_SETTING,

    TYPE_DR_START = 150,
    TYPE_DR_SWITCH = TYPE_DR_START,
    TYPE_DR_DLY,
    TYPE_DR_REVERSE,
    TYPE_DR_SETTING_ALL,
    TYPE_DR_PRE_SETTING,
    TYPE_DR_END = TYPE_DR_PRE_SETTING,

    TYPE_VSS_START = 160,
    TYPE_VSS_HP_SPK_SWITCH = TYPE_VSS_START,//HP default
    TYPE_VSS_SYNTHESIS_WIDTH,//0-7
    TYPE_VSS_SPAN_WIDTH,//0-7
    TYPE_VSS_DEPTH_WIDTH,
    TYPE_VSS_SPATIAL_INTENSITY,//0-10
    TYPE_VSS_BASS_INTENSITY,//0-10
    TYPE_VSS_DEFINITION,//0-10
    TYPE_VSS_BASS_COFREQ,//20-200Hz
    TYPE_VSS_SWITCH,
    TYPE_VSS_PRE_SETTING,
    TYPE_VSS_END = TYPE_VSS_PRE_SETTING,

    TYPE_PLG_SETTING_START = 190,
    TYPE_PLG_AMICX_SETTING_START = TYPE_PLG_SETTING_START,
    TYPE_PLG_AMIC0_GAIN = TYPE_PLG_AMICX_SETTING_START,
    TYPE_PLG_AMIC1_GAIN,
    TYPE_PLG_AMIC2_GAIN,
    TYPE_PLG_AMIC0_ENABLE,
    TYPE_PLG_AMIC1_ENABLE,
    TYPE_PLG_AMIC2_ENABLE,
    TYPE_PLG_AMICX_SETTING_END = TYPE_PLG_AMIC2_ENABLE,

    TYPE_PLG_FS_SETTING_START,
    TYPE_PLG_FS_SETTING_DELTAFREQ = TYPE_PLG_FS_SETTING_START,
    TYPE_PLG_FS_SETTING_END = TYPE_PLG_FS_SETTING_DELTAFREQ,

    TYPE_PLG_MIC_NHS_SETTING_START = 197,
    TYPE_PLG_MIC_NHS_NP = TYPE_PLG_MIC_NHS_SETTING_START,
    TYPE_PLG_MIC_NHS_NF = 198,
    TYPE_PLG_MIC_NHS_THD_PTPR = 199,
    TYPE_PLG_MIC_NHS_THD_PAPR = 200,
    TYPE_PLG_MIC_NHS_THD_PHPR = 201,
    TYPE_PLG_MIC_NHS_THD_PNPR = 202,
    TYPE_PLG_MIC_NHS_THD_DROP = 203,
    TYPE_PLG_MIC_NHS_ATT = 204,
    TYPE_PLG_MIC_NHS_TIME_DROP = 205,
    TYPE_PLG_MIC_NHS_TIME_HISTORY = 206,
    TYPE_PLG_MIC_NHS_TIME_HOLD = 207,
    TYPE_PLG_MIC_NHS_THD_REL = 208,
    TYPE_PLG_MIC_NHS_THD_SINGLE_TONE = 209,
    TYPE_PLG_MIC_NHS_GAIN_DSTEP = 210,
    TYPE_PLG_MIC_NHS_GAIN_FLOOR_DECREASE = 211,
    TYPE_PLG_MIC_NHS_TIME_AVG_CONST = 212,
    TYPE_PLG_MIC_NHS_TIME_DESCEND = 213,
    TYPE_PLG_MIC_NHS_TIME_AESEND = 214,
    TYPE_PLG_MIC_NHS_TIME_ENTER = 215,
    TYPE_PLG_MIC_NHS_TIME_COFF_SMOOTH = 216,
    TYPE_PLG_MIC_NHS_SETTING_END = TYPE_PLG_MIC_NHS_TIME_COFF_SMOOTH,

    TYPE_PLG_SETTING_END = TYPE_PLG_MIC_NHS_SETTING_END,

    TYPE_INPUT_START  = 220,
    TYPE_INPUT_SOURCE = TYPE_INPUT_START,
    TYPE_INPUT_VOL,

    TYPE_DIVIDER_HLP_START,
    TYPE_DIVIDER_HP_FRE = TYPE_DIVIDER_HLP_START,
    TYPE_DIVEDER_HP_TYPE,

    TYPE_DIVIDER_LP_FRE,
    TYPE_DIVEDER_LP_TYPE,
    TYPE_DIVIDER_HLP_END = TYPE_DIVEDER_LP_TYPE,

    TYPE_INPUT_END = TYPE_DIVIDER_HLP_END,
    //Total end
    TYPE_END = TYPE_INPUT_END
}TypeClass;

enum E_INPUT_SOURCE
{
    AUX0 = 0,
    AUX1,
    AUX2,
    COAXIAL,
    FIBER
};

enum EQ_EQUIPMENT
{
    AMPLITUDE,
    COMPRESS,
    EXPANSION,
    NOISE
};


#ifdef __cplusplus
extern "C" {
#endif

#include "sys_types.h"

enum afx_eq_music_type_e {
    AFX_EQ_MUSIC_TYPE_START        = 0,
    AFX_EQ_MUSIC_TYPE_POP          = AFX_EQ_MUSIC_TYPE_START,
    AFX_EQ_MUSIC_TYPE_ROCK,
    AFX_EQ_MUSIC_TYPE_JAZZ,
    AFX_EQ_MUSIC_TYPE_CLASSICAL,
    AFX_EQ_MUSIC_TYPE_DANCE,
    AFX_EQ_MUSIC_TYPE_COUNTRY,
    AFX_EQ_MUSIC_TYPE_END          = AFX_EQ_MUSIC_TYPE_COUNTRY,
    AFX_EQ_MUSIC_TYPE_NONE,
    AFX_EQ_MUSIC_TYPE_NUM
};

enum afx_chain_cmd_e {
    AFX_CHAIN_CMD_NONE,
    AFX_CHAIN_CMD_EQ5_SETTING,
    AFX_CHAIN_CMD_EQ_MUSIC_TYPE,    /* see @afx_eq_music_type_e */
    AFX_CHAIN_CMD_EQ_SETPREAMP,     /* see @afx_eq_pampprm_t */
    AFX_CHAIN_CMD_UNIVERSE_SETTING,
    AFX_CHAIN_CMD_QUERY_MASS_EFFECTS,
};

typedef struct {
    uint32_t          owner;
    uint32_t          ch;
    eq_band_setting  *setting;
    float             preamp;
    int8_t            enable;
}eq5_setting_list;

typedef struct {
    uint32_t rate;
    uint8_t ch;
    uint8_t sample_size;
    uint16_t proc_samples;
    bool aacelerate;
} afx_chain_init_params_t;

void *create_afx_chain(int8_t io, afx_work_flow* awf, int32_t awf_num);
void  destroy_afx_chain(void *chain_handle);

int32_t afx_chain_open(void *chain_handle, afx_chain_init_params_t *chain_params);
void afx_chain_close(void *chain_handle);
void afx_chain_proc(void *chain_handle, int32_t *inbuf, int32_t samples);
int32_t afx_chain_cmd(void *chain_handle, uint32_t cmd, uint32_t val);
void afx_chain_set_tail(void *chain_handle, void *tail_handle, afx_tail_callback_t func);

void *getLeftChannelEqAddrForPageId(PageClass page_id);
void *getRightChannelEqAddrForPageId(PageClass page_id);
void *getLeftChannelEqPreampForPageId(PageClass page_id);
void *getRightChannelEqPreampForPageId(PageClass page_id);
void *getLeftChannel5SecEqPreampForPageIdAndCompType(PageClass page_id, uint32_t comp);
void *getRightChannel5SecEqPreampForPageIdAndCompType(PageClass page_id, uint32_t comp);
void *getLeftChannel5SecEqAddrForPageIdAndCompType(PageClass page_id, uint32_t comp);
void *getRightChannel5SecEqAddrForPageIdAndCompType(PageClass page_id, uint32_t comp);
void setTbvcSwitch(void* setting, bool sw);


void* getDrAddrForPageId(PageClass page_id);
void* getDrcAddrForPageId(PageClass page_id);

void* getTspsAtuneAddrForPageId(PageClass page_id);
void* getFcAddrForPageId(PageClass page_id);

void* getEchoAddrForPageId(PageClass page_id);

void* getRevbAddrForPageId(PageClass page_id);

bool    getDrSwitchStu(void* dr_addr);
float   getDrcMakeupGain(void* drc_addr);
uint8_t getDrcSubFuncMask(void* drc_addr);

void* getVssXAddrForPageId(PageClass page_id, VSS_SUB_CMD sub_cmd);

void x_eq_subband_para(void *eq_addr, EQ_SUBBAND band, EQ_SUBBAND_PARAS para, void* val, bool set);
void x_eq_preamp_para(void *eq_amp, EQ_PREAMP_SUBFUNC subfunc, EQ_PREAMP_SBPARA para, void* val, bool set);
void x_dr_sunfunc_para(void* dr_addr, DR_SUBFUNC subfunc, DR_PARAS para, void* val, bool set);
void x_drc_sunfunc_para(void* drc_addr, DRC_SUBFUNC subfunc, DRC_SUBFUNC_PARAS para, void* val, bool set);
void x_tsps_sunfunc_para(void* tsps_addr, TSPS_SUBFUNC subfunc, TSPS_PARAS para, void* val, bool set);
void x_atune_sunfunc_para(void* atune_addr, ATUNE_SUBFUNC subfunc, ATUNE_PARAS para, void* val, bool set);
void x_fc_sunfunc_para(void* fc_addr, FC_SUBFUNC subfunc, FC_PARAS para, void* val, bool set);
void x_echo_sunfunc_para(void* echo_addr, ECHO_SUBFUNC subfunc, ECHO_PARAS para, void* val, bool set);
void x_revb_sunfunc_para(void* revb_addr, REVB_SUBFUNC subfunc, REVB_PARAS para, void* val, bool set);
void x_vss_sunfunc_para(void* vss_addr, VSS_SUBFUNC subfunc, VSS_PARAS para, void* val, bool set);

void setRevbHolderForPageId(bool hold, PageClass page_id);
void setEchoHolderForPageId(bool hold, PageClass page_id);
void setDrcHolderForPageId(bool hold, PageClass page_id);

void init_afx_params(void* chain);
#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_EFFECTS_H_ */
