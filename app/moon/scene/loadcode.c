#include "sys_inc.h"
#include "user_config.h"
#include "task.h"
#include "system.h"
#include "loadcode.h"


void load_code_init(void)
{
    norflash_load_code(SRAM1_RAM_ADDR, SRAM1_FLASH_ADDR, SRAM1_SIZE);
    norflash_load_code(SRAM2_RAM_ADDR, SRAM2_FLASH_ADDR, SRAM2_SIZE);
}

