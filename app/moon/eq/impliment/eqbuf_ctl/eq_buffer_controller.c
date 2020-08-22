/*
    For the convenience of less ram waste, all the eq process data is stored in  'eq_param_buf', which is 516 bytes
    As a result, all afx cmd is processed one by one with the controll of 'eq_param_buff_sem_sync'
*/
#define EQ_BUF_CTL_FUNC __attribute__((section(".eqbuf_controller_seg")))

static uint8_t first_ref = 1;
SemaphoreHandle_t eq_param_buff_sem_sync;
uint8_t eq_param_buf[0x200+4] __attribute__((aligned(4)));

EQ_BUF_CTL_FUNC void eq_buff_sem_take_wrapper()
{
    if(0 == first_ref){
        if(pdPASS != osSemaphoreTake(eq_param_buff_sem_sync, pdMS_TO_TICKS(/*ONE_SECOND_IN_MS*/100))){
            loge("eq_buff lag error!");
        }
    }
}

EQ_BUF_CTL_FUNC void eq_buff_sem_give_wrapper()
{
    if(0 == first_ref){
        osSemaphoreGive(eq_param_buff_sem_sync);
    }
}
/*
    Get the eq buffer sync to 'eq_param_buff_sem_sync'
*/
EQ_BUF_CTL_FUNC uint8_t* get_eq_param_buff()
{
    if(1 == first_ref) {
        eq_param_buff_sem_sync = xSemaphoreCreateBinary();
        first_ref = 0;
        eq_buff_sem_give_wrapper();
    }
    eq_buff_sem_take_wrapper();
    return &eq_param_buf[4];
}
/*
    Get the eq buffer without sync to 'eq_param_buff_sem_sync'
*/
EQ_BUF_CTL_FUNC uint8_t* get_eq_param_buff_atom()
{
    return &eq_param_buf[0];
}

