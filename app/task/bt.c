#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "bt.h"
#include "bt_api.h"
#include "pmu.h"
#include "norflash.h"
#include "ble_app.h"


volatile bool bt_init_complete;
uint8_t bt_local_addr[6];
uint8_t ble_local_addr[6];
const uint8_t bt_addr_default[] = BT_ADDR_DEFAULT;

void bt_rf_power_force(uint8_t enable, uint8_t power)
{
    rf_power_force(enable, power);
}

void bt_rf_maxpower_set(uint8_t enable, uint8_t power)
{
    if(enable==1){
        bt_rf_power_max_set(power);
    }
}

void bt_param_get(void)
{
    uint8_t *tag_buf;

    tag_buf = libc_malloc(SIZE_TAG);
    norflash_read((uint32_t)tag_buf, 0x1000, SIZE_TAG);
    logx("param:");
    printf_array(tag_buf, SIZE_TAG);

    memcpy(bt_local_addr, tag_buf+OFFSET_BT_ADDR, SIZE_BT_ADDR);
    memcpy(bt_local_addr,bt_addr_default,6);
    memcpy(ble_local_addr, tag_buf+OFFSET_BLE_ADDR, SIZE_BLE_ADDR);
    libc_free(tag_buf);
}


void task_main_ble(void)
{
    logi("enter task ble_ota.");

    rf_para_dft();
    bt_ll_init();

#if PMU_EXIST
    pmu_isr_bh_init();
    pmu_task_init();
#endif
    bt_frontend_init();
    bt_rf_power_force(BT_RFPOWER_FORCE_EN, BT_RFPOWER_FORCE_VAL);
    bt_rf_maxpower_set(BT_MAXPOWER_SET_EN, BT_MAXPOWER_SET_VAL);
    bt_param_get();

    xTaskCreate(task_btc, "task btc",
        BT_CTRL_TASK_STACK_SIZE, NULL, BT_CTRL_TASK_PRIO, &btc_handler);

    ble_init();
    ble_app_init();
	//while(1);
   
}


