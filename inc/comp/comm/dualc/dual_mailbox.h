/*
* Copyright (c) 2016 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File : dual_mailbox.h
@ History :
    Author              Date                Comment
    BRAD_YAN        2018/1/22       Create dual cpu mailbox header file

@ Description :
*/

#ifndef _DUAL_MAILBOX_H_
#define _DUAL_MAILBOX_H_

#include "sys_inc.h"
#include "dual_common.h"
#include "FreeRTOS.h"
#include "semphr.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define MBX_C02C1_MSG_LEN          ((uint32_t)0x20)
#define MBX_C02C1_MSG_ADDR         ((uint32_t)0x0082FFE0)

#define MBX_C12C0_MSG_LEN          ((uint32_t)0x20)
#define MBX_C12C0_MSG_ADDR         ((uint32_t)0x0082FFC0)


#define MBX_C0_MSG_INT_EN          ((uint32_t)0x40000070)
#define MBX_C0_MSG_INT_STA         ((uint32_t)0x40000074)
#define MBX_C1_MSG_INT_EN          ((uint32_t)0x40000078)
#define MBX_C1_MSG_INT_STA         ((uint32_t)0x4000007C)

#define MBX_INT_EN                 ((uint32_t)0x1)
#define MBX_INT_DIS                ((uint32_t)0x0)

#define MBX_INT_CLR_VAL            ((uint32_t)0x1)
#define MBX_INT_TRIG_VAL           ((uint32_t)0x1)

typedef uint32_t cpu_msg_t[8];
extern volatile cpu_msg_t cpu0msg;
extern volatile cpu_msg_t cpu1msg;

void dual_mailbox_c0init();
void dual_mailbox_c1init();

typedef struct cpu0context_s {
    SemaphoreHandle_t sem_sync;
} cpu0context_t;

typedef struct cpu1context_s {
    void *handle;
    volatile bool msg_received;
} cpu1context_t;

extern volatile cpu0context_t c0ct;
extern volatile cpu1context_t c1ct;

enum cpu0msg_e {
    CPU0MSG_INIT_REQ,
    CPU0MSG_DEINIT_REQ,
    CPU0MSG_INPUT_ACK,
    CPU0MSG_OUTPUT_ACK,

    CPU0MSG_FRAME_START,
    CPU0MSG_FRAME_END_ACK,

    CPU0MSG_EN_ANC_IRQ,
    CPU0MSG_DIS_ANC_IRQ,
};

enum cpu1msg_e {
    CPU1MSG_INIT_ACK,
    CPU1MSG_DEINIT_ACK,
    CPU1MSG_INPUT_REQ,
    CPU1MSG_OUTPUT_REQ,
    CPU1MSG_FRAME_END,
};

uint32_t cpu1msg_receive();
void cpu0msg_send(uint32_t msg);
uint32_t cpu0msg_receive();
void cpu1msg_send(uint32_t msg);
void dual_cpu_init(cpu1_msg_process_hook_t func);
void dual_cpu_deinit();


#ifdef __cplusplus
}
#endif

#endif /* _DUAL_MAILBOX_H_ */
