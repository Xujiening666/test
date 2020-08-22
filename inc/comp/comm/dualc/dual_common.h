/*
* Copyright (c) 2016 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File : dual_common.h
@ History :
    Author              Date                 Comment
    BRAD_YAN        2018/1/22       Create dual cpu control header file

@ Description :
*/

#ifndef _DUAL_COMMON_H_
#define _DUAL_COMMON_H_

#include "sys_inc.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define CPU1_PURPOSE_REG0                 ((uint32_t)0x00820000)
#define CPU1_PURPOSE_REG1                 ((uint32_t)0x00820004)
#define CPU1_PURPOSE_REG2                 ((uint32_t)0x00820008)
#define CPU1_PURPOSE_REG3                 ((uint32_t)0x0082000C)

typedef void (*cpu1_msg_process_hook_t)(uint32_t msg);

void startup_cpu1(void);
void config_cpu1_hook(cpu1_msg_process_hook_t func);

#ifdef __cplusplus
}
#endif

#endif /* _DUAL_COMMON_H_ */
