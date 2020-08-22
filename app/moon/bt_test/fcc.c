#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "bt_api.h"
#include "info_mem.h"
#include "fcc.h"



#define FCC_RX_MAX      19

uint8_t fcc_rx_arr[20];
uint8_t cmd_cnt=0;

AT(.fcc_sram_seg)
void fcc_test_rx(uint32_t data_in)
{
    static uint8_t rx_state = FCC_UART_IDLE;
    static uint8_t rx_cnt = 0,checksum = 0,rx_cnt_total = 0;
    static uint32_t data_pre;
    uint8_t data;

    data = data_in & 0xff;
    fcc_rx_arr[rx_cnt] = data;
    //rx_state = FCC_UART_RX;
    rx_cnt_total++;

	//logi("data is :%x", data);

    // first
    if ((data_pre == 'S') && (data == 'L')) {
        //logi(" fr %x",data);
        cmd_cnt++;
        if(cmd_cnt==2) {
            //logi("   fr %x  %d",data,rx_cnt_total);
        }

        rx_state = FCC_UART_RX;
        rx_cnt = 0;
        checksum = 0;
    } else {
        checksum = checksum^data;
    }

    data_pre = data;
    if(rx_state == FCC_UART_RX) {
        switch (rx_cnt) {
        case 0:
            break;
        case 1:
            fcc_control.test_control = data;  // 0: stop; 1: start
            break;
        case 2:
            fcc_control.freq_type = data;     // 0: hop;  1: hop
            break;
        case 3:
            fcc_control.channel_idx = data;   // 0~79
            break;
        case 4:
            fcc_control.tx_power = data;      // 0~32
            break;
        case 5:
            fcc_control.rx_gain = data;       // 0~32
            break;
        case 6:
            fcc_control.rx_gain_auto = data;
            break;
        case 7:
            fcc_control.role_idx = data;      // 1:slave 0:master -> node_idx
            break;
        case 8:
            fcc_control.tx_type = data;       // packe type
            break;
        case 9:
            fcc_control.tx_pkt_len = data;    // 0~1024
            break;
        case 10:
            fcc_control.tx_pkt_len = fcc_control.tx_pkt_len + data*256;    // 0~1024
            break;
        case 11:
            fcc_control.rx_type = data;       // packe type
            break;
        case 12:
            fcc_control.rx_pkt_len = data;    // 0~1024
            break;
        case 13:
            fcc_control.rx_pkt_len = fcc_control.rx_pkt_len + data*256;    // 0~1024
            break;
        case 14:
            fcc_control.payload_type = data;  // 0: 0x0;  1:0xff  2:0x55  3:0xf0:  4:prbs9
            break;
        case 15:
            fcc_control.tx_edr = data;
            break;
        case 16:
            fcc_control.rx_edr = data;
            break;
        case 17:
            //logi("checksum:%x", checksum);
            break;
        case 18:   // checksum, xor of alldata
            break;
        default:
            break;
        }

        rx_cnt++;
    }

    if(rx_cnt == (FCC_RX_MAX-1)) {
        // if cmd success
        for(int kk=1;kk<17;kk++){
            logi(" %d %d  |  ",kk,fcc_rx_arr[kk]);
        }
        rx_cnt = 0;
        rx_state=FCC_UART_IDLE;

		logi("tx_cnt is 0x%x--rx_cnt is 0x%x",*((uint32_t *)(0x40010020)),*((uint32_t *)(0x40010024)));

        pattern_parse();
        fcc_test_cntl(fcc_control);
    }
}

void bt_fcc_test(void)
{
#if BT_FCC_TEST_EN
    info_mem_init(); //fcc debug
    rf_para_dft();
    rf_config(1);
    btdm_config();

    uart_register_rx_callback(UART_ID_00, fcc_test_rx);

    fcc_test_init();
    while(1){};
#endif
}


