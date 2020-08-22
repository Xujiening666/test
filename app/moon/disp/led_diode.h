#ifndef _LED_DIODE_H_
#define _LED_DIODE_H_

#define LED_RED_PIN_CFG         (LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0)
#define LED_RED_PIN_LOW_CFG     (LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0)
#define LED_RED_PIN_HIGH_CFG    (LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH  | PIN_DRI0)

#define LED_BLUE_PIN_CFG        (LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0)
#define LED_BLUE_PIN_LOW_CFG    (LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0)
#define LED_BLUE_PIN_HIGH_CFG   (LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0)

enum disp_led_id_e {
    LED_RED,
    LED_BLUE,
};


typedef struct {
    uint8_t red_sta;
    uint8_t red_sta_last;
    uint8_t red_cnt;

    uint8_t blue_sta;
    uint8_t blue_sta_last;
    uint8_t blue_cnt;
} led_diode_t;
extern led_diode_t led_diode;


void led_diode_flash(enum disp_led_id_e led, bool enable);
void disp_led_diode_timer(void);
void disp_led_diode_init(void);
void disp_led_flase_time_do(enum disp_led_id_e id, uint16_t interval,uint8_t timers);


#endif
