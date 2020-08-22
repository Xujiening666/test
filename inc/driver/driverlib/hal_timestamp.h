/*
* Copyright (c) 2018 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File : hal_timestamp.h
@ History :
    Author          Date                Comment
    wuxiaopeng      2018/08/02          Create timestamp driver header file

@ Description :

*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_TIMESTAMP_H
#define __HAL_TIMESTAMP_H

void timestamp_init(void);
uint64_t us_to_timestamp(uint32_t us);
uint32_t timestamp_to_us(uint64_t ts);
uint64_t ms_to_timestamp(uint32_t ms);
uint32_t timestamp_to_ms(uint64_t ts);
uint64_t get_timestamp(void);
void set_timestamp(uint64_t val);

void delay_nop(uint32_t cnt);

/*
 * loop delay, not suspend task, can use in isr
 * delay in cycle
 */
void cdelay(uint32_t cycle);

/*
 * loop delay, not suspend task, can use in isr
 * delay in us
 */
void udelay(uint32_t us);

/*
 * loop delay, not suspend task, can use in isr
 * delay in ms
 */
void mdelay(uint32_t ms);

/*
 * schedule delay, suspend task, can not use in isr
 * delay in ms
 */
void msleep(uint32_t ms);
void timestamp_test(void);

#endif /* __HAL_TIMESTAMP_H */

