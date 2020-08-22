#ifndef _SNDLIB_H_
#define _SNDLIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_hal_def.h"


#define SND_I2S         0
#define SND_CODEC       1

typedef struct {
    uint32_t tx_min_fs;         //TX 最小支持的采样率
    uint16_t mclk_factor;       // mclk分频比（e.g. mclk设置成256倍采样率，则配置成256）
    uint8_t  format;            //0:标准I2S; 1:左对齐模式（left-justified）; 2:右对齐模式（right-justified）
    uint8_t  slot_width;        //0:16; 1:32
    uint8_t  sample_resolution; //0:16bit; 1:20bit; 2:24bit; 3:32bit
    uint8_t  slot_num;          //0:1-slot, 1:2-slot; 2:4-slot; 3:8-slot; 4:16-slot
} i2s_init_params_t;


#ifdef __cplusplus
}
#endif

#endif
