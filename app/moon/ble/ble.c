#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "ble.h"


const uint8_t ble_addr_default[] = BLE_ADDR_DEFAULT;

const uint8_t ble_name_default[] = BLE_NAME_DEFAULT;


extern uint32_t ble_random_num;


void ble_addr_set(uint8_t *ble_addr)
{
    memcpy(ble_addr, ble_addr_default, 6);
    
#if BLE_ADDR_USE_RANDOM
    libc_memcpy(ble_addr, &ble_random_num, 4);
#endif
    memcpy(ble_local_addr, ble_addr, 6);
    logi("ble addr:%02x:%02x:%02x:%02x:%02x:%02x",
        ble_addr[5],ble_addr[4],ble_addr[3],ble_addr[2],ble_addr[1],ble_addr[0]);
}

uint8_t ble_name_len_get(void)
{
      //return sizeof(ble_name_default); 
      return strlen((char *)ble_name_default);
}

void ble_name_set(uint8_t *ble_name)
{
      libc_memcpy(ble_name, ble_name_default, sizeof(ble_name_default));
      logi("**ble name: %s", ble_name);
}


