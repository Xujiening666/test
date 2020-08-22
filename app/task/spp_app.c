/*
spp app
*/
#define LOG_LEV 3
#include "sys_inc.h"
#include "user_config.h"

extern bool spp_send_data_do(uint8_t idx, const char * cmd, uint8_t len);
int spp_app_data_send(uint8_t *value, uint8_t value_len)
{
    return spp_send_data_do(0, (const char *)value, value_len);
}


void spp_app_dump_buffer(char *name, uint8_t *buffer, uint16_t buff_size)
{
    printf_debug("%s: ", name);
    for(int i = 0; i < buff_size; i++)
        printf_debug("%02X ", buffer[i]);
    printf_debug("\n");
}

uint8_t spp_app_data_receive(uint8_t *buffer, uint8_t buff_size)
{
    uint8_t sendbuf[]={1,2,3,4,5,6,7,8,9,0};
    spp_app_dump_buffer("spp_app_data_receive", buffer, buff_size);
    spp_app_data_send(sendbuf, sizeof(sendbuf));

    return 0;
}

extern void spp_register_receive_data_handler(uint8_t (*func_handler)(uint8_t *ptr, uint8_t len));

void spp_app_init()
{
    logi("%s", __func__);
    spp_register_receive_data_handler(spp_app_data_receive);
}

void spp_app_exit()
{

}


