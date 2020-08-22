#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "ledc.h"

uint32_t led_lt[3]; //for light led enable

extern uint8_t led_buf[LED_COM_NUM];
extern const uint8_t led_truth_tbl[LED_MAX_NUM];


AT(.ledc_rodata_seg)
const uint8_t led_truth_tbl[LED_MAX_NUM] =
{
#if (SYS_PLATFORM == PLATFORM_FPGA)
    //0---------1---------2---------3---------4---------5---------6---------7---------
    LED_1A,   LED_1B,   LED_1E,   LED_AUX,  LED_TF,   LED_NULL, LED_NULL, LED_NULL,

    //8---------9---------10--------11--------12--------13--------14--------15--------
    LED_1F,   LED_2A,   LED_2B,   LED_2E,   LED_2D,   LED_NULL, LED_NULL, LED_NULL,

    //16--------17--------18--------19--------20--------21--------22--------23--------
    LED_1G,  LED_2F,    LED_COL,  LED_3B,   LED_FM,   LED_BAT0, LED_NULL, LED_NULL,

    //24--------25--------26--------27--------28--------29--------30--------31--------
    LED_1C,   LED_2G,   LED_3F,   LED_3C,   LED_4E,   LED_NULL, LED_NULL, LED_NULL,

    //32--------33--------34--------35--------36--------37--------38--------39--------
    LED_1D,   LED_2C,   LED_3G,   LED_3A,   LED_4C,   LED_4G,   LED_NULL, LED_NULL,

    //40--------41--------42--------43--------44--------45--------46--------47--------
    LED_3D,   LED_USB,  LED_3E,   LED_4D,   LED_4F,   LED_4B,   LED_NULL, LED_NULL,

    //48--------49--------50--------51--------52--------53--------54--------55--------
    LED_NULL, LED_NULL, LED_BAT1, LED_NULL, LED_NULL, LED_4A,   LED_NULL, LED_NULL,

    //56--------57--------58--------59--------60--------61--------62--------63--------
    LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL,

    //64--------65--------66--------67--------68--------69--------70--------71--------
    LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL,

#else
    //0---------1---------2---------3---------4---------5---------6---------7-------
    LED_1A,   LED_1B,   LED_1E,   LED_AUX,  LED_NULL, LED_NULL, LED_TF,   LED_NULL,

    //8---------9---------10--------11--------12--------13--------14--------15------
    LED_1F,   LED_2A,   LED_2B,   LED_2E,   LED_NULL, LED_NULL, LED_2D,   LED_NULL,

    //16--------17--------18--------19--------20--------21--------22--------23------
    LED_1G,  LED_2F,    LED_COL,  LED_3B,   LED_NULL, LED_NULL, LED_FM,   LED_BAT0,

    //24--------25--------26--------27--------28--------29--------30--------31------
    LED_1C,   LED_2G,   LED_3F,   LED_3C,   LED_NULL, LED_NULL, LED_4E,   LED_NULL,

    //32--------33--------34--------35--------36--------37--------38--------39------
    LED_1D,   LED_2C,   LED_3G,   LED_3A,   LED_NULL, LED_NULL, LED_4C,   LED_4G,

    //40--------41--------42--------43--------44--------45--------46--------47------
    LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL,

    //48--------49--------50--------51--------52--------53--------54--------55------
    LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL,

    //56--------57--------58--------59--------60--------61--------62--------63------
    LED_3D,   LED_USB,  LED_3E,   LED_4D,   LED_4F,   LED_NULL, LED_NULL, LED_4B,

    //64--------65--------66--------67--------68--------69--------70--------71------
    LED_NULL, LED_NULL, LED_BAT1, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_4A,

#endif
};

AT(.ledc_init_seg)
void ledc_init(void)
{
    pin_config(LEDC_D0_PIN_CFG);
    pin_config(LEDC_D1_PIN_CFG);
    pin_config(LEDC_D2_PIN_CFG);
    pin_config(LEDC_D3_PIN_CFG);
    pin_config(LEDC_D4_PIN_CFG);
#if (SYS_PLATFORM == PLATFORM_FPGA)
    pin_config(LEDC_D5_PIN_CFG);
    pin_config(LEDC_D6_PIN_CFG);
#else
    pin_config(LEDC_D7_PIN_CFG);
    pin_config(LEDC_D8_PIN_CFG);
#endif

    ledc_param.clk_src = LEDC_CLK_LOSC;
    if(ledc_param.clk_src == LEDC_CLK_100K) {
        ledc_param.clk_div = 9;         //(9+1)/100k = 0.1ms
    } else {
        ledc_param.clk_div = 2;         //(2+1)/32k = 0.094ms
    }
    ledc_param.keep_time = 4;           //(4+1) *0.1 = 0.5ms

    ledc_param.det_en = LEDC_DET_NONE;
    ledc_param.int_en = LEDC_INT_NONE;
    ledc_param.det_dbc = 0xFF;

    ledc_module_init();
}

AT(.ledc_init_seg)
void ledc_deinit(void)
{
    ledc_module_deinit();
}

AT(.ledc_init_seg)
void ledc_null_get(uint32_t *led_null)
{
    //the bit of null LED, config to 1
    for(uint32_t m=0; m<LED_MAX_NUM; m++) {
        if(LED_NULL == led_truth_tbl[m]) {
            if(m < 32) {
                led_null[0] |= BIT(m);
            } else if(m < 64) {
                led_null[1] |= BIT(m-32);
            } else {
                led_null[2] |= BIT(m-64);
            }

            led_null[3]++;     //null cnt
        }
    }
}

AT(.ledc_seg)
void ledc_lt_get(uint32_t led_num)
{
    uint32_t n;
    for(n=0; n<LED_MAX_NUM; n++) {
        if(led_num == led_truth_tbl[n]) {
            break;
        }
    }

    if(n < 32) {
        led_lt[0] |= BIT(n);
    } else if (n < 64) {
        led_lt[1] |= BIT(n-32);
    } else {
        led_lt[2] |= BIT(n-64);
    }
}

AT(.ledc_seg)
void ledc_update(void)
{
    uint32_t n;

    led_lt[0] = 0;
    led_lt[1] = 0;
    led_lt[2] = 0;

    for(uint32_t group=0; group<LED_COM_NUM; group++) {
        for(uint32_t seg=0; seg<=LED_SEG_NUM; seg++) {
            if(led_buf[group] & BIT(seg)) {
                n = group<<4;
                ledc_lt_get(n+seg+1);
            }
        }
    }

    ledc_lt_update(led_lt);
}

AT(.ledc_seg)
void ledc_irq_cb(uint32_t reg)
{
    if (reg & LEDC_INT_D0_FALL) {
    }
    if (reg & LEDC_INT_D1_FALL) {
    }
    if (reg & LEDC_INT_D2_FALL) {
    }
    if (reg & LEDC_INT_D3_FALL) {
    }
    if (reg & LEDC_INT_D4_FALL) {
    }
    if (reg & LEDC_INT_D5_FALL) {
    }
    if (reg & LEDC_INT_D6_FALL) {
    }
    if (reg & LEDC_INT_D7_FALL) {
    }
    if (reg & LEDC_INT_D8_FALL) {
    }


    if (reg & LEDC_INT_D0_RISE) {
    }
    if (reg & LEDC_INT_D1_RISE) {
    }
    if (reg & LEDC_INT_D2_RISE) {
    }
    if (reg & LEDC_INT_D3_RISE) {
    }
    if (reg & LEDC_INT_D4_RISE) {
    }
    if (reg & LEDC_INT_D5_RISE) {
    }
    if (reg & LEDC_INT_D6_RISE) {
    }
    if (reg & LEDC_INT_D7_RISE) {
    }
    if (reg & LEDC_INT_D8_RISE) {
    }

}

