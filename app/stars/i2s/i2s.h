#ifndef _I2S_H_
#define _I2S_H_

#include "sys_inc.h"
#include "user_config.h"
#include "sndlib.h"

#if (I2S0_BCLK_PIN_SEL == 0)
#define I2S0_BCLK_PIN_CFG       (PIN_B7 | PIN_FUNC5 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 1)
#define I2S0_BCLK_PIN_CFG       (PIN_B4 | PIN_FUNC2 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 2)
#define I2S0_BCLK_PIN_CFG       (PIN_A13 | PIN_FUNC2 | PIN_DRI3)
#endif

#if (I2S0_LRCK_PIN_SEL == 0)
#define I2S0_LRCK_PIN_CFG       (PIN_B6 | PIN_FUNC5 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 1)
#define I2S0_LRCK_PIN_CFG       (PIN_B5 | PIN_FUNC2 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 2)
#define I2S0_LRCK_PIN_CFG       (PIN_A14 | PIN_FUNC2 | PIN_DRI3)
#endif

#if (I2S0_TX_PIN_SEL== 0)
#define I2S0_TX_PIN_CFG         (PIN_B8 | PIN_FUNC5 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 1)
#define I2S0_TX_PIN_CFG         (PIN_B6 | PIN_FUNC2 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 2)
#define I2S0_TX_PIN_CFG         (PIN_A11 | PIN_FUNC2 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 3)
#define I2S0_TX_PIN_CFG         (PIN_B10 | PIN_FUNC3 | PIN_DRI3)
#endif

#if (I2S0_RX_PIN_SEL == 0)
#define I2S0_RX_PIN_CFG         (PIN_B2 | PIN_FUNC7 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 1)
#define I2S0_RX_PIN_CFG         (PIN_B10 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 2)
#define I2S0_RX_PIN_CFG         (PIN_A12 | PIN_FUNC2 | PIN_DRI3)
#endif

#if (I2S0_MCLK_PIN_SEL == 0)
#define I2S0_MCLK_PIN_CFG       (PIN_B5 | PIN_FUNC5 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 1)
#define I2S0_MCLK_PIN_CFG       (PIN_B11 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 2)
#define I2S0_MCLK_PIN_CFG       (PIN_A10 | PIN_FUNC2 | PIN_DRI3)
#endif


void i2s_init(void);
void i2s_deinit(void);

void i2s_module_init(i2s_init_params_t *params);
void i2s_module_deinit(void);


#endif

