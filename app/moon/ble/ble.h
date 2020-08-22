#ifndef _BLE_H_
#define _BLE_H_


extern uint8_t bt_local_addr[6];
extern uint8_t ble_local_addr[6];


uint8_t bt_is_left_ear(void);
uint32_t sys_setting_ble_random(void);

void ble_addr_set(uint8_t *ble_addr);
void ble_name_set(uint8_t *ble_name);
uint8_t ble_name_len_get(void);


#endif
