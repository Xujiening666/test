#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define BT_NAME_MAX         32

void sys_init(uint32_t chipid, uint32_t offset);
void bt_name_get(uint8_t* bt_name, uint8_t* len);
void bt_addr_get(uint8_t* bt_addr);

void write_irq_init(void);
void exception_init(void);
void hexdump(char* name, char* base, int len);
uint32_t sys_get_ver(void);
void set_flash_rfpara(uint32_t *flash_btss_para,uint8_t len);
void get_flash_rfpara(uint32_t *flash_btss_para,uint8_t len);
uint32_t nor_param_get_tone_addr(void);
uint32_t get_idle_process_count(void);
void set_chip_id(uint8_t *sn_id,uint32_t sn_size);

void get_chip_id(uint8_t *sn_id,uint32_t sn_size);


#ifdef __cplusplus
}
#endif

#endif

