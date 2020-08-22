#ifndef _GPIO_H_
#define _GPIO_H_


#if (SYS_PLATFORM == PLATFORM_FPGA)
#define GPIO_INT_CLK_SEL 0x0 //LOSC
#else
#define GPIO_INT_CLK_SEL 0x1 //HOSC
#endif

typedef void (*GPIO_USB_CB)();
extern GPIO_USB_CB gpiob_usb_cb;

void gpio_clk_init(void);
void gpiob_irq_en(void);
void gpiob_irq_dis(void);
void gpioc_irq_en(void);
void gpioc_irq_dis(void);
void gpio_irq_en(void);
void gpiob_cb(void);
void gpioc_cb(void);

#endif

