#ifndef _HAL_CLK_LIB_H_
#define _HAL_CLK_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"

#define SRC_EXT_LOSC   32768
#define SRC_32K_HOSC   32000
#define SRC_12M_HOSC   12000000
#define SRC_24M_HOSC   24000000
#define SRC_50K_RCOSC  50000

typedef struct vf_table {
    uint32_t freq;
    uint32_t volt;
} vf_table_t;

enum clk_sram_speed_e {
    SRAM_NORMAL,
    SRAM_HIGH,
};

enum ccu_id_e {
    CCU_NORF = 0, // 0
    CCU_DMAC,     // 1
    CCU_BTSS,     // 2
    CCU_USB,      // 3
    CCU_SDMMC,    // 4
    CCU_AUSS,     // 5
    CCU_SIC,      // 6
    CCU_ASRC,     // 7

    CCU_IR = 12,  //12
    CCU_LEDC,     //13

    CCU_PWM = 16,
    CCU_TIMER,
    CCU_GPIO,
    CCU_UART0,
    CCU_UART1,
    CCU_TRNG,
    CCU_SPI,

    CCU_TWI1 = 24,
    CCU_TWI2,
    CCU_KADC,
    CCU_I2S,
    CCU_SPDIF,  //28
    CCU_EID,
    CCU_TKC,    //30
    CCU_UART2,  //31
};


/*
  *  CLK_50K_RCOSC-->| 0
  *                                  |LOSC_SEL-->CLK_LOSC-->|0
  *  CLK_EXT_LOSC---->| 1                                       |
  *                                                                              |-->CLK32K_SRC_SEL-->CLK_32K
  *                                                                              |
  *  CLK_32K_HOSC----------------------------->|1
  *
*/
enum clk_id_e {
    //rom audio use the number 0, so we can not change
    //we just can follow
    AUDIO_PLL_CLK,    /* 0 */
    AUDIO_PLLX8_CLK,  /* 1 */
    CORE_PLL_CLK,     /* 2 */
    CLK_CPU_SYS,      /* 3 */
    CLK_CPU0_SYS,     /* 4 */
    CLK_CPU1,         /* 5 */
    CLK_AHB,          /* 6 */
    CLK_APB,          /* 7 */
    CLK_96M,          /* 8 */
    CLK_48M,          /* 9 */
    CLK_24M_HOSC,     /* 10 */
    CLK_12M_HOSC,     /* 11 HOSC / 2 */
    CLK_32K_HOSC,     /* 12 HOSC / 750 */
    CLK_EXT_LOSC,     /* 13 external LOSC 32768Hz */
    CLK_50K_RCOSC,    /* 14 50K +/- 20K */
    CLK_32K,          /* 15 from CLK_32K_HOSC or CLK_LOSC */
    CLK_LOSC,         /* 16 from CLK_50K_RC or CLK_EXT_LOSC */
    CLK_SDMMC,        /* 17 */
    CLK_USB,          /* 18 */
    CLK_AUSS,         /* 19 */
    CLK_I2S,          /* 20 */
    CLK_UART0,        /* 21 */
    CLK_UART1,        /* 22 */
    CLK_TWI1,         /* 23 */
    CLK_TWI2,         /* 24 */
    CLK_KADC,         /* 25 */
    CLK_TRNG,         /* 26 */
    CLK_EID,          /* 27 */
    CLK_FM,           /* 28 */
    CLK_BTSS,         /* 29 */
    CLK_SIC,          /* 30 */
    CLK_SPDIF,        /* 31 */
    CLK_TKC,          /* 32 */
    CLK_GPIO,         /* 33 */
    CLK_IR,           /* 34 FIXME:sl2801 not support, sl6800 support */
    CLK_LEDC,         /* 35 FIXME:sl2801 not support, sl6800 support */
    CLK_UART2,        /* 36 FIXME:sl2801 not support, sl6800 support */
    CLK_ASRC,         /* 37 FIXME:sl2801 not support, sl6800 support */
    CLK_SYSTICK,      /* 38 FIXME:sl2801 not support, sl6800 support */
    CLK_INVALID = 100,/* 100 */
};

void clk_enable(enum clk_id_e clk_id);
void clk_disable(enum clk_id_e clk_id);

void clk_set_div(enum clk_id_e clk_id, uint32_t div);
uint32_t clk_get_div(enum clk_id_e clk_id);

void clk_set_rate(enum clk_id_e clk_id, uint32_t rate);
uint32_t clk_get_rate(enum clk_id_e clk_id);

void clk_set_src(enum clk_id_e clk_id, enum clk_id_e src);
enum clk_id_e clk_get_src(enum clk_id_e clk_id);

void module_reset();
bool module_is_enable(enum ccu_id_e ccu_id);
void module_ccu_enable(enum ccu_id_e ccu_id);
void module_ccu_disable(enum ccu_id_e ccu_id);

void set_corepll_clk(uint32_t rate);
uint32_t get_corepll_clk(void);

void clk_set_cpu1_div(uint32_t div);
void clk_set_cpu1_rate(uint32_t rate);
void clk_set_cpu0_div(uint32_t div);
void clk_set_cpux_freq(uint32_t cpu1_freq, uint32_t cpu0_div);
void auto_set_cpux_freq(void);

void clk_bypass_usb_suspend_clk(bool bypass);
void clk_reset_cpu1(void);
void clk_dcxo_set(uint16_t dcxo_current, bool dcxo_lp_en, bool xo32k_en);
void ccu_init(void);
void ccu_init_sram(void);
void ccu_reinit(uint32_t volt, uint32_t pll, uint32_t sys_div, uint32_t cpu0_div, uint32_t ahb_div, uint32_t nor_div);
int32_t clk_test(void);

void audiopll_sleep(void);
void set_audiopll_rate_ex(uint32_t rate);
void adjust_audiopll(uint32_t rate);
uint32_t get_audiopll_rate(void);
void clk_set_sram_speed(enum clk_sram_speed_e sram_speed);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_CLK_LIB_H_ */

