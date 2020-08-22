#define LOG_LEV 4
#include "sys_inc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "user_config.h"
#include "spdif.h"
#include "libc_common.h"
#include "event.h"
#include "audio_out.h"
#include "audio_service.h"

AT(.spdif_init_seg)
void spdif_lock_cb(void)
{
#if SPDIF_EN
    loge("spdif_lock_cb");
    spdif_module_set_online_status(true);
#endif
}

AT(.spdif_init_seg)
void spdif_unlock_cb(void)
{
#if SPDIF_EN
    loge("spdif_unlock_cb");
    spdif_module_set_online_status(false);
#endif
}

AT(.spdif_init_seg)
void spdif_init(void)
{
#if SPDIF_EN
    spdif_param_t param;
    module_ccu_enable(CCU_SPDIF);
    clk_enable(CLK_SPDIF);
    pin_config(SPDIF_PIN_CFG);

#if (SPDIF_PIN_SEL == 0)
    param.channel = SPDIF_CHANNEL_A;
#elif (SPDIF_PIN_SEL == 1)
    param.channel = SPDIF_CHANNEL_B;
#endif
    param.lock_callback = spdif_lock_cb;
    param.unlock_callback = spdif_unlock_cb;
    spdif_module_init(&param);
    spdif_module_rxinit();
#endif
}

AT(.spdif_init_seg)
void spdif_deinit(void)
{
#if SPDIF_EN
    spdif_module_rxdeinit();
    spdif_module_deinit();
#endif
}
