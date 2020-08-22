#ifndef _HAL_TWI_LIB_H_
#define _HAL_TWI_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_hal_def.h"

#define TWI1_ADDR_MODE          TWI_ADDR_MODE_7BIT
#define TWI2_ADDR_MODE          TWI_ADDR_MODE_7BIT

enum twi_id_e {
    TWI_ID_01 = 0,
    TWI_ID_02
};


void twi_module_init(uint32_t id, uint32_t fre);
void twi_module_deinit(uint32_t id);
int twi_module_write(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes);
int twi_module_read(uint32_t id, uint32_t addr, uint8_t *data, uint32_t bytes);


#ifdef __cplusplus
}
#endif

#endif
