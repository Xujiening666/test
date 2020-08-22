#ifndef __TAS5711_H__
#define __TAS5711_H__

#include "sys_inc.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;


#define  TAS5711_SLAVE_ADR      (0x36)

typedef union {
    struct {
        u8 offset;
        u8 value;
    };
    struct {
        u8 command;
        u8 param;
    };
} cfg_reg;

extern const cfg_reg registers[];

#define CFG_META_SWITCH (255)
#define CFG_META_DELAY  (254)
#define CFG_META_BURST  (253)

/*-----------------STANDBY-----------------*/
//TODO
#define STANDBY_HI()
#define STANDBY_LOW()
#define PA_RES_HIGH()
#define PA_RES_LOW()
#define PA_PDN_HIGH()
#define PA_PDN_LOW()


#define   MASTER_SOFT_MUTE    0xff
#define   MASTER_N60DB 	       168
#define   MASTER_N59DB 	       166
#define   MASTER_N58DB 	       164
#define   MASTER_N57DB 	       162
#define   MASTER_N56DB 	       160
#define   MASTER_N55DB 	       158
#define   MASTER_N54DB 	       156
#define   MASTER_N53DB 	       154
#define   MASTER_N52DB 	       152
#define   MASTER_N51DB 	       150
#define   MASTER_N50DB 	       148
#define   MASTER_N49DB 	       146
#define   MASTER_N48DB 	       144
#define   MASTER_N47DB 	       142
#define   MASTER_N46DB 	       140
#define   MASTER_N45DB 	       138
#define   MASTER_N44DB 	       136
#define   MASTER_N43DB 	       134
#define   MASTER_N42DB 	       132
#define   MASTER_N41DB 	       130
#define   MASTER_N40DB 	       128
#define   MASTER_N39DB 	       126
#define   MASTER_N38DB 	       124
#define   MASTER_N37DB 	       122
#define   MASTER_N36DB 	       120
#define   MASTER_N35DB 	       118
#define   MASTER_N34DB 	       116
#define   MASTER_N33DB 	       114
#define   MASTER_N32DB 	       112
#define   MASTER_N31DB 	       110
#define   MASTER_N30DB 	       108
#define   MASTER_N29DB 	       106
#define   MASTER_N28DB 	       104
#define   MASTER_N27DB 	       102
#define   MASTER_N26DB 	       100
#define   MASTER_N25DB 	       98
#define   MASTER_N24DB 	       96
#define   MASTER_N23DB 	       94
#define   MASTER_N22DB 	       92
#define   MASTER_N21DB 	       90
#define   MASTER_N20DB 	       88
#define   MASTER_N19DB 	       86
#define   MASTER_N18DB 	       84
#define   MASTER_N17DB 	       82
#define   MASTER_N16DB 	       80
#define   MASTER_N15DB 	       78
#define   MASTER_N14DB 	       76
#define   MASTER_N13DB 	       74
#define   MASTER_N12DB 	       72
#define   MASTER_N11DB 	       70
#define   MASTER_N10DB 	       68
#define   MASTER_N09DB 	       66
#define   MASTER_N08DB 	       64
#define   MASTER_N07DB 	       62
#define   MASTER_N06DB 	       60
#define   MASTER_N05DB 	       58
#define   MASTER_N04DB 	       56
#define   MASTER_N03DB 	       54
#define   MASTER_N02DB 	       52
#define   MASTER_N01DB 	       50
#define   MASTER_00DB 	  48
#define   MASTER_01DB     46
#define   MASTER_02DB     44
#define   MASTER_03DB     42
#define   MASTER_04DB     40
#define   MASTER_05DB     38
#define   MASTER_06DB     36
#define   MASTER_07DB     34
#define   MASTER_08DB     32
#define   MASTER_09DB     30
#define   MASTER_10DB     28
#define   MASTER_11DB     26
#define   MASTER_12DB     24
#define   MASTER_13DB     22
#define   MASTER_14DB     20
#define   MASTER_15DB     18
#define   MASTER_16DB     16
#define   MASTER_17DB     14
#define   MASTER_18DB     12
#define   MASTER_19DB     10
#define   MASTER_20DB     8
#define   MASTER_21DB     6
#define   MASTER_22DB     4
#define   MASTER_23DB     2
#define   MASTER_24DB     0

#define CFG_META_SWITCH (255)
#define CFG_META_DELAY  (254)
#define CFG_META_BURST  (253)

#define MAX_TRE_TAS5711   8
#define MAX_BAS_TAS5711   8
#define MAX_VOL_TAS5711   32
#define DEF_BASS_TAS5711    MAX_BAS_TAS5711/2
#define DEF_TREBLE_TAS5711  MAX_TRE_TAS5711/2
#define DEF_VOL_TAS5711   20

enum
{
  SLEP_OFF = 0,
  SLEP_5MIN,
  SLEP_10MIN,
  SLEP_15MIN,
  SLEP_30MIN,
  SLEP_60MIN,
  SLEP_90MIN
};

extern void tas5711_Init(void);
extern void tas5711_SetVol(u8 vol);
extern void tas5711_Mute(void);
extern void tas5711_Unmute(void);
extern void tas5711_SetEQ(u8 eq);
extern void tas5711_SetTre(u8 trb);
extern void tas5711_SetBas(u8 bass);
extern void transmit_registers(cfg_reg const *r, u16 n);
extern void load_tas5711_reg(void);
extern void load_music_coef(void);
extern void load_movie_coef(void);
extern void load_voice_coef(void);
extern void load_def_coef(void);
#endif
