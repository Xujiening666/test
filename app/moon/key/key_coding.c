#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "key.h"


kcd_t kcd;


AT(.key_init_seg)
void kcd_init(void)
{
    kcd.cur_direction = DIRECTION_INIT;
    kcd.last_direction = DIRECTION_INIT;
    kcd.status = STATUS_NONE;
    kcd.step_delay = 0;
    kcd.switch_delay = 0;

    pin_config(KEY_CODING_A_PIN_CFG);
    pin_config(KEY_CODING_B_PIN_CFG);

    pin_irq_clk_config(KEY_CODING_A_PIN_NUM, (0x10<<8)|(0x00<<7)|(0x01<<4));
    pin_irq_config(KEY_CODING_A_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    gpiob_irq_en();
}

AT(.key_seg)
void kcd_get_sta(void)
{
    if (pin_get_irq_pending(KEY_CODING_A_PIN_NUM)) {
        pin_clear_irq_pending(KEY_CODING_A_PIN_NUM);

        if (kcd.status != STATUS_NONE || kcd.step_delay) {
            return;
        }

        if (pin_get_value(KEY_CODING_A_PIN_NUM)) {
            if (pin_get_value(KEY_CODING_B_PIN_NUM)) {
                kcd.cur_direction = DIRECTION_CLOCKWISE;
            } else {
                kcd.cur_direction = DIRECTION_ANTICLOCKWISE;
            }
        } else {
            if (pin_get_value(KEY_CODING_B_PIN_NUM)) {
                kcd.cur_direction = DIRECTION_ANTICLOCKWISE;
            } else {
                kcd.cur_direction = DIRECTION_CLOCKWISE;
            }
        }

        if (kcd.last_direction != kcd.cur_direction && kcd.switch_delay > 0) {
            kcd.status = STATUS_NONE;
        } else {
            kcd.last_direction = kcd.cur_direction;

            if (kcd.cur_direction == DIRECTION_CLOCKWISE) {
                kcd.status = STATUS_CLOCKWISE;
            } else if (kcd.cur_direction == DIRECTION_ANTICLOCKWISE) {
                kcd.status = STATUS_ANTICLOCKWISE;
            }
        }

        kcd.step_delay = 20;
        kcd.switch_delay = 50;
    }
}

AT(.key_seg)
void kcd_delay(void)
{
    if (kcd.step_delay) {
        kcd.step_delay--;
    }
    if (kcd.switch_delay) {
        kcd.switch_delay--;
    }
}

AT(.key_seg)
uint32_t kcd_get_key(void)
{
    uint32_t kval;
    kval = KEY_NONE;

    kcd_delay();

    if (kcd.status == STATUS_CLOCKWISE) {
        logi("+");
    } else if (kcd.status == STATUS_ANTICLOCKWISE) {
        logi("-");
    }
    kcd.status = STATUS_NONE;

    return kval;
}

