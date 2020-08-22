#include "sys_inc.h"
#include "user_config.h"
#include "codec.h"
#include "system.h"

#if (!AUDIO_ADJUST_HPLR_LINEOUTLR) || ((1 != AUDIO_OUT_DEVICE_SEL) && (1 != AUDIO_IN_DEVICE_SEL))
void anc_irq_init(void){}
void anc_irq_deinit(void){}
#endif

#if ((1 != AUDIO_OUT_DEVICE_SEL) && (1 != AUDIO_IN_DEVICE_SEL))
struct snd_hardware *get_sndcodec_obj(void){return NULL;}
void auss_micbias_enable(bool enable){}
void auss_enter_sleep(void){}

bool anc_lineout_is_enable(void){return false;}

void snd_spk_unmute(void){}
void snd_spk_mute(void){}

void codec_init(void){}
void codec_deinit(void){}
HAL_StatusTypeDef HAL_AUSS_SetCtl(uint32_t ctl, uint32_t val){return HAL_ERROR;}
#else


AT(.auss_flash_seg)
static void auss_ccu_init(void)
{
#if ((1 == AUDIO_OUT_DEVICE_SEL) || (1 == AUDIO_IN_DEVICE_SEL))
    module_ccu_enable(CCU_DMAC);
    module_ccu_enable(CCU_AUSS);
    module_ccu_enable(CCU_BTSS);
    clk_enable(CLK_AUSS);

    if (CHIP_VER_A == sys_get_ver()) {
        /* see RF spec */
        *(__IO uint32_t*)0x40011000 |= (0x1 << 8);
        *(__IO uint32_t*)0x40011040 |= (0x1 << 0) | (0x1 << 2);
    }
#endif
}

extern bool linein_detect_state(void);


/* 扬声器功放解mute(此函数将会被audio驱动调用) */
AT(.auss_sram_seg)
void snd_spk_unmute(void)
{
	if(linein_detect_state()){
		return;
	}

#if ((1 == AUDIO_OUT_DEVICE_SEL) || (1 == AUDIO_IN_DEVICE_SEL))
#if (AUDIO_USE_MUTE == 1)
    uint32_t val = (AUDIO_MUTE_PIN_STATE == PIN_LOW) ? PIN_HIGH : PIN_LOW;
    pin_set_value(AUDIO_MUTE_PIN, val); /* 使用GPIO unmute */
#elif (AUDIO_USE_MUTE == 2)
    auss_micbias_enable(true);          /* 使用mic bias PIN unmute */
#endif
#endif
}

/* 扬声器功放mute(此函数将会被audio驱动调用) */
AT(.auss_sram_seg)
void snd_spk_mute(void)
{

#if ((1 == AUDIO_OUT_DEVICE_SEL) || (1 == AUDIO_IN_DEVICE_SEL))
#if (AUDIO_USE_MUTE == 1)
    pin_set_value(AUDIO_MUTE_PIN, AUDIO_MUTE_PIN_STATE); /* 使用GPIO mute */
#elif (AUDIO_USE_MUTE == 2)
    auss_micbias_enable(false);                          /* 使用mic bias PIN mute */
#endif
#endif
}

AT(.auss_flash_seg)
void codec_init(void)
{
#if ((1 == AUDIO_OUT_DEVICE_SEL) || (1 == AUDIO_IN_DEVICE_SEL))
    auss_module_init_params_t params;

#if (AUDIO_DMICDATA0_EN || AUDIO_DMICDATA1_EN)
    pin_config(AUDIO_DMIC_CLK_PIN_CFG); /* 数字MIC CLK引脚配置 */
#endif

#if AUDIO_DMICDATA0_EN
    pin_config(AUDIO_DMICDATA0_PIN_CFG); /* 数字MICDATA0 引脚配置 */
#endif

#if AUDIO_DMICDATA1_EN
    pin_config(AUDIO_DMICDATA1_PIN_CFG); /* 数字MICDATA1 引脚配置 */
#endif

#if AUDIO_USE_MUTE
    /* speaker mute gpio */
    pin_config(AUDIO_MUTE_PIN | PIN_FUNCOUTPUT | AUDIO_MUTE_PIN_STATE);
#endif

    auss_ccu_init();

    params.features = AUSS_FEATURES_DAC_SDM2;
    params.features |= AUSS_FEATURES_HEADSET_OPTIMISE;
#if (AUDIO_PHONE_OUTPUT_SEL == 0)
    params.features |= AUSS_FEATURES_LINEIN_OPTIMISE;
#endif

#if (AUDIO_USE_MUTE == 2)
        params.features |= AUSS_FEATURES_MICBIAS_FOR_MUTE;
#endif

#if AUDIO_ADJUST_HPLR_LINEOUTLR
    params.features |= AUSS_FEATURES_ANC_LINEOUT;
#endif
    auss_module_init(&params);
#endif
}

AT(.auss_flash_seg)
void codec_deinit(void)
{
#if ((1 == AUDIO_OUT_DEVICE_SEL) || (1 == AUDIO_IN_DEVICE_SEL))
    auss_module_deinit();
#endif
}
#endif
