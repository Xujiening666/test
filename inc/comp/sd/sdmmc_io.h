#ifndef _SDMMC_IO_H_
#define _SDMMC_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"
#include "diskio.h"

void sd_stop(void);
DRESULT sd_get_status(void);
DRESULT sd_failcb(void);
void sd_detect_set(uint8_t sta);
uint32_t sd_clkdiv_set(void);
DRESULT sd_disk_read_retry(void);

#ifdef __cplusplus
}
#endif

#endif
