/*
* Copyright (c) 2017 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@ Description :

*/

#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include "sys_inc.h"
#include "loadcode.h"

void get_decbuf(void **pbuf, uint32_t *pbytes);
void get_sndoutbuf(void **pbuf, uint32_t *pbytes);
void get_sndinbuf(void **pbuf, uint32_t *pbytes);

#ifdef __cplusplus
}
#endif

#endif /* _SCENE_MANAGER_H_ */
