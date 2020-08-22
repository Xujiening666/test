#ifndef _FCC_H_
#define _FCC_H_


#ifdef __cplusplus
extern "C" {
#endif


enum fcc_rx_e {
   FCC_UART_IDLE = 0,
   FCC_UART_RX,
};

void bt_fcc_test(void);
void fcc_test_init(void);
int pattern_parse(void);
void fcc_bt_addr_set(void);
void btss_close(void);

void fcc_test_cntl(fcc_control_t fcc_control_n);


#ifdef __cplusplus
}
#endif

#endif

