#ifndef _EQ_INTERFACE_YH_H_
#define _EQ_INTERFACE_YH_H_

#include "Sys_inc.h"
#include "sys_types.h"

enum _PAGE_ID
{
    Page_Music = 0,
    Page_Effect,
    Page_Mic,
    Page_Input_1,
    Page_Input_2
};


enum E_Item_Type
{
    frequence = 0,
    Q,                        //1
    Gain,                    //2
    EqType,                    //3
    InputSource,            //4        输入源
    InputVolume,            //5        输入音量
    SwitchTone,                //6        变调开关
    Alpha_tone,                //7        音调
    Alpah_voice_speed,        //8        音速
    ResonanceCoefficient,    //9        共振系数

    CompensationGain,        //10    补偿增益
    VoltageLimiteSwitch,    //11    压限开关
    StartupLevel,            //12    启动电平
    StartupTime,            //13    启动时间
    CompressRatio,            //14    压缩比例
    ReleaseTime,            //15    释放时间

    SwitchMix,                //16    混响总开关
    MixPreDelay,            //17    混响预延时
    MixTime,                //18    混响时间
    ExpandCoefficient,        //19    扩散系数
    MixVolume,                //20    混响音量
    ThroughVolume,            //21    直达音量
    LowFreDampingPoint,        //22    低频阻尼频点
    LowFreDamping,            //23    低频阻尼
    HighFreDampingPoint,    //24    高频阻尼频点
    HighFreDamping,            //25    高频阻尼
    LowpassFilterFre,        //26    低通滤波截止频率
    HighpassFilterFre,        //27    高通滤波截止频率

    SwitchEcho,                //28    回声总开关
    EchoLowPassFilter,        //29    低通滤波
    EchoThroughVolume,        //30    回声直达音量
    EchoMixVolume,            //31    回声混响音量
    EchoLowpassFre,            //32    低通频率
    EchoDelay,                //33    回声延时
    EchoDeplicateRatio,        //34    回声重复率

    SWITCH_MIX_ECHO_MODE = 0xFB,        //        切换混响回声模式
    ENABLE_EQ = 0xFC,        //        使能EQ        pageID,  0xFC , group , reserved , 0.0/1.0
    RESET_ALL = 0xFE,        //      复位全部
};


//A0
enum {
    BTD_COMFIRM = 0x07,
    BTD_PRE = 0x08,
    BTD_NEXT = 0x09,
    BTD_PP = 0x0A,
    BTD_BLE_SWITCH_BT = 0x17,
    BTD_SWITCH_TO_AUX = 0x18,

    BTD_SET_VOLUME = 0x21,

    BTD_GET_LIST = 0xB0,
    BTD_GET_LIST_DONE = 0xB1,
    BTD_GET_CUR_PLAY_INFO = 0xB2,

    BTD_SET_PLAY_EFFECT = 0xB3,
    BTD_GET_VOLUME = 0xB4,
    BTD_BLE_OFF = 0xB5,
    BTD_BLE_ON = 0xB6
};

typedef struct S_EQ_DATA{
    uint8_t pageId;
    uint8_t type;
    uint8_t itemIndex;
    uint8_t extend;
    float   data;
}s_eq_data;

void OnEqInit();
void OnPlaySetting(uint8_t* dataBuffer) ;
void OnSelectPlay(uint8_t* dataBuffer);
void OnUpdateFileList(uint8_t* dataBuffer);
void OnPlayPauseFavorateFolder(uint8_t* dataBuffer);
void OnPlayControl(uint8_t* dataBuffer);
void OnQuerySupportMode(uint8_t* dataBuffer);
void OnMixMode(uint8_t* dataBuffer);
void OnTrack(int8_t* dataBuffer);
void OnMicEq(uint8_t* dataBuffer);
void OnGuitarTrack(uint8_t* dataBuffer);
void OnFavorate(uint8_t* dataBuffer);

void InitEqChannel(uint8_t page);
void InitEcho();
void InitTone(uint8_t page);
void OnBleEqTimer(xTimerHandle xTimer);

#endif

