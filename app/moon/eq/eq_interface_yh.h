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
    InputSource,            //4        ����Դ
    InputVolume,            //5        ��������
    SwitchTone,                //6        �������
    Alpha_tone,                //7        ����
    Alpah_voice_speed,        //8        ����
    ResonanceCoefficient,    //9        ����ϵ��

    CompensationGain,        //10    ��������
    VoltageLimiteSwitch,    //11    ѹ�޿���
    StartupLevel,            //12    ������ƽ
    StartupTime,            //13    ����ʱ��
    CompressRatio,            //14    ѹ������
    ReleaseTime,            //15    �ͷ�ʱ��

    SwitchMix,                //16    �����ܿ���
    MixPreDelay,            //17    ����Ԥ��ʱ
    MixTime,                //18    ����ʱ��
    ExpandCoefficient,        //19    ��ɢϵ��
    MixVolume,                //20    ��������
    ThroughVolume,            //21    ֱ������
    LowFreDampingPoint,        //22    ��Ƶ����Ƶ��
    LowFreDamping,            //23    ��Ƶ����
    HighFreDampingPoint,    //24    ��Ƶ����Ƶ��
    HighFreDamping,            //25    ��Ƶ����
    LowpassFilterFre,        //26    ��ͨ�˲���ֹƵ��
    HighpassFilterFre,        //27    ��ͨ�˲���ֹƵ��

    SwitchEcho,                //28    �����ܿ���
    EchoLowPassFilter,        //29    ��ͨ�˲�
    EchoThroughVolume,        //30    ����ֱ������
    EchoMixVolume,            //31    ������������
    EchoLowpassFre,            //32    ��ͨƵ��
    EchoDelay,                //33    ������ʱ
    EchoDeplicateRatio,        //34    �����ظ���

    SWITCH_MIX_ECHO_MODE = 0xFB,        //        �л��������ģʽ
    ENABLE_EQ = 0xFC,        //        ʹ��EQ        pageID,  0xFC , group , reserved , 0.0/1.0
    RESET_ALL = 0xFE,        //      ��λȫ��
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

