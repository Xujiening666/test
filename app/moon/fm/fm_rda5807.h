#ifndef _FM_RDA5807_H_
#define _FM_RDA5807_H_

#include "sys_types.h"


#define RDA5807_PE_SP       1
#define RDA5807_HS_HP       1
#define RDA5807_M_MP        1


uint8_t rda5807_online(void);
void rda5807_init(void);
void rda5807_set_vol(uint8_t vol);
uint8_t rda5807_seek(uint16_t freq);
void rda5807_off(void);
void rda5807_mute(void);
void rda5807_unmute(void);
void rda5807_seek_tune(uint16_t freq);
void rda5807_fill(const uint8_t *pSrc);
void rda5807_write(const uint8_t *data, uint8_t size);
void rda5807_read(uint8_t *data, uint8_t size);


#endif


