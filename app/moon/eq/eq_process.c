#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "eq_process.h"
#include "audio_service.h"
#include "event.h"

#include "impliment/eqbuf_ctl/eq_buffer_controller.c"
#include "impliment/basic_api/afx_apis.c"
#include "impliment/uploader/upload_eq.c"

#define ONE_SECOND_IN_MS (1000)
#define RECORD_SLEEP  \
    audio_service_cmd(AS_CMD_SET_KARAOKE_REC_SWITCH, 0)
#define RECORD_WAKEUP_FOR_MS(t) \
    msleep(t); \
    audio_service_cmd(AS_CMD_SET_KARAOKE_REC_SWITCH, 1)

void clear_all_holder()
{
    setDrcHolderForPageId(0, PAGE_MIC);
    setRevbHolderForPageId(0, PAGE_EFFECT);
    setEchoHolderForPageId(0, PAGE_EFFECT);
}

void reset_all_effects()
{
    uint8_t *eq_buf = get_eq_param_buff();
    eq_data ed   = {0};
    uint8_t cnt  = 0;
    ed.page_id   = PAGE_RESET;
    ed.item_type = TYEE_RESET;
    ed.fill.data = 0.0;
    libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

    on_set_eq_data(eq_buf, cnt*8);
}

void on_set_bypass_profile()
{
    RECORD_SLEEP;

    reset_all_effects();

    RECORD_WAKEUP_FOR_MS(200);
}


// This order has its own meaning
// The later will call preview's api

#include "impliment/high_lvl_api/hla_eq.c"
#include "impliment/high_lvl_api/hla_reverb.c"
#include "impliment/high_lvl_api/hla_echo.c"
#include "impliment/high_lvl_api/hla_tsps_fc.c"
#include "impliment/high_lvl_api/hla_atune.c"
#include "impliment/high_lvl_api/hla_drc.c"
#include "impliment/high_lvl_api/hla_vss.c"
#include "impliment/high_lvl_api/hla_eq5.c"

void on_set_eq_data_isr(uint8_t* buff,uint32_t length)
{
    uint8_t* eq_buf = get_eq_param_buff();
    libc_memcpy(eq_buf-4, &length, sizeof(uint32_t));
    libc_memcpy(eq_buf, buff, length);
    event_put(EQ_EVENT_SET_EQ_DATA_ISR);
}
//const length equal 512
void on_set_eq_data(uint8_t* buff,uint32_t length)
{
    uint32_t page_id = buff[0];
    uint32_t type = buff[1];
    uint32_t io = buff[2];
    printf_debug("page_id(%d), type(%d), io(%d), length(%d), %x\n", page_id, type, io, length, buff);
    if(page_id == PAGE_MUSIC || page_id == PAGE_OUTPUT1 ||
        page_id == PAGE_OUTPUT2 || page_id == PAGE_VSS)
    {
        void *chain = audio_service_get_playback_afx_chain();
        libc_memcpy(eq_param_buf, &length, sizeof(uint32_t));
        if(buff != 0) {
            libc_memcpy(eq_param_buf + sizeof(uint32_t), buff, length);
            //printf_debug("playback setting chain : %x\n", chain);
            afx_chain_cmd(chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, (uint32_t)(eq_param_buf));
        }
    }
    else if(page_id == PAGE_MIC || page_id == PAGE_EFFECT)
    {
        void *chain = audio_service_get_record_afx_chain();
        {
            libc_memcpy(eq_param_buf, &length, sizeof(uint32_t));
            if(buff != 0) {
                libc_memcpy(eq_param_buf + sizeof(uint32_t), buff, length);
                //printf_debug("record setting chain : %x\n", chain);
                afx_chain_cmd(chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, (uint32_t)(eq_param_buf));
            }
        }
    }
    else if(page_id & 0xFE && type & 0xFE)
    {
        if( REC == io )
        {
            void *record_chain = audio_service_get_record_afx_chain();
            loge("record reset\n");
            libc_memcpy(eq_param_buf, &length, sizeof(uint32_t));
            if(buff != 0) {
                libc_memcpy(eq_param_buf + sizeof(uint32_t), buff, length);
                afx_chain_cmd(record_chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, (uint32_t)(eq_param_buf));
                return;
            }
        }
        else if( PLBK == io )
        {
            void *plbk_chain = audio_service_get_playback_afx_chain();
            loge("plbk reset\n");
            libc_memcpy(eq_param_buf, &length, sizeof(uint32_t));
            if(buff != 0) {
                libc_memcpy(eq_param_buf + sizeof(uint32_t), buff, length);
                afx_chain_cmd(plbk_chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, (uint32_t)(eq_param_buf));
                return;
            }
        }
        else if( RP_ALL == io)
        {
            void *record_chain = audio_service_get_record_afx_chain();
            loge("record reset\n");
            libc_memcpy(eq_param_buf, &length, sizeof(uint32_t));
            if(buff != 0) {
                libc_memcpy(eq_param_buf + sizeof(uint32_t), buff, length);
                afx_chain_cmd(record_chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, (uint32_t)(eq_param_buf));
            }
            get_eq_param_buff();
            void *plbk_chain = audio_service_get_playback_afx_chain();
            loge("plbk reset\n");
            libc_memcpy(eq_param_buf, &length, sizeof(uint32_t));
            if(buff != 0) {
                libc_memcpy(eq_param_buf + sizeof(uint32_t), buff, length);
                afx_chain_cmd(plbk_chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, (uint32_t)(eq_param_buf));
            }
        }
    }
}

void on_get_eq_data(uint8_t page_id, uint8_t *buff)
{
    printf_debug("on_get_eq_data,page_id=%d\n",page_id);
}

extern int32_t getConfigedHelperPtrSize(int8_t io);
extern int32_t getHelperPtrSize(int8_t io);

void on_get_available_space(uint32_t* total_space_in_byte, uint32_t* available_space_in_byte)
{
#if EQ_REALTIME
    *total_space_in_byte     = getConfigedHelperPtrSize(0);        //byte
    *available_space_in_byte = getHelperPtrSize(0);
    loge("on_get_available_space ----- %d/%d", *available_space_in_byte, *total_space_in_byte);
#else
    *total_space_in_byte = *available_space_in_byte = 0;
#endif
}

#include "hal_clk_lib.h"
extern uint32_t sys_get_cpu0_load_per();
extern uint32_t sys_get_cpu1_load_per();

void on_get_aprx_cpuload(uint32_t* cpu0_load_per, uint32_t* cpu1_load_per)
{
#if EQ_REALTIME
    __maybe_unused uint32_t cpu1_rate = clk_get_rate(CLK_CPU1)/1000000;
    *cpu0_load_per = sys_get_cpu0_load_per();
    *cpu1_load_per = sys_get_cpu1_load_per();
    loge("---- cpu0:%d%, %d%, cpu1(%d)", *cpu0_load_per, *cpu1_load_per, cpu1_rate);
#else
    *cpu0_load_per = 0;
    *cpu1_load_per = 0;
#endif
}

void on_get_module_space(uint8_t* buffer)
{
    //buffer[3* i] : type
    //buffer[3* i + 1] , buffer[3* i + 2] :   module space in byte
    // 0xFF 0xFF 0xFF : end

    buffer[0] = 30; //tone
    *((WORD*)(buffer+1)) = 2*1024;

    buffer[3] = 90; //mix
    *((WORD*)(buffer+4)) = 5*1024;
}

/*
    PC is ready for receive data from device,
    call function "COMP_HID_Send_Data"  to send data to pc
    the max_size of buffer is 64
*/
void on_usb_eq_prepare_ready()
{
    logi("pc is ready for receive");
    event_put(EQ_EVENT_UPLOAD_RAM_EQ);
}

#if EQ_LOAD_EN
uint8_t eq_profile  = 0;
uint8_t rec_loaded  = 0;
uint8_t plbk_loaded = 0;
TimerHandle_t EqLoadTimerHandle;

void on_eq_unloaded(uint8_t io)
{
    taskENTER_CRITICAL();
    if(REC_IN == io)
        rec_loaded  = 0;
    else
        plbk_loaded = 0;
    taskEXIT_CRITICAL();
}

AT(.eqsetting_load)
void eq_load_internal()
{
    extern bool load_eq_setting(int8_t idx, uint8_t io);
    if(!rec_loaded){
        if(load_eq_setting(eq_profile, REC_IN)){
            rec_loaded = 1;
        }
    }
    if(!plbk_loaded){
        if(load_eq_setting(eq_profile, PLBK_OUT)){
            plbk_loaded = 1;
        }
    }
}

AT(.eqsetting_load)
void bank_debug()
{
    static uint32_t last = 0;
    static uint32_t cur  = 0;
    cur = *(uint32_t *)0x3000004c;
    if(last) {
        logd("bank : %d", cur - last);
    }
    last = cur;
    logd("c0 : %d, c1 : %d", clk_get_rate(CLK_CPU0_SYS), clk_get_rate(CLK_CPU1));
}

AT(.eqsetting_load)
void eq_load_timer_cb(xTimerHandle xTimer)
{
    eq_load_internal();
#if EQ_LOAD_SHOW_STATUS
    bank_debug();
    logd("eq_load_timer_cb: rec(%d), plbk(%d)", rec_loaded, plbk_loaded);
#endif
}

void on_eq_profile_change(uint8_t new_profile)
{
    taskENTER_CRITICAL();
    if(new_profile != eq_profile){
        eq_profile  = new_profile;
        rec_loaded  = 0;
        plbk_loaded = 0;
    }
    taskEXIT_CRITICAL();
}
#else
AT(.eqsetting_load)
void eq_load_internal()
{
}
#endif


