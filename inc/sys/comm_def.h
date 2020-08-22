#ifndef _COMM_DEF_H_
#define _COMM_DEF_H_

#include "compiler.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_config.h"
#include "sys_retcode.h"
#include "debug.h"
#include "slxxxx.h"

#ifdef __cplusplus
 extern "C" {
#endif

//#define USB_DEBUG  /*usb host define */
//#define BRAD_DEBUG
#define BTC_DEBUG
//#define I2S_DEBUG


#define PATTERN_PASS		(*((uint32_t*)0x40000100) = 0xFC)
#define PATTERN_FAIL		(*((uint32_t*)0x40000100) = 0xFD)

//#define Pattern_Goto(GoP)		(*((uint32_t*)0x40000100) = GoP)

#define PATTERN_END			while(1)

#define REG_RESERVED		0x11111111
#define REG_READ_ONLY		0x22222222


#ifdef __cplusplus
}
#endif

#endif // __HW_TYPES_H__
