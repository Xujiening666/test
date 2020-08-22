#ifndef _HAL_UART_LIB_H_
#define _HAL_UART_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define UART_NUM                3
#define UART_RX_CALLBACK_MAXNUM 4


enum uart_id_e {
    UART_ID_00 = 0,
    UART_ID_01,
    UART_ID_02,
};

/* UART Hardware Flow Control */
enum uart_hw_flow_ctl_e {
    UART_HWCONTROL_NONE       = (0x0 << 0),
    UART_HWCONTROL_AUTO       = (0x3 << 0),
    UART_HWCONTROL_ONLY_RTS   = (0x1 << 0),
    UART_HWCONTROL_ONLY_CTS   = (0x2 << 0),
};

typedef struct {
    uint32_t baudrate;
    uint32_t clksrc;
    uint32_t flowctl;
    uint32_t fifo;
    uint32_t dma;
} uart_param_t;

typedef void (*rx_callback)(uint32_t rx_data);
typedef struct{
    rx_callback rx_callback;
    uint8_t registered;
} uart_rx_callback;

typedef void (*rx_dma_callback)(uint8_t *rx_data, uint8_t len);
typedef struct{
    rx_dma_callback rx_dma_callback;
    uint8_t registered;
} uart_rx_dma_callback;

void uart0_module_init(void);
void uart1_module_init(void);
void uart2_module_init(void);
void uart_register_rx_callback(uint8_t uart_id, void (*rx_callback)(uint32_t rx_data));
void uart_dbg_init(uint32_t uart_id);
void uart_tx(uint32_t uart_id, uint32_t data);

int uart1_reinit(void);
void uart1_dma_rx_callback(void *dma_ch);
void uart1_dma_tx_callback(void *dma_ch);
void uart2_dma_rx_callback(void *dma_ch);
void uart2_dma_tx_callback(void *dma_ch);
void uart_module_dma_rx_init(uint8_t uart_id,uint32_t buffer_len);
void uart_module_dma_tx_init(uint8_t uart_id,uint32_t buffer_len);
void uart_dma_tx_send_data(uint8_t uart_id,uint8_t *buffer,uint8_t len);
void uart_dma_rx_get_data(uint8_t uart_id,uint8_t buffer_len);
void uart_register_dma_rx_callback(uint8_t uart_id, void (*rx_callback)(uint8_t *rx_data, uint8_t len));


#if DEBUG_DUMP_TASK_EN
void dump_ch_data(uint32_t ch, uint8_t *buf, uint32_t bytes);
void task_dump(void *pvParameters);
void task_dump_init(void);
#endif


#ifdef __cplusplus
}
#endif

#endif


