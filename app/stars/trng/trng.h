#ifndef _TRNG_H_
#define _TRNG_H_


void trng_init(void);
void trng_deinit(void);
uint32_t trng_get_random_data(void);

void trng_module_init(void);
void trng_module_deinit(void);
uint32_t trng_get_data(void);


#endif

