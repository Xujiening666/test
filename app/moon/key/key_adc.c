#define LOG_LEV 5
#define SLTK_LOG_TAG "kadc"
#include "sys_inc.h"
#include "user_config.h"
#include "key.h"
#include "info_mem.h"


#if KEY_AD_EN
#define NO_SL_DEBUGBOARD    1
kadc_t kadc;
kadc_param_t kadc_param[KADC_CH_MAX];
#if KADC_KNOB_EN
kadc_vol_t kadc_vol;
#endif

bool hp_detach;

typedef enum {
    KEY_FUNC1   = KEY_PLAY,
    KEY_FUNC2   = KEY_VOL_UP,//KEY_PLAY,
    KEY_FUNC3   = KEY_VOL_DOWN,//KEY_NEXT,
    KEY_FUNC4   = KEY_NEXT,//KEY_VOL_DOWN,
    KEY_FUNC5   = KEY_PLAY,
    KEY_FUNC6   = KEY_UNBIND_USER,
    KEY_FUNC7   = KEY_NONE,
    KEY_FUNC8   = KEY_NONE,
    KEY_FUNC9   = KEY_NONE,
    KEY_FUNC10  = KEY_NONE,
    KEY_FUNC11  = KEY_NONE,
    KEY_FUNC12  = KEY_NONE,
    KEY_FUNC13  = KEY_NONE,
    KEY_FUNC14  = KEY_NONE,
    KEY_FUNC15  = KEY_NONE,
    KEY_FUNC16  = KEY_NONE,
} keyfunc_adc_t;

AT(.key_init_rodata_seg)
const uint32_t kadc_init_config[KADC_CH_MAX][3] = {
    //en
    //irq sel
    //threshold(0x000 ~ 0xFFF), t_cnt(0x0 ~ 0xF), t_dbc(0x0 ~ 0xF), mode

    //adc0:
    {KADC0_EN,
#if KADC0_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc1:
    {KADC1_EN,
#if KADC1_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc2:
    {KADC2_EN,
#if KADC2_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc3:
    {KADC3_EN,
#if KADC3_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc4:
    {KADC4_EN,
#if KADC4_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc5:
    {KADC5_EN,
#if KADC5_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif
};

#if KADC0_KNOB
AT(.key_rodata_seg)
const uint16_t knob0_tbl[KNOB0_VOL_MAX] = {
    0x880,              // 0
    0x900,              // 1
    0x980,              // 2
    0xA00,              // 3
    0xA80,              // 4
    0xB00,              // 5
    0xB80,              // 6
    0xC00,              // 7
    0xC80,              // 8
    0xD00,              // 9
    0xE00,              // 10
    0xF00,              // 11
};
#endif

#if KADC1_KNOB
AT(.key_rodata_seg)
const uint16_t knob1_tbl[KNOB1_VOL_MAX] = {
    0x080,              // 0
    0x100,              // 1
    0x1A0,              // 2
    0x240,              // 3
    0x2E0,              // 4
    0x380,              // 5
    0x420,              // 6
    0x4C0,              // 7
    0x560,              // 8
    0x600,              // 9
    0x700,              // 10
    0xF00,              // 11
};
#endif

#if KADC2_KNOB
AT(.key_rodata_seg)
const uint16_t knob2_tbl[KNOB2_VOL_MAX] = {
    0x080,              // 0
    0x100,              // 1
    0x1A0,              // 2
    0x240,              // 3
    0x2E0,              // 4
    0x380,              // 5
    0x420,              // 6
    0x4C0,              // 7
    0x560,              // 8
    0x600,              // 9
    0x700,              // 10
    0xF00,              // 11
};
#endif


#if KADC4_KNOB
AT(.key_rodata_seg)
const uint16_t knob4_tbl[KNOB4_VOL_MAX] = {
    0x600,              // 0
    0x680,              // 1
    0x700,              // 2
    0x780,              // 3
    0xF00,              // 4
};
#endif


AT(.key_rodata_seg)
const uint16_t share1_tbl[SHARE1_VOL_MAX] = {
    2000,               // 0
    4000,               // 1
    6000,               // 2
    8000,               // 3
    12000,              // 4
};

AT(.kadc_seg)
uint8_t knob_vol_get(uint32_t adc_value, uint8_t adc_ch, uint8_t vol_max)
{
    uint16_t *p;
    uint8_t n;

    p = 0;
#if KADC0_KNOB
    if (adc_ch == KADC_CH0) {
        p = (uint16_t *)knob0_tbl;
    }
#endif
#if KADC1_KNOB
    if (adc_ch == KADC_CH1) {
        p = (uint16_t *)knob1_tbl;
    }
#endif
#if KADC2_KNOB
    if (adc_ch == KADC_CH2) {
        p = (uint16_t *)knob2_tbl;
    }
#endif
#if KADC3_KNOB
    if (adc_ch == KADC_CH3) {
        p = (uint16_t *)knob3_tbl;
    }
#endif
#if KADC4_KNOB
    if (adc_ch == KADC_CH4) {
        p = (uint16_t *)knob4_tbl;
    }
#endif
#if KADC5_KNOB
    if (adc_ch == KADC_CH5) {
        p = (uint16_t *)knob5_tbl;
    }
#endif

#if KADC_SHARE1
    if (adc_ch == KADC_CH_SHARE1) {
        p = (uint16_t *)share1_tbl;
    }
#endif

    for (n=0;n<vol_max;n++) {
        if (adc_value <= *(p+n)) {
            break;
        }
    }
    return n;
}

AT(.kadc_init_seg)
void knob_vol_init(void)
{
#if KADC0_KNOB
    kadc_vol.knob0_val = KNOB_INIT;
    kadc_vol.knob0_step = KNOB0_VOL_MAX;
#endif
#if KADC1_KNOB
    kadc_vol.knob1_val = KNOB_INIT;
    kadc_vol.knob1_step = KNOB1_VOL_MAX;
#endif
#if KADC2_KNOB
    kadc_vol.knob2_val = KNOB_INIT;
    kadc_vol.knob2_step = KNOB2_VOL_MAX;
#endif
#if KADC3_KNOB
    kadc_vol.knob3_val = KNOB_INIT;
    kadc_vol.knob3_step = KNOB3_VOL_MAX;
#endif
#if KADC4_KNOB
    kadc_vol.knob4_val = KNOB_INIT;
    kadc_vol.knob4_step = KNOB4_VOL_MAX;
#if KADC_SHARE1
    kadc_share1_init();
#endif
#endif
#if KADC5_KNOB
    kadc_vol.knob5_val = KNOB_INIT;
    kadc_vol.knob5_step = KNOB5_VOL_MAX;
#endif
}

AT(.kadc_init_seg)
void kadc_ccu_init(void)
{
    module_ccu_enable(CCU_KADC);
    cdelay(2000);           //delay >2000 cycles
    //kadc use LOSC, the CCU clk enable only control the HOSC clk src,
    //so no need to enable the clk
    //clk_enable(CLK_KADC);   //KADC clock enable
}

AT(.kadc_init_seg)
void kadc_gpio_init(void)
{
#if KADC0_EN
    pin_set_func(KADC0_PIN_NUM, KADC0_FUNC);
    hp_detach = false;
#endif
#if KADC1_EN
    pin_set_func(KADC1_PIN_NUM, KADC1_FUNC);
#endif
#if KADC2_EN
    pin_set_func(KADC2_PIN_NUM, KADC2_FUNC);
#endif
#if KADC3_EN
    pin_set_func(KADC3_PIN_NUM, KADC3_FUNC);
#endif
#if KADC4_EN
    pin_set_func(KADC4_PIN_NUM, KADC4_FUNC);
#endif
#if KADC5_EN
    pin_set_func(KADC5_PIN_NUM, KADC5_FUNC);
#endif
}

AT(.kadc_init_seg)
void kadc_wakeup_init(void)
{
#if SLEEP_EN
#if KADC0_EN && !KADC0_IRQ
    pin_irq_clk_config(KADC0_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC1_EN && !KADC1_IRQ
    pin_irq_clk_config(KADC1_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC2_EN && !KADC2_IRQ
    pin_irq_clk_config(KADC2_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC3_EN && !KADC3_IRQ
    pin_irq_clk_config(KADC3_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC4_EN && !KADC4_IRQ
    pin_irq_clk_config(KADC4_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC5_EN && !KADC5_IRQ
    pin_irq_clk_config(KADC5_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#endif
}

AT(.kadc_init_seg)
void kadc_init(void)
{
    kadc_ccu_init();
    kadc_gpio_init();
    knob_vol_init();
    libc_memcpy(kadc_param, kadc_init_config, KADC_CH_MAX*sizeof(kadc_param_t));
    kadc_module_init(kadc_param);
    kadc_wakeup_init();
    mdelay(5);
    kadc_cal_en();
#if KADC_IRQ_EN
    kadc_irq_en();
#endif
}

AT(.kadc_seg)
void kadc_irq_cb(uint32_t pending, uint32_t *kdata)
{
    sleep_delay_set();

#if KADC_IRQ_EN

    uint32_t kadc_ret;
    uint32_t kadc_data;

    for (uint32_t ch=0; ch<KADC_CH_MAX; ch++) {
        if (kadc_param[ch].en == 0) {
            continue;
        }

        kadc_ret = KEY_EVENT_NONE;
        kadc_data = kdata[ch];

        if (pending & (KADC_INTS_KEYDOWN_PENDING<<(4*ch))) {
            //logi("kdown-%x:%x", ch,kadc_data);
            kadc.val = kadc_get_key(kadc_data);
            kadc.cnt = 0;
            kadc.long_sta = 0;
            kadc_ret = kadc.val | KEY_EVENT_S;          //short
        }

        if (pending & (KADC_INTS_DATA_PENDING<<(4*ch))) {
            //logi("kdata-%x:%x", ch,kadc_data);
            kadc.cnt++;
            if(kadc.long_sta) {
                if(kadc.cnt == KEY_ADC_CNT_LH) {
                    kadc_ret = kadc.val | KEY_EVENT_H;  //hold
                    kadc.cnt = KEY_ADC_CNT_L;
                }
            } else if(kadc.cnt == KEY_ADC_CNT_L) {
                kadc.long_sta = 1;
                kadc_ret = kadc.val | KEY_EVENT_L;      //long
            }
        }

        if (pending & (KADC_INTS_KEYUP_PENDING<<(4*ch))) {
            //logi("kup-%x:%x", ch,kadc_data);
            if(kadc.long_sta) {
                kadc_ret = kadc.val | KEY_EVENT_LU;     //long up
            } else {
                kadc_ret = kadc.val | KEY_EVENT_SU;     //short up
            }
        }

        if ((kadc_ret & KEY_MASK) != KEY_NONE) {
            if ((KEY_EVENT_SU | KEY_MODE) == kadc_ret) {
                mode_update(1);
                key_mode = true;
            }
            //logi("k:%x", kadc_ret);
            event_put(kadc_ret);
        }
    }

#endif
}

AT(.kadc_seg)
bool hp_get_get_detach_state(void)
{
    printf_debug("**%s,%d hp_detach=%d\r",__func__,__LINE__,hp_detach);
    return hp_detach;
}

AT(.kadc_seg)
uint32_t kadc_scan_get_key(void)
{
    uint32_t kret;
    kret = KEY_NONE;
    __maybe_unused uint8_t private_flag;
#if KADC_SCAN_EN

    uint32_t kval;
#if KADC_KNOB_EN
    uint32_t kstep;
#endif

#if (KADC0_EN && !KADC0_IRQ)
    kval = kadc_data_get(KADC_CH0);
#if NO_SL_DEBUGBOARD
    if (KNOB0_THRESHOLD < kval && hp_detach) {
//		info_get(INFO_PRIVATE_MODE_FLAG, (void *)&private_flag, 1);
//		if(private_flag != 1) //非隐私模式
//		{
            hp_detach = false;
			HP_DICON_CONFIG();
#if !maikefeng
		    event_put(KEY_EVENT_SU | KEY_HP_OUT);
#endif
//		}
    } else if (kval < KNOB0_THRESHOLD && !hp_detach) {
        info_get(INFO_PRIVATE_MODE_FLAG, (void *)&private_flag, 1);
		if(private_flag != 1)//非隐私模式
		{
			hp_detach = true;
			HP_CON_CONFIG();
#if !maikefeng
			event_put(KEY_EVENT_SU | KEY_HP_IN);
#endif
		}
    }
#endif
    
    //printf_debug("kval0:%x\n", kval);
    if (kval < KNOB0_THRESHOLD) {
        //logi("kval:0x%x", kval);
        //logi("cal_0:%x", *(uint32_t *)0x40096004 & 0xFFF);
        kret = kadc_get_key(kval);
		//printf_debug("kval0:%x\n", kret);
        return kret;
    }
#if KADC0_KNOB
    else {
        if (((kval > kadc_vol.knob0_val) && (kval-kadc_vol.knob0_val > KNOB0_DIFF)) || \
            ((kadc_vol.knob0_val > kval) && (kadc_vol.knob0_val - kval > KNOB0_DIFF))) {
            kadc_vol.knob0_val = kval;
            kstep = knob_vol_get(kval, KADC_CH0, KNOB0_VOL_MAX);
            if (kstep != kadc_vol.knob0_step) {
                kadc_vol.knob0_step = kstep;
                //TODO: use event_put() to put new event.
                logi("val0:%x, step0: %d", kval, kstep);
            }
        }
    }
#endif
#endif

#if (KADC1_EN && !KADC1_IRQ)
    kval = kadc_data_get(KADC_CH1);
    //logi("kval1:%x", kval);
#if KADC1_KNOB
    if (((kval > kadc_vol.knob1_val) && (kval-kadc_vol.knob1_val > KNOB1_DIFF)) || \
        ((kadc_vol.knob1_val > kval) && (kadc_vol.knob1_val - kval > KNOB1_DIFF))) {
        kadc_vol.knob1_val = kval;
        kstep = knob_vol_get(kval, KADC_CH1, KNOB1_VOL_MAX);
        if (kstep != kadc_vol.knob1_step) {
            kadc_vol.knob1_step = kstep;
            //TODO: use event_put() to put new event.
            logi("val1:%x, step1: %d", kval, kstep);
        }
    }
#endif
#endif

#if (KADC2_EN && !KADC2_IRQ)
    kval = kadc_data_get(KADC_CH2);
    //logi("kval2:%x", kval);
#if KADC2_KNOB
    if (((kval > kadc_vol.knob2_val) && (kval-kadc_vol.knob2_val > KNOB2_DIFF)) || \
        ((kadc_vol.knob2_val > kval) && (kadc_vol.knob2_val - kval > KNOB2_DIFF))) {
        kadc_vol.knob2_val = kval;
        kstep = knob_vol_get(kval, KADC_CH2, KNOB2_VOL_MAX);
        if (kstep != kadc_vol.knob2_step) {
            kadc_vol.knob2_step = kstep;
            //TODO: use event_put() to put new event.
            logi("val2:%x, step2: %d", kval, kstep);
        }
    }
#endif
#endif

#if (KADC3_EN && !KADC3_IRQ)
    kval = kadc_data_get(KADC_CH3);
    logi("kval3:%x", kval);
#endif

#if (KADC4_EN && !KADC4_IRQ)
    kval = kadc_data_get(KADC_CH4);
    logi("kval4:%x", kval);

#if KADC4_KNOB
#if KADC_SHARE1
    if (kadc_vol.share1_num == 1) {

#endif
        //logi("kval4:%x", kval);
        if (((kval > kadc_vol.knob4_val) && (kval-kadc_vol.knob4_val > KNOB4_DIFF)) || \
            ((kadc_vol.knob4_val > kval) && (kadc_vol.knob4_val - kval > KNOB4_DIFF))) {
            kadc_vol.knob4_val = kval;
            kstep = knob_vol_get(kval, KADC_CH4, KNOB4_VOL_MAX);
            if (kstep != kadc_vol.knob4_step) {
                kadc_vol.knob4_step = kstep;
                //TODO: use event_put() to put new event.
                logi("val4:%x, step4: %d", kval, kstep);
            }
        }
#if KADC_SHARE1
    }
    kadc_share1(kval);
#endif
#endif
#endif
#if (KADC5_EN && !KADC5_IRQ)
    kval = kadc_data_get(KADC_CH5);
    logi("kval5:%x", kval);
#endif

#endif

    return kret;
}

AT(.kadc_seg)
uint32_t kadc_get_key(uint32_t data)
{
    uint32_t val;

    val = KEY_NONE;
    //logi("[%x]",data);
/* Modify by arunboy 20200528 麦克风*/
#if maikefeng
	if(data < 0x40) {
        val = KEY_FUNC2;
	} else if(data < 0x190) {
        val = KEY_FUNC3;
	}
	return val;
#endif
    if(data < 0x40) {
        val = KEY_FUNC1;
    } else if (data < 0x60) {
        val = KEY_FUNC6;
    } else if(data < 0x90) {
        val = KEY_FUNC2;
    } else if(data < 0xd0) {
        val = KEY_FUNC3;
    }/* else if(data < 0x400) {
        val = KEY_FUNC4;
    } else if(data < 0xF00) {
        val = KEY_FUNC5;
    } */
    //printf_debug("arunboy val = %d\n",val);    
    return val;
}


#if KADC_SHARE1
/*
*  pull-up 10K, pull-down 10k, vr1 10K, vr2 10K
*
*  vr1 = 10000*0xFFF / kval - 20000
*  vr1_temp = vr1 + 10000
*  vr2_temp = vr1_temp*kval / (0xFFF-kval)
*  vr2 = vr2_temp*10000/(10000-vr2_temp)
*/

AT(.kadc_init_seg)
void kadc_share1_init(void)
{
    pin_config(SHARE1_SUB_PIN | PIN_DRI3 | PIN_FUNC15); //disable
    kadc_vol.share1_num = 1;
}

AT(.kadc_seg)
void kadc_share1(uint32_t kval)
{
    uint32_t vr1_temp;
    uint32_t vr2_temp;
    uint32_t vr2;
    uint32_t kstep;

    if (kadc_vol.share1_num == 1) {
        if (kval == 0) {
            return;
        }
        kadc_vol.share1_vr1 = (10000*0xFFF)/kval;
        if (kadc_vol.share1_vr1 > 20000) {
            kadc_vol.share1_vr1 -= 20000;
        } else {
            kadc_vol.share1_vr1 = 0;
        }
        //logi("vr1:%d", kadc_vol.share1_vr1);

        pin_set_value(SHARE1_SUB_PIN, 0);
        pin_set_func(SHARE1_SUB_PIN, PIN_FUNCOUTPUT);   //ouput
        kadc_vol.share1_num = 2;
    } else if (kadc_vol.share1_num == 2) {
        vr1_temp = kadc_vol.share1_vr1 + 10000;
        if (kval >= 0xFFF) {
            goto next;
        }
        vr2_temp = vr1_temp*kval/(0xFFF-kval);
        if (vr2_temp >= 10000) {
            goto next;
        }
        vr2 = vr2_temp*10000/(10000-vr2_temp);
        //logi("vr2:%d]", vr2);

        if (((vr2 > kadc_vol.share1_val) && (vr2-kadc_vol.share1_val > SHARE1_DIFF)) || \
            ((kadc_vol.share1_val > vr2) && (kadc_vol.share1_val - vr2 > SHARE1_DIFF))) {
            kadc_vol.share1_val = vr2;
            kstep = knob_vol_get(vr2, KADC_CH_SHARE1, SHARE1_VOL_MAX);
            if (kstep != kadc_vol.share1_step) {
                kadc_vol.share1_step = kstep;
                //TODO: use event_put() to put new event.
                logi("vr2:%d, step1: %d", vr2, kstep);
            }
        }
next:
        pin_set_func(SHARE1_SUB_PIN, PIN_FUNC15);       //disable
        kadc_vol.share1_num = 1;
    }
}
#endif

#endif

