#ifndef _IRRX_H_
#define _IRRX_H_


#if (IRRX_PIN_SEL == 0)
#define IRRX_PIN_CFG    (PIN_B0 | PIN_FUNC2)
#elif (IRRX_PIN_SEL == 1)
#define IRRX_PIN_CFG    (PIN_D1 | PIN_FUNC10)
#elif (IRRX_PIN_SEL == 2)
#define IRRX_PIN_CFG    (PIN_C3 | PIN_FUNC3)
#endif


void irrx_init(void);

void irrx_module_init(IRRX_InitTypeDef *param);
uint16_t irrx_get_usercode(void);
uint8_t irrx_get_datacode(void);
void irrx_set_wakeup(bool enable);

#endif

