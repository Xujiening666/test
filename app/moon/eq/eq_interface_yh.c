#define LOG_LEV 0
#include "sys_inc.h"
#include "user_config.h"
#include "eq_interface_yh.h"

#define LOCK_USE_SEMAPHORE 1

#if LOCK_USE_SEMAPHORE
static SemaphoreHandle_t xSemaphore;

#define eq_buf_lock()      xSemaphoreTake(xSemaphore, portMAX_DELAY)
#define eq_buf_unlock()    xSemaphoreGive(xSemaphore)
#else
#define eq_buf_lock()      taskENTER_CRITICAL()
#define eq_buf_unlock()    taskEXIT_CRITICAL()
#endif

#define MAX_PARAM_NUM   64
uint8_t* g_eqBuff;//[MAX_PARAM_NUM * 8] __attribute__((aligned(4)));

uint8_t g_eqParamNum;
TaskHandle_t eq_handler;

void on_set_eq_data(uint8_t* buff,uint32_t length);
uint8_t* get_eq_param_buff();

AT(.eq_seg)
void pushEqData(s_eq_data* eqDataNew)
{
    eq_buf_lock();
    if(g_eqParamNum < 64) { //abandon data if datas exceed the range of buffer
        s_eq_data* pEqData;
        for(uint8_t i=0; i< g_eqParamNum; i++) {
            pEqData = (s_eq_data*)(g_eqBuff + i*8);
            if((eqDataNew->pageId == pEqData->pageId) &&
                (eqDataNew->type == pEqData->type) &&
                (eqDataNew->itemIndex == pEqData->itemIndex)) {

                    memcpy((uint8_t*)pEqData,(uint8_t*)eqDataNew,8);
                    eq_buf_unlock();
                    return;
            }
        }
        memcpy(g_eqBuff + g_eqParamNum * 8, (uint8_t*)eqDataNew, 8 );
        g_eqParamNum++;
    }
    eq_buf_unlock();

}

AT(.eq_seg)
void OnEqTimerTask(void *handle)
{
    while(1) {
        msleep(200);
        if(g_eqParamNum == 0){
            continue;
        }

        eq_buf_lock();
        //uint8_t* eqBuff = get_eq_param_buff();
        //memcpy(eqBuff,g_eqBuff,g_eqParamNum*8);
        on_set_eq_data(g_eqBuff,g_eqParamNum*8);
        g_eqParamNum = 0;
        eq_buf_unlock();
    }
}

AT(.eq_seg)
void OnDataReceive(uint8_t* buffer, uint8_t length)
{
    if(buffer[0] != 0x59 || buffer[1] != 0x59)
    {
        logi("error head");
        return;
    }

    if(buffer[length-1] != 0x48 || buffer[length-2] != 0x48) {
        logi("error end");
        return;
    }

    static bool bInit = 0;
    if(!bInit) {
        InitEqChannel(0);   //page music
        //InitEqChannel(2);   //page mic
        //InitEqChannel(3);   //page input1
        //InitEqChannel(4);   //page input2
        //InitEcho();         //page effect
        bInit = 1;
    }

    switch(buffer[2])
    {
        case 0xA0:
            //OnPlaySetting(buffer + 3);
            break;
        case 0xB0:
            //OnSelectPlay(buffer + 3);
            break;
        case 0xC0:
            ///OnUpdateFileList(buffer + 3);
            break;
        case 0xD0:
            //OnFavorate(buffer + 3);
            break;
        case 0xE0:
            //OnPlayPauseFavorateFolder(buffer + 3);
            break;
        case 0xF0:
            //OnPlayControl(buffer + 3);
            break;
        case 0xF1:
            //OnQuerySupportMode(buffer + 3);
            break;
        case 0xF2:
            OnMixMode(buffer + 4);
            break;
        case 0xF3:
            OnTrack((int8_t*)(buffer + 4));
            break;
        case 0xF4:
            OnMicEq(buffer + 3);
            break;
        case 0xF5:
            OnGuitarTrack(buffer + 3);
            break;

    }
}

AT(.eq_seg)
void OnPlaySetting(uint8_t* dataBuffer)
{

}

void OnSelectPlay(uint8_t* dataBuffer)
{
}

void OnUpdateFileList(uint8_t* dataBuffer)
{
}

void OnFavorate(uint8_t* dataBuffer)
{
}

void OnPlayPauseFavorateFolder(uint8_t* dataBuffer)
{
}

void OnPlayControl(uint8_t* dataBuffer)
{
}

void OnQuerySupportMode(uint8_t* dataBuffer)
{

}

void OnMixMode(uint8_t* dataBuffer)
{
}

AT(.eq_seg)
void OnTrack(int8_t* dataBuffer)
{
    s_eq_data eqData;
    eqData.pageId= 0;//page music
    eqData.type= Gain;
    for(int i=0; i< 7;i++)
    {
        eqData.itemIndex= i;
        eqData.data= dataBuffer[i]*1.0;
        pushEqData(&eqData);
    }
}

AT(.eq_seg)
void OnMicEq(uint8_t* dataBuffer)
{
    uint8_t buff[8];
    memset(buff,0xFF,8);  //unrecognize type data

    switch(dataBuffer[0])
    {
        case 0x00:
            {
                buff[0] = 0;//page music,  voice speed is allocate in music page but not mic
                buff[1] = Alpah_voice_speed;
                switch(dataBuffer[1])
                {
                    case 1: //man high
                        *(float*)(buff+4) = -0.24;
                        break;
                    case 2: //man middle
                        *(float*)(buff+4) = -0.52;
                        break;
                    case 3: //normal
                        *(float*)(buff+4) = 0;
                        break;
                    case 4: //woman high
                        *(float*)(buff+4) = 0.4;
                        break;
                    case 5: //woman middle
                        *(float*)(buff+4) = 0.2;
                        break;
                    case 6: //children
                        *(float*)(buff+4) = 0.6;
                        break;
                }
                pushEqData((s_eq_data*)buff);
            }
            break;
        case 0x01:
            {
                buff[0] = 2;    //page mic
                buff[1] = Gain; //type gain
                for(int i=0;i< 5;i++)
                {
                    buff[2] = i;
                    *(float*)(buff + 4) = (float)((int8_t)dataBuffer[i+1]);
                    pushEqData((s_eq_data*)buff);
                }
            }
            break;
        case 0x02:
            {
                buff[0] = 1;    //page effect
                buff[1] = EchoMixVolume; //type gain
                *(float*)(buff + 4) = (float)((int8_t)dataBuffer[1]);
                pushEqData((s_eq_data*)buff);
            }
            break;
        case 0x03:
            {
                buff[0] = 1;    //page effect
                buff[1] = EchoDelay; //type gain
                *(float*)(buff + 4) = (float)((int8_t)dataBuffer[1]);
                pushEqData((s_eq_data*)buff);
            }
            break;
    }


}

//--------------------
AT(.eq_seg)
void OnGuitarTrack(uint8_t* dataBuffer)
{
    uint8_t buff[8];
    buff[0] = 3;//3: input1   4:input2
    buff[1] = Gain;
    for(int i=0; i< 7;i++)
    {
        buff[2] = i;
        *(float*)(buff + 4) = (float)(dataBuffer[i+1]);
        pushEqData((s_eq_data*)buff);
    }
}
static bool eq_inited = false;

//------------------------------------------------
AT(.eq_seg)
void OnEqInit()
{
    if(!eq_inited){
        g_eqBuff = get_eq_param_buff();
        g_eqParamNum = 0;
#if LOCK_USE_SEMAPHORE
        xSemaphore = xSemaphoreCreateMutex();
        xSemaphoreGive(xSemaphore);
#endif
        xTaskCreate(OnEqTimerTask, "eq task",
            BT_CTRL_TASK_STACK_SIZE, NULL, BLE_HOST_TASK_PRIO, &eq_handler);
        eq_inited = true;
    }
    //g_eqParamNum = 0;

}

AT(.eq_seg)
void OnEqExit()
{
    if(eq_inited){
        vTaskDelete(eq_handler);
        eq_inited = false;
    }
}

const float initMusicTypeEqData[] = {
    // eq  ----------------
    250.0,
    500.0,
    1000.0,
    2000.0,
    4000.0,
    8000.0,
    16000.0
};

AT(.eq_seg)
void InitEqChannel(uint8_t page)
{
    s_eq_data eqData;
    eqData.pageId= page;
    for(uint16_t i=0; i< 7 ;i++)
    {
        eqData.type= frequence;
        eqData.itemIndex= i;
        eqData.data = initMusicTypeEqData[i];
        pushEqData(&eqData);
    }
}

AT(.eq_seg)
void InitEcho()
{
    uint8_t buff[8];
    buff[0] = 1;        //page effect

    buff[1] = SWITCH_MIX_ECHO_MODE;
    *(float*)(buff+4) = 1.0;
    pushEqData((s_eq_data*)buff);


    buff[1] = EchoLowPassFilter;
    //*(float*)(buff+4) = 1.0;
    pushEqData((s_eq_data*)buff);

    buff[1] = EchoLowpassFre;
    //*(float*)(buff+4) = 1.0;
    pushEqData((s_eq_data*)buff);

    buff[1] = EchoDeplicateRatio;
    *(float*)(buff+4) = 85;
    pushEqData((s_eq_data*)buff);

}

/*

*/
AT(.eq_seg)
void InitTone(uint8_t page)
{
    uint8_t buff[8];
    buff[0] = page;
    buff[1] = SwitchTone;
    *(float*)(buff+4) = 1.0;
    pushEqData((s_eq_data*)buff);
#if 0
    buff[1] = Alpah_voice_speed;    //default value
    *(float*)(buff+4) = 1.0;
    pushEqData(buff,8);

    buff[1] = Alpah_voice_speed;
    *(float*)(buff+4) = 1.0;
    pushEqData(buff,8);
#endif

}


