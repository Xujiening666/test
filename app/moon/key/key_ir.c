#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "key.h"


#if KEY_IR_EN

kir_t kir;


AT(.irrx_rodata_seg)
const uint8_t kir_tbl[IR_KEY_NUM][2] = {
    {IR_KEY00, KEY_CH_DOWN},    {IR_KEY01, KEY_MODE},       {IR_KEY02, KEY_CH_UP},
    {IR_KEY10, KEY_PREV},       {IR_KEY11, KEY_NEXT},       {IR_KEY12, KEY_PLAY},
    {IR_KEY20, KEY_VOL_DOWN},   {IR_KEY21, KEY_VOL_UP},     {IR_KEY22, KEY_EQ},
    {IR_KEY30, KEY_NUM0},       {IR_KEY31, KEY_N100_PLUS},  {IR_KEY32, KEY_REPEAT},
    {IR_KEY40, KEY_NUM1},       {IR_KEY41, KEY_NUM2},       {IR_KEY42, KEY_NUM3},
    {IR_KEY50, KEY_NUM4},       {IR_KEY51, KEY_NUM5},       {IR_KEY52, KEY_NUM6},
    {IR_KEY60, KEY_NUM7},       {IR_KEY61, KEY_NUM8},       {IR_KEY62, KEY_NUM9},
};

AT(.irrx_init_seg)
void kir_init(void)
{
    kir.user_code = KEY_IR_USER_NONE;
    kir.data_code = KEY_NONE;
    kir.cnt = 0xFF;
    irrx_init();
}

AT(.irrx_seg)
uint8_t kir_get_key(void)
{
    uint32_t n;
    if(kir.cnt < 23) {   //23*5ms > 110ms, time out
        kir.cnt++;
    } else {
        kir.user_code = KEY_IR_USER_NONE;
        kir.data_code = KEY_NONE;
        kir.cnt = 0xFF;
    }

#if IR_DATA_SHIFT
    if (kir.user_code != 0xFF00) {
#else
    if (kir.user_code != 0x00FF) {
#endif
        return KEY_NONE;
    }

    n = IR_KEY_NUM;
    while(n--) {
        if(kir.data_code == kir_tbl[n][0]) {
            return kir_tbl[n][1];
        }
    }

    return KEY_NONE;
}

AT(.irrx_seg)
void kir_get_data(void)
{
    kir.cnt = 0;

    kir.user_code = irrx_get_usercode();
    kir.data_code = irrx_get_datacode();
#if IR_DATA_SHIFT
    kir.user_code = kir_shift_16bit(kir.user_code);
    kir.data_code = kir_shift_8bit(kir.data_code);
#endif
    logi("ir[%04x,%02x]", kir.user_code,kir.data_code);
}

#if IR_DATA_SHIFT
AT(.irrx_seg)
uint8_t kir_shift_8bit(uint8_t data)
{
    data = (data<<4) | (data>>4);
    data = ((data<<2)&0xCC) | ((data>>2)&0x33);
    data = ((data<<1)&0xAA) | ((data>>1)&0x55);
    return data;
}

AT(.irrx_seg)
uint16_t kir_shift_16bit(uint16_t data)
{
    data = (data<<8) | (data>>8);
    data = ((data<<4)&0xF0FF) | ((data>>4)&0xFF0F);
    data = ((data<<2)&0xCCCC) | ((data>>2)&0x3333);
    data = ((data<<1)&0xAAAA) | ((data>>1)&0x5555);
    return data;
}
#endif

#endif

