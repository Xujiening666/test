
/************************************************************
warning:  do not modify this file
warning:  do not modify this file
warning:  do not modify this file
warning:  do not modify this file
warning:  do not modify this file

Author: Smartlink
*************************************************************/


#include "sys_inc.h"
#include "user_config.h"

#define ENABLE_FLAG 1
#define VERSION 2

AT(.chip_programmer_data)
const uint8_t chip_startup_config[256] = {
    'P','M','U','C','F',    //header
    0x00,   //config length low
    0x01,   //config length low
    VERSION, 
#if CHIP_NO_VBUS    
    1,
#else
    0,
#endif
    0,0,0,0,0,0,0,    //reserved

    #if PMU_POWER_ON_MODE       //data 0  and 1
        ENABLE_FLAG,
        1,
        ENABLE_FLAG ,
        1,
    #else
        0,
        0,
        0,
        0,
    #endif

    1,//ENABLE_FLAG & PMU_VBAT_POWER_ON_CONTROL,
    1,//PMU_VBAT_POWER_ON_CONTROL,
    
    1,//ENABLE_FLAG & PMU_LDO_VCCIO_SHUTDOWN_OPEN_RES_CONTROL,
    1,//PMU_LDO_VCCIO_SHUTDOWN_OPEN_RES_CONTROL,
    
    0,//ENABLE_FLAG & PMU_POWER_OFF_CLOSE_RTC_VCC,
    0//PMU_POWER_OFF_CLOSE_RTC_VCC
};

AT(.programmer_data)
void Reset_Handler()
{}

AT(.programmer_data)
int main()
{
return 1;
}

