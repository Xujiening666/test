#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "pmu.h"
#include "gpio.h"
#include "device.h"
#include "key.h"

GPIO_USB_CB gpiob_usb_cb;

AT(.gpio_init_seg)
void gpio_clk_init(void)
{
    module_ccu_enable(CCU_GPIO);
}

AT(.gpio_init_seg)
void gpiob_irq_en(void)
{
    INTC_SetPriority(IRQn_GPIOB, IRQn_GPIOB_PRIO);
    INTC_SetVector(IRQn_GPIOB, (uint32_t)gpiob_cb);
    INTC_EnableIRQ(IRQn_GPIOB);
}

AT(.gpio_init_seg)
void gpiob_irq_dis(void)
{
    INTC_DisableIRQ(IRQn_GPIOB);
}

AT(.gpio_init_seg)
void gpioc_irq_en(void)
{
    INTC_SetPriority(IRQn_GPIOC, IRQn_GPIOC_PRIO);
    INTC_SetVector(IRQn_GPIOC, (uint32_t)gpioc_cb);
    INTC_EnableIRQ(IRQn_GPIOC);
}

AT(.gpio_init_seg)
void gpioc_irq_dis(void)
{
    INTC_DisableIRQ(IRQn_GPIOC);
}

AT(.gpio_init_seg)
void gpio_irq_en(void)
{
#if (USB_DETECT_EN || SD_DETECT_EN || KIO_GPIOB || KEY_CODING_EN || (KADC_SCAN_EN&&SLEEP_EN))
    gpiob_irq_en();
#endif

#if (PMU_EXIST || KIO_GPIOC)
    gpioc_irq_en();
#endif
}

AT(.gpio_seg)
void gpiob_cb(void)
{
    sleep_delay_set();

#if USB_DETECT_EN
    if (pin_get_irq_pending(USB_DP_PIN_NUM)) {
        pin_clear_irq_pending(USB_DP_PIN_NUM);
        if(gpiob_usb_cb != NULL){
            gpiob_usb_cb();
        }
    }
    if (pin_get_irq_pending(USB_DM_PIN_NUM)) {
        pin_clear_irq_pending(USB_DM_PIN_NUM);
    }
#endif

#if SD_DETECT_EN
    if (pin_get_irq_pending(SD_DETECT_PIN_NUM)) {
        pin_clear_irq_pending(SD_DETECT_PIN_NUM);
    }
#endif

#if (KEY_IO_EN && KIO_GPIOB)
    kio_get_sta();
#endif

#if KEY_CODING_EN
    kcd_get_sta();
#endif

#if (KADC_SCAN_EN&&SLEEP_EN)
#if KADC0_EN
    if (pin_get_irq_pending(KADC0_PIN_NUM)) {
        pin_clear_irq_pending(KADC0_PIN_NUM);
    }
#endif
#if KADC1_EN
    if (pin_get_irq_pending(KADC1_PIN_NUM)) {
        pin_clear_irq_pending(KADC1_PIN_NUM);
    }
#endif
#if KADC2_EN
    if (pin_get_irq_pending(KADC2_PIN_NUM)) {
        pin_clear_irq_pending(KADC2_PIN_NUM);
    }
#endif
#if KADC3_EN
    if (pin_get_irq_pending(KADC3_PIN_NUM)) {
        pin_clear_irq_pending(KADC3_PIN_NUM);
    }
#endif
#if KADC4_EN
    if (pin_get_irq_pending(KADC4_PIN_NUM)) {
        pin_clear_irq_pending(KADC4_PIN_NUM);
    }
#endif
#if KADC5_EN
    if (pin_get_irq_pending(KADC5_PIN_NUM)) {
        pin_clear_irq_pending(KADC5_PIN_NUM);
    }
#endif
#endif
}

AT(.gpio_seg)
void gpioc_cb(void)
{
    sleep_delay_set();

#if (KEY_IO_EN && KIO_GPIOC)
    kio_get_sta();
#endif

#if PMU_EXIST
    pmu_irq_proc();
#endif
}

