#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "key.h"


#if KEY_IO_EN

uint8_t volatile kio_sta;   //0-key press;  1-key release

AT(.key_init_seg)
void kio_gpio_init(void)
{
#ifdef PIN_KIO0
    pin_config(PIN_KIO0|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO0, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO0, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO1
    pin_config(PIN_KIO1|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO1, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO1, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO2
    pin_config(PIN_KIO2|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO2, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO2, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO3
    pin_config(PIN_KIO3|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO3, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO3, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO4
    pin_config(PIN_KIO4|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO4, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO4, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO5
    pin_config(PIN_KIO5|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO5, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO5, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO6
    pin_config(PIN_KIO6|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO6, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO6, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif

#ifdef PIN_KIO7
    pin_config(PIN_KIO7|PIN_FUNC14|PIN_PULL_UP100K);
    pin_irq_clk_config(PIN_KIO7, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(PIN_KIO7, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
#endif
}

AT(.key_init_seg)
void kio_irq_enable(void)
{
#if KIO_GPIOB
    gpiob_irq_en();
#endif

#if KIO_GPIOC
    gpioc_irq_en();
#endif
}

AT(.key_init_seg)
void kio_irq_disable(void)
{
#if KIO_GPIOB
    gpiob_irq_dis();
#endif

#if KIO_GPIOC
    gpioc_irq_dis();
#endif
}

AT(.key_init_seg)
void kio_init(void)
{
    kio_sta = 0xFF;

    kio_irq_disable();
    kio_gpio_init();
    kio_irq_enable();
}

AT(.key_seg)
void kio_get_sta(void)
{
#ifdef PIN_KIO0
    if (pin_get_irq_pending(PIN_KIO0)) {
        pin_clear_irq_pending(PIN_KIO0);
        if(((kio_sta & KIO0) && (pin_get_value(PIN_KIO0)==0)) ||
            (((kio_sta & KIO0) == 0) && (pin_get_value(PIN_KIO0)==1))){
            kio_sta ^= KIO0;
        }
    }
#endif

#ifdef PIN_KIO1
    if (pin_get_irq_pending(PIN_KIO1)) {
        pin_clear_irq_pending(PIN_KIO1);
        if(((kio_sta & KIO1) && (pin_get_value(PIN_KIO1)==0)) ||
            (((kio_sta & KIO1) == 0) && (pin_get_value(PIN_KIO1)==1))){
            kio_sta ^= KIO1;
        }
    }
#endif

#ifdef PIN_KIO2
    if (pin_get_irq_pending(PIN_KIO2)) {
        pin_clear_irq_pending(PIN_KIO2);
        if(((kio_sta & KIO2) && (pin_get_value(PIN_KIO2)==0)) ||
            (((kio_sta & KIO2) == 0) && (pin_get_value(PIN_KIO2)==1))){
            kio_sta ^= KIO2;
        }
    }
#endif

#ifdef PIN_KIO3
    if (pin_get_irq_pending(PIN_KIO3)) {
        pin_clear_irq_pending(PIN_KIO3);
        if(((kio_sta & KIO3) && (pin_get_value(PIN_KIO3)==0)) ||
            (((kio_sta & KIO3) == 0) && (pin_get_value(PIN_KIO3)==1))){
            kio_sta ^= KIO3;
        }
    }
#endif

#ifdef PIN_KIO4
    if (pin_get_irq_pending(PIN_KIO4)) {
        pin_clear_irq_pending(PIN_KIO4);
        if(((kio_sta & KIO4) && (pin_get_value(PIN_KIO4)==0)) ||
            (((kio_sta & KIO4) == 0) && (pin_get_value(PIN_KIO4)==1))){
            kio_sta ^= KIO4;
        }
    }
#endif

#ifdef PIN_KIO5
    if (pin_get_irq_pending(PIN_KIO5)) {
        pin_clear_irq_pending(PIN_KIO5);
        if(((kio_sta & KIO5) && (pin_get_value(PIN_KIO5)==0)) ||
            (((kio_sta & KIO5) == 0) && (pin_get_value(PIN_KIO5)==1))){
            kio_sta ^= KIO5;
        }
    }
#endif

#ifdef PIN_KIO6
    if (pin_get_irq_pending(PIN_KIO6)) {
        pin_clear_irq_pending(PIN_KIO6);
        if(((kio_sta & KIO6) && (pin_get_value(PIN_KIO6)==0)) ||
            (((kio_sta & KIO6) == 0) && (pin_get_value(PIN_KIO6)==1))){
            kio_sta ^= KIO6;
        }
    }
#endif

#ifdef PIN_KIO7
    if (pin_get_irq_pending(PIN_KIO7)) {
        pin_clear_irq_pending(PIN_KIO7);
        if(((kio_sta & KIO7) && (pin_get_value(PIN_KIO7)==0)) ||
            (((kio_sta & KIO7) == 0) && (pin_get_value(PIN_KIO7)==1))){
            kio_sta ^= KIO7;
        }
    }
#endif
}

AT(.key_seg)
uint32_t kio_get_key(void)
{
    uint32_t kval;
    kval = KEY_NONE;

#if ONE_KEY_MULTI_FUNC
    if((kio_sta & KIO0) == 0) {
        kval = KEY_NEXT_VOLUP;
    } else if((kio_sta & KIO1) == 0) {
        kval = KEY_PREV_VOLDOWN;
    }
#else
    if((kio_sta & KIO0) == 0) {
        kval = KEY_PREV;
    } else if((kio_sta & KIO1) == 0) {
        kval = KEY_PLAY;
    } else if((kio_sta & KIO2) == 0) {
        kval = KEY_NEXT;
    } else if((kio_sta & KIO3) == 0) {
        kval = KEY_VOL_DOWN;
    } else if((kio_sta & KIO4) == 0) {
        kval = KEY_VOL_UP;
    } else if((kio_sta & KIO5) == 0) {
        kval = KEY_MODE;
    } else if((kio_sta & KIO6) == 0) {
        kval = KEY_REPEAT;
    } else if((kio_sta & KIO7) == 0) {
        kval = KEY_EQ;
    }
#endif

    return kval;
}
#endif
