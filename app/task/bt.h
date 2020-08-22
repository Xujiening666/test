#ifndef _BT_H_
#define _BT_H_


extern volatile bool bt_init_complete;
extern TaskHandle_t btc_handler;
extern uint32_t ble_random_num;

void task_btc(void *pvParameters);
bool bt_is_left_ear(void);
void task_main_ble(void);



#endif

