/*
* Copyright (c) 2016 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File :
@ History :
@ Description :

*/

#ifndef _SERIAL_DEBUG_IO_H_
#define _SERIAL_DEBUG_IO_H_

#ifdef __cplusplus
 extern "C" {
#endif

void serial_debug_io_init(void *hdl);
int xprintf(bool block, const char *format, ...);
int scanf_debug(const char *format, ...);

#if DEBUG_LOG_EN
#define printf_debug(format, ...) xprintf(true, format, ##__VA_ARGS__)
#define cpu1_printf_debug(format, ...) xprintf(false, format, ##__VA_ARGS__)
void printf_array(const void *buff, uint16_t len);
#else
#define printf_debug(format, ...)
#define cpu1_printf_debug(format, ...)
#define printf_array(a, b)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SERIAL_DEBUG_IO_H_ */
