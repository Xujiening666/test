#ifndef _HAL_EID_LIB_H_
#define _HAL_EID_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif


enum eid_key_num_e {
    EID_KEY0 = 0,
    EID_KEY1,
    EID_KEY2,
    EID_KEY3,
};

enum eid_id_e {
    EID_CHIP_ID = 0,
    EID_ENCRYPT_KEY,
    EID_DCXO_TRIM,
    EID_RFTX_PAPOWER_CTUNE,
    EID_RFRX_LNA_CW,
    EID_RF_BIAS_RES_TRIM,
    EID_RFRX_LDO_TRIM,
    EID_RFTX_MAX_POWER,
    EID_RFTX_POWER_OFFSET,
    EID_RFRX_POWER_OFFSET,

    EID_FT_FLAG,
    EID_CP_FLAG,
};


uint32_t eid_key_get(uint32_t key_num);
void eid_read_protect(uint32_t key_num);
uint32_t eid_get_encryptkey(void);
uint32_t eid_get_pmu_vref_fine_tune(void);
uint32_t eid_get(uint32_t eid_id);
//uint32_t edi_get_sn();
uint32_t sys_get_sn();



#ifdef __cplusplus
}
#endif

#endif

