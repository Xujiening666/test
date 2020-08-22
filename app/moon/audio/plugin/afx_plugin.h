#ifndef AFX_PLUGIN_H
#define AFX_PLUGIN_H

#include "user_config.h"
#include "sys_inc.h"
#include "audio_effects.h"

extern void on_unload_for_page_id(PageClass page_id);
extern void COMP_HID_Send_Data(uint8_t* buff,uint8_t length );

#define PLG_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id == PAGE_PLG)
#define CHECK_AND_PREPARE_BUFFER(buf, size, exception) \
    if(size != 64) exception;\
    else libc_memset(buf, 0xff, size)

#define SEND_DATA_TO_PC(buf) \
    COMP_HID_Send_Data(buf, 64)

//---------------- 音量设置 ----------------

#include "mixer.h"
#include "sndlib.h"

typedef struct
{
    int32_t amic0_gain;
    int8_t  amic0_en;
    int32_t amic1_gain;
    int8_t  amic1_en;
    int32_t amic2_gain;
    int8_t  amic2_en;
}amicx_setting;

//---------------- 移频设置 ----------------

#include "audio_service.h"

extern const void* freq_shift_lib_assign();

typedef struct
{
    /* shift freq in Hz (0~10Hz) */
    int8_t deltafreq;
}fs_rtprm_t;

extern const fs_rtprm_t app_fs_init;
extern fs_rtprm_t app_fsrtprm;

//---------------- NHS设置 -----------------

#include "audio_service.h"

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

extern const nhs_rtprms_t app_nhs_init;
extern nhs_rtprms_t app_nhsrtprm;
extern const void *nhs_lib;

#endif