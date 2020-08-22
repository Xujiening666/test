#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "snd_hardware.h"
#include "i2s.h"
#include "twi.h"
#include "wm8978.h"
#include "tas5711.h"


#if (0 == I2S0_EN)
struct snd_hardware *get_sndi2s_obj(void)
{
    return NULL;
}

void i2s_init(void)
{
}

#else

AT(.i2s_flash_seg)
int app_sndi2s_params_early(enum pcm_hw_direction dir,
    struct pcm_hw_params *params)
{
    return 0;
}

AT(.i2s_flash_seg)
int app_sndi2s_params(enum pcm_hw_direction dir,
    struct pcm_hw_params *params)
{
    return 0;
}

AT(.i2s_flash_seg)
int app_sndi2s_free_early(enum pcm_hw_direction dir)
{
    switch (dir) {
    case PCM_HW_DIR_OUT:

#if I2S0_USE_TAS5711
        tas5711_Mute();
#endif
        break;

    case PCM_HW_DIR_IN:
        break;
    }

    return 0;
}

AT(.i2s_flash_seg)
int app_sndi2s_free(enum pcm_hw_direction dir)
{
    return 0;
}

AT(.i2s_flash_seg)
int app_sndi2s_prepare_early(enum pcm_hw_direction dir)
{
    return 0;
}

AT(.i2s_flash_seg)
int app_sndi2s_prepare(enum pcm_hw_direction dir)
{
    switch (dir) {
    case PCM_HW_DIR_OUT:
#if I2S0_USE_TAS5711
        tas5711_Unmute();
#endif
        break;
    case PCM_HW_DIR_IN:
        break;
    }
    return 0;
}


AT(.i2s_flash_seg)
int app_sndi2s_trigger_early(enum pcm_hw_direction dir,
    enum pcm_hw_trigger_cmd cmd)
{
    return 0;
}

AT(.i2s_flash_seg)
int app_sndi2s_trigger(enum pcm_hw_direction dir,
    enum pcm_hw_trigger_cmd cmd)
{
    return 0;
}


AT(.i2s_flash_seg)
void i2s_init(void)
{
#if I2S0_EN
    clk_enable(AUDIO_PLL_CLK);
    module_ccu_enable(CCU_DMAC);
    module_ccu_enable(CCU_I2S);
    clk_enable(CLK_I2S);

    pin_config(I2S0_BCLK_PIN_CFG);
    pin_config(I2S0_LRCK_PIN_CFG);
#if I2S0_TX_EN
    pin_config(I2S0_TX_PIN_CFG);
#endif
#if I2S0_RX_EN
    pin_config(I2S0_RX_PIN_CFG);
#endif
#if I2S0_MCLK_EN
    pin_config(I2S0_MCLK_PIN_CFG);
#endif

    i2s_init_params_t params;
    params.mclk_factor = I2S0_MCLK_FACTOR;              // mclk分频比（e.g. mclk设置成256倍采样率，则配置成256）
    params.format = I2S0_FORMAT;                        //0:标准I2S; 1:左对齐模式（left-justified）; 2:右对齐模式（right-justified）
    params.slot_width = I2S0_SLOT_WIDTH;                //0:16; 1:32
    params.sample_resolution = I2S0_SAMPLE_RESOLUTION;  //0:16bit; 1:20bit; 2:24bit; 3:32bit
    params.slot_num = I2S0_SLOT_NUM;                    //0:1-slot, 1:2-slot; 2:4-slot; 3:8-slot; 4:16-slot
    params.tx_min_fs = 8000;                            //I2S TX最小支持多大的采样率
    i2s_module_init(&params);

#if (I2S0_USE_WM8978 || I2S0_USE_TAS5711)
    twi_init();
#endif

#if I2S0_USE_WM8978
    WM8978_Init();
#endif

#if I2S0_USE_TAS5711
    pin_config(PIN_C4|PIN_FUNCOUTPUT|PIN_HIGH);//power on
    tas5711_Init(); //delay before write reg
    tas5711_Mute();
    load_tas5711_reg();
    tas5711_SetVol(32);
#endif

#endif
}

AT(.i2s_flash_seg)
void i2s_deinit(void)
{
#if I2S0_EN
    i2s_module_deinit();
#endif
}

#endif

