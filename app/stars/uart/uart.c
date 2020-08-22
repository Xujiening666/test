#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "uart.h"
#include "key.h"
#include "ring_buffer.h"
#include "hal_dma_lib.h"
#include "app_afx_config.h"

#if EQ_AFX_API_TEST
#include "eq_process.h"
#include "audio_service.h"

#define AFX_API_TEST_FUNC __attribute__((section(".afx_api_test_func_seg")))
#define MAX_SUBFUNC_APIS_NUM  16

typedef enum {
    STATE_PREPARE,
    STATE_GET_FID,
    STATE_GET_PARA,
}FUNC_GET_STATE;

uint8_t   cmd_cache_state   = 0;
uint8_t   cmd_cache_it      = 0;
uint8_t   func_get_state    = STATE_PREPARE;
uint8_t   func_id           = 0;
uint32_t  para[8]           = {0};
uint32_t  para_it           = 0;

uint8_t   cmd_cache[32]     = {0};

AFX_API_TEST_FUNC void reset_afx_test()
{
    func_get_state = STATE_PREPARE;
    func_id = 0;
    libc_memset(para, 0x00, 8*sizeof(uint32_t));
    para_it = 0;
    libc_memset(cmd_cache, 0x00, 32);
    cmd_cache_it = 0;
}

AFX_API_TEST_FUNC void call_test_func()
{
    switch(func_id) {
        case API_NONE:
        {
            loge("bypass");
            on_set_bypass_profile();
            break;
        }
        case API_CLEAR_HOLDERS:
        {
            loge("clear holders");
            clear_all_holder();
            break;
        }
        case API_REVB_ON_SET_DEFAULT_REVB:
        {
            loge("revb(%d,%d)", para[0], para[1]);
            on_set_default_reverb_profile(para[0], para[1]);
            break;
        }
        case API_REVB_ON_SWITCH_MS:
        {
            loge("revb ms(%d)", para[0]);
            on_set_rec_reverb_ms_switch(para[0]);
            break;
        }
        case API_REVB_ON_SET_GWET:
        {
            loge("revb-gwet(%d)", para[0]);
            on_set_reverb_gwet(para[0]);
            break;
        }
        case API_REVE_ON_SET_RT60:
        {
            loge("revb-rt60(%d)", para[0]);
            on_set_reverb_time(para[0]);
            break;
        }
        case API_ECHO_ON_SET_DEFAULT_ECHO:
        {
            loge("echo(%d, %d)", para[0], para[1]);
            on_set_default_echo_profile(para[0], para[1]);
            break;
        }
        case API_ECHO_ON_SET_DELAY:
        {
            loge("echo - dly(%d)", para[0]);
            on_set_echo_delay(para[0]);
            break;
        }
        case API_ECHO_ON_SET_GWET:
        {
            loge("echo- gwet", para[0]);
            on_set_echo_gwet(para[0]);
            break;
        }
        case API_TSPS_ON_SET_FEMALE:
        {
            loge("female(%d)");
            on_set_female_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_MALE:
        {
            loge("male(%d)");
            on_set_male_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_ROBOT:
        {
            loge("robot(%d)");
            on_set_robot_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_KID:
        {
            loge("kid(%d)");
            on_set_kid_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_VC_CHANGE:
        {
            loge("vc(%d)");
            on_set_voc_change_profile(para[0]);
            break;
        }
        case API_EQ_ON_SET_RADIO_PROFILE:
        {
            loge("radio(%d)");
            on_set_radio_profile(para[0]);
            break;
        }
        case API_EQ_ON_SET_TREB_BASS_GAIN:
        {
            loge("ebg(%d, %d, %d)",para[0], para[1], para[2]);
            on_set_treb_bass_gain(para[0], para[1], para[2]);
            break;
        }
        case API_EQ_ON_SET_PREAMP:
        {
            loge("epreamp(%d)", para[0]);
            on_set_eq_focus_preamp(para[0]);
            break;
        }
        case API_EQ_ON_SET_ATUNE:
        {
            loge("atune (%d, %d)", para[0], para[1]);
            on_set_autotune_profile(para[0], para[1]);
            atune_setting as = {0};
            on_x_atune_for_pageid(PAGE_MIC, ATUNE_ALL_PAPAS_MASK, &as, EQ_GET_DATA);
            loge("%s", as.enable?"on":"dis");
            loge("note:%d", as.note);
            loge("oct:%d",  as.oct);
            loge("scale:%d",as.scale);
            loge("speed:%d",      as.speed);
            break;
        }
        case API_DRC_ON_SET_NGATE:
        {
            loge("ng (%d, %d)", para[0], para[1]);
            on_set_natural_noise_gate(para[0], para[1]);
            break;
        }
        case API_VSS_ON_SET_SPK_PROFILE:
        {
            loge("vss spk(%d)");
            on_set_default_vss_spk_profile(para[0]);
            break;
        }
        case API_VSS_ON_SET_HP_PROFILE:
        {
            loge("vss hp(%d)");
            on_set_default_vss_hp_profile(para[0]);
            break;
        }
        case API_EQ5_ROCK_MIC:
        {
            loge("rock mic");
            on_set_rock_mic_eq_profile(para[0]);
            break;
        }
        case API_EQ5_SONIC_BOOM:
        {
            loge("sonic boom");
            on_set_sonic_boom_eq_profile(para[0]);
            break;
        }
        case API_AS_TEST:
        {
            loge("as test");
            audio_service_cmd(para[0], para[1]);
            break;
        }
    }
}

AFX_API_TEST_FUNC void parse_cmd_cache()
{
    int data = 0;
    func_get_state = STATE_PREPARE;
    for(int i = 0; i < cmd_cache_it; i++) {
        data = cmd_cache[i];
        //loge("data : %d", data);
        if(func_get_state == STATE_PREPARE)
        {
            if(data == 'f') {
                func_get_state = STATE_GET_FID;
                func_id = API_NONE;
            }
            else {
                reset_afx_test();
            }
        }
        else if(func_get_state == STATE_GET_FID) {
            //loge("fid");
            if(data >= '0' && data <= '9') {
                func_id = 10*func_id + (data - '0');
            }
            else if( data == '(' ) {
                func_get_state = STATE_GET_PARA;
                para_it = 0;
            }
            else {
                reset_afx_test();
            }
        }
        else if(func_get_state == STATE_GET_PARA) {
            //loge("para");
            if(data == ',') {//next
                para_it++;
            }
            else if(data == ')') {//end to call func
                //loge("call, func : %d", func_id);
                call_test_func();
                reset_afx_test();
            }
            else {
                para[para_it] = 10*para[para_it] + (data - '0');
            }
        }
        else {
            reset_afx_test();
        }
    }
}
#endif

#if (UART0_EN || UART1_EN || UART2_EN)

extern uart_param_t uart_param[UART_NUM];

AT(.uart_init_seg)
void uart_init(void)
{
#if UART0_EN
    pin_config(UART0_RX_PIN_CFG);
    pin_config(UART0_TX_PIN_CFG);

    uart_param[0].baudrate = UART0_BAUDRATE;
    uart_param[0].clksrc = UART0_SCLK_FREQ;
    uart_param[0].flowctl = 0;  //uart0 not support flowctl
    uart_param[0].fifo = 0;     //uart0 not support fifo mode

    uart0_module_init();

#if KEY_UART_EN
    uart_register_rx_callback(UART_ID_00, uart_rx_key);
#endif
#endif

#if UART1_EN
    pin_config(UART1_RX_PIN_CFG);
    pin_config(UART1_TX_PIN_CFG);

    uart_param[1].baudrate = UART1_BAUDRATE;
    uart_param[1].clksrc = UART1_SCLK_FREQ;
    uart_param[1].flowctl = UART1_HWFLOWCTL;
    uart_param[1].fifo = UART1_FIFO_EN;
    uart_param[1].dma  = UART1_DMA_EN;

    if (UART1_HWFLOWCTL) {
        pin_config(UART1_RTS_PIN_CFG);
        pin_config(UART1_CTS_PIN_CFG);
    }

    uart1_module_init();
#if KEY_UART_EN
    uart_register_rx_callback(UART_ID_01, uart_rx_key);
#endif
#endif

#if UART2_EN
    pin_config(UART2_RX_PIN_CFG);
    pin_config(UART2_TX_PIN_CFG);

    uart_param[2].baudrate = UART2_BAUDRATE;
    uart_param[2].clksrc = UART2_SCLK_FREQ;
    uart_param[2].flowctl = UART2_HWFLOWCTL;
    uart_param[2].fifo = UART2_FIFO_EN;
    uart_param[2].dma = UART2_DMA_EN;

    if (UART2_HWFLOWCTL) {
        pin_config(UART2_RTS_PIN_CFG);
        pin_config(UART2_CTS_PIN_CFG);
    }

    uart2_module_init();
 #if KEY_UART_EN
    uart_register_rx_callback(UART_ID_02, uart_rx_key);
#endif
#endif

    uart_debug_init();
}

AT(.uart_init_seg)
void uart_debug_init(void)
{
#if DEBUG_LOG_EN
    uart_dbg_init(UART_ID_00);
#endif
}

AT(.uart_dma_init_seg)
void uart_uart1_dma_callback(uint8_t *rx_data, uint8_t len)
{
#if UART1_EN
#if UART1_DMA_EN
    printf_array(rx_data,len);
#endif
#endif
}

AT(.uart_dma_init_seg)
void uart_uart2_dma_callback(uint8_t *rx_data, uint8_t len)
{
#if UART2_EN
#if UART2_DMA_EN
    printf_array(rx_data,len);
#endif
#endif
}

AT(.uart_dma_init_seg)
void uart_dma_mode_init(void)
{
#if (UART1_EN && UART1_DMA_EN)
    uart_module_dma_rx_init(UART_ID_01,DMA_DATA_LEN);
    uart_register_dma_rx_callback(UART_ID_01,uart_uart1_dma_callback);
    uart_module_dma_tx_init(UART_ID_01,DMA_DATA_LEN);
#endif

#if (UART2_EN && UART2_DMA_EN)
    uart_module_dma_rx_init(UART_ID_02,DMA_DATA_LEN);
    uart_register_dma_rx_callback(UART_ID_02,uart_uart2_dma_callback);
    uart_module_dma_tx_init(UART_ID_02,DMA_DATA_LEN);
#endif
}

#if KEY_UART_EN
AT(.uart_sram_seg)
void uart_send_event(uint32_t event)
{
    event_put(event);
}

AT(.uart_sram_seg)
void uart_rx_key(uint32_t data)
{
#if EQ_AFX_API_TEST
    if(cmd_cache_state == 0) {
#endif
    switch (data) {
    case 'p':
        uart_send_event(KEY_EVENT_SU | KEY_PREV);
        break;

    case 'n':
        uart_send_event(KEY_EVENT_SU | KEY_NEXT);
        break;

    case '+':
        uart_send_event(KEY_EVENT_SU | KEY_VOL_UP);
        break;

    case '-':
        uart_send_event(KEY_EVENT_SU | KEY_VOL_DOWN);
        break;

    case ' ':
        uart_send_event(KEY_EVENT_SU | KEY_PLAY);
        break;

    case 'm':
        mode_update(1);
        uart_send_event(KEY_EVENT_SU | KEY_MODE);
        break;

    case 'e':
        uart_send_event(KEY_EVENT_SU | KEY_EQ);
        break;

	case 0x0D:
		logi(" ");
		break;

	case '1':
		logi(" p1 ");
        *(uint32_t *)0x40037ffc ^= 1<<1;
		break;

    case '2':
        logi(" p2 ");
        *(uint32_t *)0x40037ffc ^= 1<<2;
        break;

    case '3':
        logi(" p3 ");
        *(uint32_t *)0x40037ffc ^= 1<<3;
        break;

    case '4':
        logi(" p4 ");
        *(uint32_t *)0x40037ffc ^= 1<<4;
        break;

    case '5':
        logi(" p5 ");
        *(uint32_t *)0x40037ffc ^= 1<<5;
        break;

	case '6':
		logi(" p6 ");
        *(uint32_t *)0x40037ffc ^= 1<<6;
		break;

    case '7':
        logi(" p7 ");
        *(uint32_t *)0x40037ffc ^= 1<<7;
        break;

    case '8':
        logi(" p8 ");
        *(uint32_t *)0x40037ffc ^= 1<<8;
        break;

    case '9':
        logi(" p9 ");
        *(uint32_t *)0x40037ffc ^= 1<<9;
        break;

    case '0':
        logi(" p10 ");
        *(uint32_t *)0x40037ffc ^= 1<<10;
        break;
#if EQ_AFX_API_TEST
        case '#':
            cmd_cache_state = 1;
            cmd_cache_it    = 0;
            break;
        case '@':
            reset_afx_test();
            break;
#endif
        }
#if EQ_AFX_API_TEST
    }
    else {
        if(data == '#') {
            cmd_cache_state = 0;
            uart_send_event(EQ_EVENT_AFX_TEST);
        }
        else {
            cmd_cache[cmd_cache_it++] = data & 0xff;
        }
    }
#endif
}
#endif

void uart_test(void)
{
    for (uint8_t i=0;i<10;i++) {
        uart_tx(UART_ID_00, 0xc0+i);
    }
    while(1);
}

#else

void uart_init(void)
{
}

void uart_dma_mode_init(void)
{
}

#endif

