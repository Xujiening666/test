#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "sd.h"
#include "usb.h"


enum device_id_e {
    DEV_SDCARD = 0,
    DEV_UDISK,
    DEV_PC,
    DEV_NONE,
};

enum device_sta_e {
    DEV_OFFLINE         = 0,    //offline
    DEV_UNINITIALIZED,          //have not initialized
    DEV_FAIL,                   //have initalized, but fail
    DEV_FS_FAIL,                //have initialized, and ok, but fat system is fail
    DEV_OK,                     //have initialized, and ok, have valid files
};

typedef struct {
    uint8_t cur_dev;
    uint8_t mounted;
    uint8_t startup;

    enum device_sta_e sdcard_sta;
    enum device_sta_e udisk_sta;
    enum device_sta_e pc_sta;
    bool line_sta;
} device_t;
extern device_t dev;


void device_init(void);
void device_detect(xTimerHandle xTimer);
int device_initialize(uint32_t dev_num);
bool device_is_initialized(uint32_t dev_num);
bool device_is_online(uint32_t dev_num);
uint32_t device_get_sta(uint32_t dev_num);
void device_set_sta(uint32_t dev_num, uint32_t dev_sta);
uint32_t device_get_num(void);
uint32_t device_get_next_num(void);
void device_set_num(uint32_t dev_num);
int device_try(uint32_t dev_num);

#endif

