#ifndef _UART_H_
#define _UART_H_

//UART0
#if (UART0_TX_PIN_SEL == 0)
#define UART0_TX_PIN_CFG    (PIN_B1 | PIN_FUNC4)
#elif (UART0_TX_PIN_SEL == 1)
#define UART0_TX_PIN_CFG    (PIN_B3 | PIN_FUNC4)
#elif (UART0_TX_PIN_SEL == 2)
#define UART0_TX_PIN_CFG    (PIN_B6 | PIN_FUNC7)
#elif (UART0_TX_PIN_SEL == 3)
#define UART0_TX_PIN_CFG    (PIN_B8 | PIN_FUNC3)
#elif (UART0_TX_PIN_SEL == 4)
#define UART0_TX_PIN_CFG    (PIN_B9 | PIN_FUNC5)
#elif (UART0_TX_PIN_SEL == 5)
#define UART0_TX_PIN_CFG    (PIN_D0 | PIN_FUNC9)
#elif (UART0_TX_PIN_SEL == 6)
#define UART0_TX_PIN_CFG    (PIN_A0 | PIN_FUNC2)
#elif (UART0_TX_PIN_SEL == 7)
#define UART0_TX_PIN_CFG    (PIN_B4 | PIN_FUNC4)
#elif (UART0_TX_PIN_SEL == 8)
#define UART0_TX_PIN_CFG    (PIN_B11 | PIN_FUNC5)
#elif (UART0_TX_PIN_SEL == 9)
#define UART0_TX_PIN_CFG    (PIN_B13 | PIN_FUNC5)
#endif

#if (UART0_RX_PIN_SEL == 0)
#define UART0_RX_PIN_CFG    (PIN_B4 | PIN_FUNC4)
#elif (UART0_RX_PIN_SEL == 1)
#define UART0_RX_PIN_CFG    (PIN_B10 | PIN_FUNC5)
#elif (UART0_RX_PIN_SEL == 2)
#define UART0_RX_PIN_CFG    (PIN_D1 | PIN_FUNC9)
#elif (UART0_RX_PIN_SEL == 3)
#define UART0_RX_PIN_CFG    (PIN_A1 | PIN_FUNC2)
#elif (UART0_RX_PIN_SEL == 4)
#define UART0_RX_PIN_CFG    (PIN_B12 | PIN_FUNC5)
#endif


//UART1
#if (UART1_CTS_PIN_SEL == 0)
#define UART1_CTS_PIN_CFG   (PIN_B4 | PIN_FUNC8)
#elif (UART1_CTS_PIN_SEL == 1)
#define UART1_CTS_PIN_CFG   (PIN_B11 | PIN_FUNC8)
#elif (UART1_CTS_PIN_SEL == 2)
#define UART1_CTS_PIN_CFG   (PIN_B3 | PIN_FUNC2)
#elif (UART1_CTS_PIN_SEL == 3)
#define UART1_CTS_PIN_CFG   (PIN_A31 | PIN_FUNC2)
#elif (UART1_CTS_PIN_SEL == 4)
#define UART1_CTS_PIN_CFG   (PIN_A25 | PIN_FUNC2)
#elif (UART1_CTS_PIN_SEL == 5)
#define UART1_CTS_PIN_CFG   (PIN_B13 | PIN_FUNC8)
#endif

#if (UART1_RTS_PIN_SEL == 0)
#define UART1_RTS_PIN_CFG   (PIN_B3 | PIN_FUNC8)
#elif (UART1_RTS_PIN_SEL == 1)
#define UART1_RTS_PIN_CFG   (PIN_B10 | PIN_FUNC8)
#elif (UART1_RTS_PIN_SEL == 2)
#define UART1_RTS_PIN_CFG   (PIN_B2 | PIN_FUNC2)
#elif (UART1_RTS_PIN_SEL == 3)
#define UART1_RTS_PIN_CFG   (PIN_A30 | PIN_FUNC2)
#elif (UART1_RTS_PIN_SEL == 4)
#define UART1_RTS_PIN_CFG   (PIN_A24 | PIN_FUNC2)
#elif (UART1_RTS_PIN_SEL == 5)
#define UART1_RTS_PIN_CFG   (PIN_B12 | PIN_FUNC8)
#endif

#if (UART1_RX_PIN_SEL == 0)
#define UART1_RX_PIN_CFG    (PIN_B13 | PIN_FUNC8)
#elif (UART1_RX_PIN_SEL == 1)
#define UART1_RX_PIN_CFG    (PIN_D1 | PIN_FUNC8)
#elif (UART1_RX_PIN_SEL == 2)
#define UART1_RX_PIN_CFG    (PIN_B1 | PIN_FUNC2)
#elif (UART1_RX_PIN_SEL == 3)
#define UART1_RX_PIN_CFG    (PIN_A29 | PIN_FUNC2)
#elif (UART1_RX_PIN_SEL == 4)
#define UART1_RX_PIN_CFG    (PIN_A23 | PIN_FUNC2)
#elif (UART1_RX_PIN_SEL == 5)
#define UART1_RX_PIN_CFG    (PIN_B11 | PIN_FUNC8)
#endif

#if (UART1_TX_PIN_SEL == 0)
#define UART1_TX_PIN_CFG    (PIN_B12 | PIN_FUNC8)
#elif (UART1_TX_PIN_SEL == 1)
#define UART1_TX_PIN_CFG    (PIN_D0 | PIN_FUNC8)
#elif (UART1_TX_PIN_SEL == 2)
#define UART1_TX_PIN_CFG    (PIN_B0 | PIN_FUNC2)
#elif (UART1_TX_PIN_SEL == 3)
#define UART1_TX_PIN_CFG    (PIN_A28 | PIN_FUNC2)
#elif (UART1_TX_PIN_SEL == 4)
#define UART1_TX_PIN_CFG    (PIN_A22 | PIN_FUNC2)
#elif (UART1_TX_PIN_SEL == 5)
#define UART1_TX_PIN_CFG    (PIN_B10 | PIN_FUNC8)
#endif


//UART2
#if (UART2_CTS_PIN_SEL == 0)
#define UART2_CTS_PIN_CFG   (PIN_B13 | PIN_FUNC9)
#endif

#if (UART2_RTS_PIN_SEL == 0)
#define UART2_RTS_PIN_CFG   (PIN_B12 | PIN_FUNC9)
#endif

#if (UART2_RX_PIN_SEL == 0)
#define UART2_RX_PIN_CFG    (PIN_B2 | PIN_FUNC9)
#elif (UART2_RX_PIN_SEL == 1)
#define UART2_RX_PIN_CFG    (PIN_B4 | PIN_FUNC10)
#endif

#if (UART2_TX_PIN_SEL == 0)
#define UART2_TX_PIN_CFG    (PIN_B0 | PIN_FUNC9)
#elif (UART2_TX_PIN_SEL == 1)
#define UART2_TX_PIN_CFG    (PIN_B3 | PIN_FUNC10)
#endif

#define DMA_DATA_LEN    256

void uart_init(void);
void uart_debug_init(void);
void uart_send_event(uint32_t event);
void uart_rx_key(uint32_t data);
void uart_uart1_dma_callback(uint8_t *rx_data, uint8_t len);
void uart_uart2_dma_callback(uint8_t *rx_data, uint8_t len);
void uart_dma_mode_init(void);

#endif

