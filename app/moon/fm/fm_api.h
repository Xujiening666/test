#ifndef _FM_API_H_
#define _FM_API_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"
#include "twi.h"
#include "fm_rda5807.h"
#include "fm_qn8065.h"
#include "fm_qn8035.h"

#if (FM_CLK_PIN_SEL == 0)
#elif (FM_CLK_PIN_SEL == 1)
#define FM_CLK_PIN_CFG  (PIN_B12 | PIN_DRI3 | PIN_FUNC2)
#elif (FM_CLK_PIN_SEL == 2)
#define FM_CLK_PIN_CFG  (PIN_B1 | PIN_DRI3 | PIN_FUNC3)
#elif (FM_CLK_PIN_SEL == 3)
#define FM_CLK_PIN_CFG  (PIN_B8 | PIN_DRI3 | PIN_FUNC2)
#elif (FM_CLK_PIN_SEL == 4)
#define FM_CLK_PIN_CFG  (PIN_B13 | PIN_DRI3 | PIN_FUNC3)
#elif (FM_CLK_PIN_SEL == 5)
#define FM_CLK_PIN_CFG  (PIN_C4 | PIN_DRI3 | PIN_FUNC3)
#elif (FM_CLK_PIN_SEL == 6)
#define FM_CLK_PIN_CFG  (PIN_D0 | PIN_DRI3 | PIN_FUNC11)
#endif

#if (FM_TWI_SEL == 1)
#define FM_TWI_ID           TWI_ID_01
#elif (FM_TWI_SEL == 2)
#define FM_TWI_ID           TWI_ID_02
#endif


#define FM_TWI_ADDR         (0x20>>1)
#define FM_CH_MAX           50

#define FM_FREQ_MIN         875
#define FM_FREQ_MAX         1080
#define FM_FREQ_STEP        1

#define FM_VOLUME           10

#define FM_CLK_32K_HOSC     0   //32K_HOSC(square)
#define FM_CLK_24M_HOSC     1   //HOSC(24M,square)
#define FM_CLK_12M          2   //12M(square)
#define FM_CLK_LOSC         3   //LOSC(square)
#define FM_CLK_32768_OSC    4   //32768 OSC(sine)


enum fm_id_e {
    FM_ID_NONE          = 0,
    FM_ID_RDA5807,
    FM_ID_QN8065,
    FM_ID_QN8035,
    FM_ID_BK1080,
};


void fm_clk_init(void);
void fm_clk_deinit(void);
void fm_get_id(void);
void fm_init(void);
void fm_deinit(void);
void fm_tune2ch(uint16_t freq);
uint8_t fm_seek(uint16_t freq);
void fm_set_vol(uint8_t vol);


#ifdef __cplusplus
}
#endif

#endif /* _FM_API_H_ */
