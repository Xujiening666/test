/*
* Copyright (c) 2016 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File : compiler.h
@ History :
    Author          Date                Comment
    weijincai

@ Description :

*/

#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#define ffs(x)          __builtin_ffs(x)
#define popcount(x)     __builtin_popcount(x)
#define ctz(x)          __builtin_ctz(x)
#define clz(x)          __builtin_clz(x)


#ifdef USE_BUILTIN_STRING_FUNC
#define memcpy(dst, src, bytes)     __builtin_memcpy(dst, src, bytes)
#define memset(dst, val, bytes)     __builtin_memset(dst, val, bytes)
#define memmove(dst, src, size)     __builtin_memmove(dst, src, size)
#define memcmp(buf1, buf2, count)   __builtin_memcmp(buf1, buf2, count)
#else
void *sltk_memcpy(void *dst, const void *src, uint32_t bytes);
void *sltk_memset(void *dst, int val, uint32_t bytes);
void *sltk_memmove(void *dst, const void *src, uint32_t count);
int sltk_memcmp(const void *buf1, const void *buf2, uint32_t count);
#endif

#define __maybe_unused __attribute__((unused))

#define MODULE_FUNC __attribute__((weak))

#ifdef __cplusplus
}
#endif

#endif // _COMPILER_H_
