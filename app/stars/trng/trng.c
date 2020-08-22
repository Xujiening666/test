#include "sys_inc.h"
#include "user_config.h"
#include "trng.h"


AT(.trng_init_seg)
void trng_init(void)
{
    trng_module_init();
}

#if 0
AT(.trng_init_seg)
void trng_deinit(void)
{
    trng_module_deinit();
}
#endif

AT(.trng_sram_seg)
uint32_t trng_get_random_data(void)
{
    uint32_t data;

    do {
        data = trng_get_data();
    } while(data == 0);

    return data;
}


