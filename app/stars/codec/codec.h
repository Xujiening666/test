#ifndef _CODEC_H_
#define _CODEC_H_

#include "hal_auss_lib.h"

#if (AUDIO_DMIC_CLK_PIN_SEL == 0)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B5 | PIN_FUNC2)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 1)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_C5 | PIN_FUNC3)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 2)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B3 | PIN_FUNC2)
#endif

#if (AUDIO_DMICDATA0_PIN_SEL == 0)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B6 | PIN_FUNC2)
#elif (AUDIO_DMICDATA0_PIN_SEL == 1)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_C6 | PIN_FUNC3)
#elif (AUDIO_DMICDATA0_PIN_SEL == 2)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B4 | PIN_FUNC2)
#endif

#if (AUDIO_DMICDATA1_PIN_SEL == 0)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B7 | PIN_FUNC2)
#elif (AUDIO_DMICDATA1_PIN_SEL == 1)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_C7 | PIN_FUNC3)
#endif


void codec_init(void);
void auss_module_init(auss_module_init_params_t *params);
void auss_module_deinit(void);
void snd_spk_unmute(void);
void snd_spk_mute(void);


#endif

