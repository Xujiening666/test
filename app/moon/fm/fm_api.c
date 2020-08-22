#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "fm_api.h"
#include "fm.h"

AT(.fm_seg)
void fm_clk_init(void)
{
#if (FM_CLK_PIN_SEL > 0)
    pin_config(FM_CLK_PIN_CFG);

    //0:32K_HOSC(square); 1:HOSC(24M,square); 2:12M(square); 3:LOSC(square);
#if (FM_CLK_SEL == 0)
    clk_set_src(CLK_FM, CLK_32K_HOSC);
#elif (FM_CLK_SEL == 1)
    clk_set_src(CLK_FM, CLK_24M_HOSC);
#elif (FM_CLK_SEL == 2)
    clk_set_src(CLK_FM, CLK_12M_HOSC);
#elif (FM_CLK_SEL == 3)
    clk_set_src(CLK_FM, CLK_LOSC);
#endif

    clk_enable(CLK_FM);
#endif
}

AT(.fm_seg)
void fm_clk_deinit(void)
{
#if (FM_CLK_PIN_SEL > 0)
    clk_disable(CLK_FM);
#endif
}

AT(.fm_seg)
void fm_get_id(void)
{
    fm_mode.id = FM_ID_NONE;

#if FM_RDA5807_EN
    if (rda5807_online()) {
        fm_mode.id = FM_ID_RDA5807;
    }
#endif

#if FM_QN8065_EN
    if (qn8065_online()) {
        fm_mode.id = FM_ID_QN8065;
    }
#endif

#if FM_QN8035_EN
    if (qn8035_online()) {
        fm_mode.id = FM_ID_QN8035;
    }
#endif
}

AT(.fm_seg)
void fm_init(void)
{
    fm_clk_init();

    switch(fm_mode.id) {
#if FM_RDA5807_EN
    case FM_ID_RDA5807:
        rda5807_init();
        break;
#endif

#if FM_QN8065_EN
    case FM_ID_QN8065:
        qn8065_init();
        break;
#endif

#if FM_QN8035_EN
    case FM_ID_QN8035:
        qn8035_init();
        break;
#endif
    }
}

AT(.fm_seg)
void fm_deinit(void)
{
    switch(fm_mode.id) {
#if FM_RDA5807_EN
    case FM_ID_RDA5807:
        rda5807_off();
        break;
#endif

#if FM_QN8065_EN
    case FM_ID_QN8065:
        qn8065_off();
        break;
#endif

#if FM_QN8035_EN
    case FM_ID_QN8035:
        qn8035_powerdown();
        break;
#endif
    }

    fm_clk_deinit();
}

AT(.fm_seg)
void fm_tune2ch(uint16_t freq)
{
    switch(fm_mode.id) {
#if FM_RDA5807_EN
    case FM_ID_RDA5807:
        rda5807_seek_tune(freq);
#endif

#if FM_QN8065_EN
    case FM_ID_QN8065:
        qn8065_tune2ch(freq);
        break;
#endif

#if FM_QN8035_EN
    case FM_ID_QN8035:
        QND_TuneToCH(freq*10);
        break;
#endif

    }
}

AT(.fm_seg)
uint8_t fm_seek(uint16_t freq)
{
    uint8_t ret = 0;

    switch(fm_mode.id) {
#if FM_RDA5807_EN
    case FM_ID_RDA5807:
        ret = rda5807_seek(freq);
        break;
#endif

#if FM_QN8065_EN
    case FM_ID_QN8065:
        ret = qn8065_seek(freq);
        break;
#endif

#if FM_QN8035_EN
    case FM_ID_QN8035:
        ret = qn8035_seek(freq);
        break;
#endif
    }

    return ret;
}

AT(.fm_seg)
void fm_set_vol(uint8_t vol)
{
    switch(fm_mode.id) {
#if FM_RDA5807_EN
    case FM_ID_RDA5807:
        rda5807_set_vol(vol);
        break;
#endif

#if FM_QN8065_EN
    case FM_ID_QN8065:
        qn8065_set_vol(vol);
        break;
#endif

#if FM_QN8035_EN
    case FM_ID_QN8035:
        qn8035_set_vol(vol);
        break;
#endif
    }
}

