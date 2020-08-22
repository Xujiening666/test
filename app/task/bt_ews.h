#ifndef _BT_EWS_H_
#define _BT_EWS_H_

enum
{
    BT_EWS_IDLE,
    BT_EWS_RECONNECT_PEER,
    BT_EWS_RECONNECT_PHONE,
    BT_EWS_WAIT_CONNECT,
};

typedef struct{
    uint32_t  state;
    uint32_t  timeout;
}bt_ews_state_t;

void bt_ews_task_create();
void bt_ews_task_delete();

uint32_t bt_ews_get_cur_state();


#endif

