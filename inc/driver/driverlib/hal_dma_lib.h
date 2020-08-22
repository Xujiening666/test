#ifndef _HAL_DMA_LIB_H_
#define _HAL_DMA_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_inc.h"

enum dma_transfer_type_e {
    DMA_SRAM_TO_SRAM,
    DMA_SRAM_TO_ASRCTX,
    DMA_SRAM_TO_I2STX,
    DMA_SRAM_TO_AUSSTX,
    DMA_SRAM_TO_UART1,
    DMA_SRAM_TO_UART2,
    DMA_ASRCRX_TO_SRAM,
    DMA_I2SRX_TO_SRAM,
    DMA_AUSSRX_TO_SRAM,
    DMA_SPDIF_TO_SRAM,
    DMA_UART1_TO_SRAM,
    DMA_UART2_TO_SRAM,
};

typedef struct {
    uint8_t transfer_type;          /* DMA传输类型 */
    uint8_t irq_priority;           /* DMA中断优先级 */
    uint8_t dst_block_bytes;        /* 目标设备传输block大小 */
    uint8_t src_block_bytes;        /* 源设备传输block大小 */
    void (*callback)(void *dma_ch); /* 传输完成回调函数 */
} dma_channel_config_t;

/* 初始化DMA模块 */
void dma_init(void);

/* 申请分配DMA channel */
void *dma_channel_alloc(void);
/* 释放DMA channel */
void dma_channel_free(void *dma_ch);
/* 配置DMA参数 */
void dma_channel_config(void *dma_ch, dma_channel_config_t *config);

/* 触发DMA传输 */
void dma_channel_start(void *dma_ch, void *dst, void *src, uint32_t bytes);

/* 终止DMA传输 */
void dma_channel_abort(void *dma_ch);

/* 获取剩余传输的数据量大小 */
uint32_t dma_channel_get_remaining(void *dma_ch);


#ifdef __cplusplus
}
#endif


#endif

