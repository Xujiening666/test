
#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "audio_service.h"
#include "audiopath_conf.h"
#include "sndlib.h"
#include "audio_in.h"
#include "audio_out.h"
#include "log.h"
#include "usbaudio.h"
#include "audio_internal_config.h"
#include "audio_player.h"
#include "app_afx_config.h"

#define EQ_LOAD_FROM_RAM     (0)

#if (!AUDIO_KARAOKE_MODE)
AT(.crossfilt_seg)
float compliment_allpass(int ch, short val, void* flt)
{
    return 0.0;
}

AT(.crossfilt_seg)
void crossover_filt_interleave(void* flt, short *hbuf, short *lbuf, int samples, int mix)
{
}

#else

#if (!BETTER_VOCAL_CUT)

AT(.crossfilt_seg)
float compliment_allpass(int ch, short val, void* flt)
{
    return 0.0;
}

AT(.crossfilt_seg)
void crossover_filt_interleave(void* flt, short *hbuf, short *lbuf, int samples, int mix)
{
    if(mix){
        for(int i = 0; i < 2*samples; i++){
            *lbuf++ = *hbuf;
            *hbuf++ = 0;
        }
    }
}
#endif

#endif

AT(.audio_effects_seg)
bool audioPlbkSpectrum()
{
    return !!(AUDIO_PLAYBACK_SPECTRUM && !AUDIO_PR_MIX_SPECTRUM);
}

AT(.audio_effects_seg)
bool audioKaraokePlbkMixSpectrum()
{
    return !!(AUDIO_PR_MIX_SPECTRUM);
}

#if (!AUDIO_PR_MIX_DECORR)
AT(.com_seg)
const void *decorr_lib = NULL;
#endif

#if (!AUDIO_KARAOKE_USE_NHS)
AT(.com_seg)
const void *nhs_lib = NULL;
#endif

#if (!AUDIO_KARAOKE_AUXTRACK_EN)
AT(.com_seg)
const void *auxtm_lib = NULL;
#endif

#if (!AUDIO_PR_MIX_SPECTRUM && !AUDIO_PLAYBACK_SPECTRUM)
AT(.com_seg)
const void *spec_handle = NULL;
#endif

#if (!AUDIO_PLAYBACK_VSS)
AT(.cache_seg)
const void* sdpro_handle = NULL;
#endif

#if (!AUDIO_PLAYBACK_DR)
AT(.cache_seg)
const void* dr_handle = NULL;
#endif

#if (!AUDIO_PLAYBACK_TBVC)
AT(.cache_seg)
const void* tbvc_handle = NULL;
#endif

#if (!AUDIO_MIC_ECHO)
AT(.cache_seg)
const void* echo_handle = NULL;
#endif

#if (!AUDIO_MIC_REVERB)
AT(.cache_seg)
const void* fdnrev_handle = NULL;
#endif

#if (!AUDIO_PLAYBACK_TSPS && !AUDIO_MIC_TSPS)
AT(.cache_seg)
const void* tsps_handle = NULL;
#endif
#if (!AUDIO_AFX_SCO_TSPS_EN)
AT(.cache_seg)
const void* sco_tsps_handle = NULL;
#endif
#if (!AUDIO_KARAOKE_USE_DUCKER)
AT(.cache_seg)
const void* ducker_handle = NULL;
#endif

#if (!AUDIO_AFX_REC_FORMANT_EN)
AT(.cache_seg)
const void* fc_handle = NULL;
#endif

#if (!AUDIO_AFX_REC_AUTOTUNE_EN)
AT(.cache_seg)
const void* atune_handle = NULL;
#endif

#if (!AUDIO_AFX_POST_ECHO_EQ_EN && !AUDIO_AFX_POST_REVB_EQ_EN)
AT(.cache_seg)
const void* eq2_handle = NULL;
#endif

#if (!AUDIO_AFX_VSS_SWITCH_FADEINOUT || !AUDIO_PLAYBACK_VSS)
AT(.cache_seg)
const void* cf_handle = NULL;
#endif

#if (!MUSIC_METADATA)
AT(.cache_seg)
const void* get_meta_hook = NULL;
#endif

/***************************************************************************
 *               蓝牙音乐相关参数配置
 ***************************************************************************/
// SBC缓存buffer大小
AT(.a2dp_play_flash_seg)
uint32_t get_a2dp_bitstream_buffer_size(void)
{
    return 16*1024;
}

//A2DP播放丢包补偿功能
bool a2dp_plc_enable(void)
{
    return false;
}

//sbc buffer数据空之后播放丢弃多少个SBC包
AT(.a2dp_play_flash_seg)
uint32_t get_a2dp_packet_discard_count(void)
{
    return 0;
}

/***************************************************************************
 *               蓝牙通话相关参数配置
 ***************************************************************************/
/* 基本通话降噪算法参数 */
const phone_aenc_config_params_t aenc_config_params = {
#if (AUDIO_PHONE_OUTPUT_SEL)
    /* 耳机模式（不使能AEC/AES/DR/AVC） */
    .func_mask = (PHONE_NS | PHONE_TXAGC | PHONE_RXAGC |
                  PHONE_CNG | PHONE_GSMOOTH),
#else
    /* 喇叭模式 */
    .func_mask = (PHONE_AEC | PHONE_AES | PHONE_NS | PHONE_DR | PHONE_AVC |
                  PHONE_TXAGC | PHONE_RXAGC | PHONE_CNG | PHONE_GSMOOTH),
#endif

    .alpha_slow     = (float)0.2,   /* 0 ~ 1 */
    .alpha_fast     = (float)0.5,   /* 0 ~ 1 */
    .pf_GHmin       = (float)-20,   /* -30 ~ 0 (dB) */

    .res_overest    = (float)0,     /* -10 ~ 10 (dB) */

    .dt_thd_norm    = (float)0.92,  /* 0 ~ 1 */

    .pf_Xi_opt      = (float)5,     /* 0 ~ 20 */
    .Npsd_bias      = (float).9,    /* 0 ~ 2 */

    .pf_Gcn         = (float)-10,   /* -30 ~ 0 (dB) */

    .avq_reftx      = (float)-23,
    .avq_refrx      = (float)-20.5,
    .avq_vadthd     = (float)-40,   /* -60 ~ 0 (dBFs) */
    .avq_offset     = (float)9,     /* 0 ~ 10 (dB) */
    .avq_Gmax       = (float)10,    /* 0 ~ 15 (dB) */

    .gmax           = (float)10,    /* -20 ~ 20 (dB) */
    .gmin           = (float)-10,   /* -20 ~ 20 (dB) */
    .xtgt           = (float)-10,   /* -20 ~ 0 (dB) */
};

/* 其他通话降噪算法参数 */
const phone_aenc_params_t aenc_params = {
    .gain_smooth_thd    = (float)0.4,
    .gain_smooth_width  = (float)5,
    .SNR_s_Min          = (float)-20,
    .SNR_ns_Min         = (float)-60,
    .psd_Min            = (float)-100,
    .pf_alpha_Xis       = 1-(float)0.99,
    .pf_alpha_Xins      = 1-(float)0.95,

    .alpha_fast_resG    = (float)0.8,
    .alpha_slow_resG    = (float)0.0,
    .dt_thd_limit       = (float)0.15,
    .dt_init_thd        = (float)0.5,
    .dt_steprise_decvar = (float)0.0145,
    .dt_stepfall_decvar = (float)8./30000,

    .finit              = (float)10,
    .pf_ph1_thd         = (float).9999,
    .pf_alpha_npsd      = 1-(float)0.8,
    .pf_alpha_PH1       = 1-(float)0.9,

    .pf_alpha_Nmin_f    = (float)0.1,
    .pf_inc_Nmin_t      = (float)0.001,

    .pf_alpha_z         = 1-(float)0.95,
    .pf_decay           = (float)-8,
    .decay_lthd         = (float)-3.2,
    .alpha_decay        = (float)0.4,
    .decay_km           = (float)0.001,
    .rise_km            = (float)0.4,
    .thd_km             = (float)-40,

    .avq_dec_step       = (float)5,
    .avq_sf_norm        = (float)-30,
    .avq_alpha_fref     = (float)0.03,
    .avq_alpha_sref     = (float)0.01,
    .avq_alpha_fn       = (float)0.15,
    .avq_alpha_sn       = (float)0.05,

    .alpha_fr           = (float)1,
    .alpha_ff           = (float).35,
    .alpha_sr           = (float).1,
    .alpha_sf           = (float)1./2000,
    .vad_alpha_short    = (float)1./16,
    .vad_alpha_long     = (float)8./3000,
    .vad_alpha_ratio    = (float)3./16,
    .vad_log_ratio_uthd = (float)2,
    .vad_log_ratio_lthd = (float)0,
    .vad_log_ratio_min  = (float)-2,
    .vad_log_ratio_max  = (float)2,
    .vad_var_min        = (float)5,
    .amp_min            = (float)-100,
};

#if EQ_LOAD_FROM_RAM
#include "eq_process.h"
#include "audio_effects.h"

typedef struct _EQ_FIXED_HEADER{
    uint8_t  magic[4];
    uint8_t  version;
    uint8_t  eq_num;
    uint8_t  reserved[2];
    uint16_t first_eq_off;
}EQ_FIXED_HEADER;

typedef struct _EQ_HEADER{
    EQ_FIXED_HEADER  eq_fhdr;
    uint32_t eq_lens;
    uint16_t eq_hdr_len;
    uint16_t crc_hdr;
    uint16_t crc_val;
}EQ_HEADER;

uint8_t eq_buffer[352] = {
    0x53, 0x4c, 0x45, 0x51, 0x02, 0x01, 0x00, 0x00,
    0x10, 0x00, 0x2a, 0x00, 0x49, 0x4e, 0x6f, 0x8d,
    0x05, 0x3e, 0x00, 0x00, 0x80, 0xc2, 0xf5, 0xbe,
    0x05, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f,
    0x05, 0xa5, 0x00, 0x01, 0x00, 0x00, 0x80, 0x3f,
    0x05, 0xa6, 0x00, 0x01, 0x00, 0x00, 0x90, 0x40,
    0x00, 0x02, 0x00, 0x00, 0xec, 0x51, 0x04, 0xc1,
    0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x00, 0x01, 0x00, 0x14, 0xae, 0xf1, 0x41,
    0x00, 0x02, 0x01, 0x00, 0xec, 0x51, 0xb8, 0x3d,
    0x00, 0x03, 0x01, 0x00, 0x00, 0x00, 0xa0, 0x40,
    0x00, 0x00, 0x02, 0x00, 0x48, 0xe1, 0x70, 0x42,
    0x00, 0x02, 0x02, 0x00, 0x85, 0xeb, 0x09, 0x41,
    0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x80, 0x3f,
    0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x00, 0x8f, 0x82, 0x08, 0x43,
    0x00, 0x02, 0x03, 0x00, 0x3d, 0x0a, 0x47, 0x40,
    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xc8, 0x42,
    0x00, 0x02, 0x04, 0x00, 0x52, 0xb8, 0x96, 0x40,
    0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0xa0, 0x40,
    0x00, 0x00, 0x05, 0x00, 0x52, 0xb8, 0x4a, 0x43,
    0x00, 0x02, 0x05, 0x00, 0xf6, 0x28, 0x2c, 0x41,
    0x00, 0x01, 0x05, 0x00, 0x33, 0x33, 0x33, 0x3f,
    0x00, 0x00, 0x07, 0x00, 0xae, 0x27, 0xf4, 0x43,
    0x00, 0x02, 0x07, 0x00, 0x00, 0x00, 0x80, 0x40,
    0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x00, 0x08, 0x00, 0x48, 0x71, 0x57, 0x44,
    0x00, 0x02, 0x08, 0x00, 0x5c, 0x8f, 0x02, 0x40,
    0x00, 0x01, 0x08, 0x00, 0x00, 0x00, 0x80, 0x3f,
    0x00, 0x00, 0x09, 0x00, 0x71, 0x6d, 0x00, 0x45,
    0x00, 0x02, 0x09, 0x00, 0x14, 0xae, 0xcf, 0x40,
    0x00, 0x01, 0x09, 0x00, 0x33, 0x33, 0xb3, 0x3f,
    0x00, 0x00, 0x0a, 0x00, 0xae, 0x9d, 0x82, 0x45,
    0x00, 0x02, 0x0a, 0x00, 0xb8, 0x1e, 0x25, 0x40,
    0x00, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x80, 0x3f,
    0x00, 0x00, 0x0b, 0x00, 0x3d, 0x70, 0xee, 0x45,
    0x00, 0x02, 0x0b, 0x00, 0xc3, 0xf5, 0xb8, 0x40,
    0x00, 0x01, 0x0b, 0x00, 0x33, 0x33, 0xb3, 0x3f,
    0x00, 0x00, 0x0c, 0x00, 0x0a, 0xbd, 0x2d, 0x46,
    0x00, 0x02, 0x0c, 0x00, 0xe1, 0x7a, 0x9c, 0x40,
    0x00, 0x03, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x02, 0x0d, 0x00, 0x5c, 0x8f, 0x02, 0x40,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f,
};

AT(.startup_seg)
int8_t scan_page(uint32_t ptr_start, uint16_t offset_start, uint16_t scan_range, uint16_t* page_blck_size){
    uint8_t  start_page, page;
    uint16_t i;
    uint16_t page_size = 1;

    memcpy(&start_page, (uint8_t*)(ptr_start+offset_start), 1);
    for(i = 1; i < scan_range; i++)
    {
        memcpy(&page, (uint8_t*)(ptr_start + offset_start + i*BLOCK_SZIE), 1);
        if(page == start_page){
            page_size++;
        }
        else{
            break;
        }
    }
    *page_blck_size = page_size;
    return REC_PAGE(start_page)?0:
            (PLBK_PAGE(start_page)?1:-1);
}

AT(.startup_seg)
bool load_eq_setting(int8_t idx, uint8_t io)//0 : rec, 1 : plbk
{
    EQ_HEADER eq_hdr = {0};
    uint8_t* ptr_start = 0;
    uint16_t buffer_offset = 0;
    uint8_t *eq_buf;

    ptr_start = eq_buffer;

    memcpy(&eq_hdr.eq_fhdr, &ptr_start[buffer_offset], sizeof(EQ_FIXED_HEADER));
    buffer_offset += sizeof(EQ_FIXED_HEADER);
    eq_hdr.eq_hdr_len += sizeof(EQ_FIXED_HEADER);

    //variable header, at least have one eq profile
    if(eq_hdr.eq_fhdr.eq_num){
        eq_hdr.eq_lens = (uint32_t)(ptr_start + buffer_offset);
        buffer_offset += sizeof(short)*eq_hdr.eq_fhdr.eq_num;
        eq_hdr.eq_hdr_len += sizeof(short)*eq_hdr.eq_fhdr.eq_num;
    }
    //crc header
    memcpy(&eq_hdr.crc_hdr, &ptr_start[buffer_offset], sizeof(short));
    eq_hdr.eq_hdr_len += 2;
    buffer_offset += 2;
    //crc value
    memcpy(&eq_hdr.crc_val, &ptr_start[buffer_offset], sizeof(short));
    eq_hdr.eq_hdr_len += 2;
    buffer_offset += 2;

    if(eq_hdr.eq_fhdr.magic[0] != 'S' || eq_hdr.eq_fhdr.magic[1] != 'L' ||
        eq_hdr.eq_fhdr.magic[2] != 'E' || eq_hdr.eq_fhdr.magic[3] != 'Q'){
        //loge("magic wrong!");
        return false;
    }

    buffer_offset = eq_hdr.eq_fhdr.first_eq_off;

    for(int i = 0; i < eq_hdr.eq_fhdr.eq_num; i++)
    {
        if(idx == i){
            uint16_t block_len = 0;
            int8_t iot;
            int once_process_block_len = 0;
            uint16_t page_blck_size = 0;

            memcpy(&block_len, (uint8_t*)(eq_hdr.eq_lens + i*sizeof(short)), 2);
            while(block_len){
                once_process_block_len = block_len > MAX_BLOCK_PER_PAGE?MAX_BLOCK_PER_PAGE:block_len;
                block_len -= once_process_block_len;
                while(once_process_block_len){
                    iot = scan_page((uint32_t)ptr_start, buffer_offset, once_process_block_len, &page_blck_size);
                    if(iot == io){
                        eq_buf = get_eq_param_buff();
                        memcpy(eq_buf, &ptr_start[buffer_offset], page_blck_size*BLOCK_SZIE);
                        on_set_eq_data(eq_buf, page_blck_size*BLOCK_SZIE);
                    }
                    buffer_offset += page_blck_size*BLOCK_SZIE;
                    once_process_block_len -= page_blck_size;
                }
            }
            break;
        }
    }
    return true;
}
#endif

