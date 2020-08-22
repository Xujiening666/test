#ifndef _RFC_H_
#define _RFC_H_

void rf_clk_enable(bool enable);
void fcc_freq_force(uint8_t force,uint8_t channel); //0->2401; channel0->2402
void fcc_rfgain_force(uint8_t rxgain_auto,uint8_t rx_gain,uint8_t tx_gain);
#endif
