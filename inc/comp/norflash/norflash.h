#ifndef _NORFLASH_H_
#define _NORFLASH_H_


#define FLASH_CHIP0_SIZE    0x80000     //512KB

enum flash_chip_e {
    FLASH_CHIP0 = 0x0,
    FLASH_CHIP1,
};

void norflash_init(uint32_t offset);
void norflash_read_id(uint32_t ram_addr, uint32_t len);
void norflash_read(uint32_t ram_addr, uint32_t flash_addr, uint32_t bytes);
void norflash_write(uint32_t ram_addr, uint32_t flash_addr, uint32_t bytes, bool erase, uint32_t wait_busy);
void norflash_erase(uint32_t flash_addr, uint32_t wait_busy);
int32_t norflash_waitbusy(void);
uint16_t norflash_get_crc(uint16_t val, uint8_t *buf, uint32_t len);
void decrypt_code(void);
void norflash_chip1_mapping(uint32_t map);
void norflash_workmode_change(uint32_t work_mode);
void norflash_clk_change(uint32_t clk_div);
void norflash_chip_sel(uint32_t chip_id);
bool pmu_nor_sec_can_erase(void);

#endif

