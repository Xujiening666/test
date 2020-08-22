/* The Alps Open Source Project
 * Copyright (c) 2016 - 2017 Smartlink Technology Co. Ltd. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

struct ring_buffer {
    uint8_t *base;
    uint32_t size;              /* max number elements, must be 2^n */
    volatile uint32_t rpos;     /* read position */
    volatile uint32_t wpos;     /* write position */
    uint32_t boundary;          /* Virtual buffers size, must be (size * 2^n) */
};

/* init the struct ring_buffer before use it */
static inline void ring_buffer_init(struct ring_buffer *rbuf,
    uint8_t *buffer_base, uint32_t size)
{
    rbuf->base = buffer_base;
    rbuf->size = size;
    rbuf->rpos = 0;
    rbuf->wpos = 0;

    rbuf->boundary = rbuf->size * 2;
}

/* get read pointer */
static inline uint8_t *ring_buffer_get_rptr(struct ring_buffer *rbuf)
{
    return rbuf->base + (rbuf->rpos % rbuf->size);
}

/* get write pointer */
static inline uint8_t *ring_buffer_get_wptr(struct ring_buffer *rbuf)
{
    return rbuf->base + (rbuf->wpos % rbuf->size);
}

/* update write position */
static inline void ring_buffer_update_wpos(
    struct ring_buffer *rbuf, uint32_t n)
{
    rbuf->wpos = (rbuf->wpos + n) % rbuf->boundary;
}

/* update read position */
static inline void ring_buffer_update_rpos(
    struct ring_buffer *rbuf, uint32_t n)
{

    rbuf->rpos = (rbuf->rpos + n) % rbuf->boundary;
}

/* get read avail */
uint32_t ring_buffer_get_r_avail(struct ring_buffer *rbuf);

/* get write avail */
uint32_t ring_buffer_get_w_avail(struct ring_buffer *rbuf);

/* get avail contiguous read block */
void ring_buffer_get_r_block(
    struct ring_buffer *rbuf, uint32_t *avail, uint8_t **rptr);

/* get avail contiguous write block */
void ring_buffer_get_w_block(
    struct ring_buffer *rbuf, uint32_t *avail, uint8_t **wptr);

void ring_buffer_write(struct ring_buffer *rbuf, uint8_t *buf, uint32_t bytes);
void ring_buffer_read(struct ring_buffer *rbuf, uint8_t *buf, uint32_t bytes);

struct audio_share_buffer {
    struct ring_buffer ctrl;

    uint16_t period_size;
    uint8_t store_ch;
    uint8_t store_sample_size;
};

typedef struct audio_share_buffer_params {
    void *buf;
    uint32_t size;
    uint16_t period_size;
    uint8_t store_ch;
    uint8_t store_sample_size;
} audio_share_buffer_params_t;

int32_t audio_share_buffer_init(struct audio_share_buffer *sbuf,
    audio_share_buffer_params_t *params);
void audio_share_buffer_deinit(struct audio_share_buffer *sbuf);
void audio_share_buffer_reset(struct audio_share_buffer *sbuf);
uint32_t audio_share_buffer_avail_read_samples(struct audio_share_buffer *sbuf);
void audio_share_buffer_read(struct audio_share_buffer *sbuf,
    void **pbuf, uint32_t *psamples, uint8_t ch, uint8_t sample_size);
void audio_share_buffer_get_read_ptr(struct audio_share_buffer *sbuf, void **p_ptr);
void audio_share_buffer_update_read_ptr(struct audio_share_buffer *sbuf, uint32_t samples);

uint32_t audio_share_buffer_avail_write_samples(struct audio_share_buffer *sbuf);


void audio_share_buffer_write(struct audio_share_buffer *sbuf,
    const void **pbuf, uint32_t *psamples, uint8_t ch, uint8_t sample_size,
    bool interleave_data);


#ifdef __cplusplus
}
#endif

#endif /* _RING_BUFFER_H_ */
