#ifndef _SPDIF_H_
#define _SPDIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (SPDIF_PIN_SEL == 0)
#define SPDIF_PIN_CFG     (PIN_D0 | PIN_FUNC10)
#elif (SPDIF_PIN_SEL == 1)
#define SPDIF_PIN_CFG     (PIN_D1 | PIN_FUNC11)
#endif

void spdif_init(void);
void spdif_deinit(void);


#ifdef __cplusplus
}
#endif

#endif

