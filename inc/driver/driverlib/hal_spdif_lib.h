#ifndef _HAL_SPDIF_LIB_H_
#define _HAL_SPDIF_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "slsoundlib.h"


enum spdif_channel_e {
    SPDIF_CHANNEL_A  = 0x0,
    SPDIF_CHANNEL_B,
};

typedef struct {
    uint32_t channel;
    void (*lock_callback)();
    void (*unlock_callback)();
} spdif_param_t;

typedef struct {
    uint32_t rate;
    uint32_t sample_size;
    uint32_t channels;
} spdif_stream_info_t;

typedef struct {
    uint8_t *buf;
    uint32_t period_bytes;
    uint32_t period_count;
} spdif_rxstream_params_t;


void spdif_module_init(spdif_param_t *param);
void spdif_module_deinit(void);
void spdif_module_rxinit(void);
void spdif_module_rxdeinit(void);

void spdif_module_set_online_status(bool online);
bool spdif_module_get_online_status(void);

void spdif_module_rxstart(spdif_rxstream_params_t *params);
void spdif_module_rxstop(void);
int64_t spdif_module_get_receive_timestamp(void);
spdif_stream_info_t spdif_module_getrxinfo(void);
int spdif_module_read(void *buf, uint32_t bytes);

#ifdef __cplusplus
}
#endif

#endif
