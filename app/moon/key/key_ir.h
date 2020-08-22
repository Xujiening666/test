#ifndef _KEY_IR_H_
#define _KEY_IR_H_


/*
* if IR_DATA_SHIFT is defined as 0, the format of usercode/datacode is "b0:b1:b2:...:b13:b14:b15",
* if IR_DATA_SHIFT is defined as 1, the format of usercode/datacode is "b15:b14:b13:...:b2:b1:b0".
*/
#define IR_DATA_SHIFT       1

#define IR_KEY_NUM          21

#define KEY_IR_USER_NONE    0x0000


#if IR_DATA_SHIFT
enum ir_ff00_e {
    IR_KEY00 = 0x45,
    IR_KEY01 = 0x46,
    IR_KEY02 = 0x47,

    IR_KEY10 = 0x44,
    IR_KEY11 = 0x40,
    IR_KEY12 = 0x43,

    IR_KEY20 = 0x07,
    IR_KEY21 = 0x15,
    IR_KEY22 = 0x09,

    IR_KEY30 = 0x16,
    IR_KEY31 = 0x19,
    IR_KEY32 = 0x0D,

    IR_KEY40 = 0x0C,
    IR_KEY41 = 0x18,
    IR_KEY42 = 0x5E,

    IR_KEY50 = 0x08,
    IR_KEY51 = 0x1C,
    IR_KEY52 = 0x5A,

    IR_KEY60 = 0x42,
    IR_KEY61 = 0x52,
    IR_KEY62 = 0x4A,
};

#else
enum ir_00ff_e {
    IR_KEY00 = 0xA2,
    IR_KEY01 = 0x62,
    IR_KEY02 = 0xE2,

    IR_KEY10 = 0x22,
    IR_KEY11 = 0x02,
    IR_KEY12 = 0xC2,

    IR_KEY20 = 0xE0,
    IR_KEY21 = 0xA8,
    IR_KEY22 = 0x90,

    IR_KEY30 = 0x68,
    IR_KEY31 = 0x98,
    IR_KEY32 = 0xB0,

    IR_KEY40 = 0x30,
    IR_KEY41 = 0x18,
    IR_KEY42 = 0x7A,

    IR_KEY50 = 0x10,
    IR_KEY51 = 0x38,
    IR_KEY52 = 0x5A,

    IR_KEY60 = 0x42,
    IR_KEY61 = 0x4A,
    IR_KEY62 = 0x52,
};
#endif


typedef struct {
    uint8_t volatile cnt;
    uint8_t volatile data_code;
    uint16_t volatile user_code;
} kir_t;

void kir_init(void);
uint8_t kir_get_key(void);
void kir_get_data(void);
uint8_t kir_shift_8bit(uint8_t data);
uint16_t kir_shift_16bit(uint16_t data);

#endif

