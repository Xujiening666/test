#ifndef _KEY_CODING_H_
#define _KEY_CODING_H_


enum kcd_direction_e {
    DIRECTION_INIT          = 0,
    DIRECTION_CLOCKWISE,
    DIRECTION_ANTICLOCKWISE,
};

enum kcd_status_e {
    STATUS_NONE             = 0,
    STATUS_CLOCKWISE,
    STATUS_ANTICLOCKWISE,
};


typedef struct {
    uint8_t volatile cur_direction;
    uint8_t volatile last_direction;
    uint8_t volatile status;
    uint8_t volatile step_delay;
    uint8_t volatile switch_delay;
} kcd_t;
extern kcd_t kcd;


#define KEY_CODING_A_PIN_CFG    (PIN_B12 | PIN_FUNC14 | PIN_PULL_UP100K)
#define KEY_CODING_A_PIN_NUM    (PIN_B12)

#define KEY_CODING_B_PIN_CFG    (PIN_B13 | PIN_FUNCINPUT | PIN_PULL_UP100K)
#define KEY_CODING_B_PIN_NUM    (PIN_B13)


void kcd_init(void);
void kcd_get_sta(void);
void kcd_delay(void);
uint32_t kcd_get_key(void);

#endif

