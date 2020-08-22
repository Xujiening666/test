#ifndef _SYSTEM_HAL_H_
#define _SYSTEM_HAL_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys_hal_def.h"
#include "sys_hal_conf.h"


/** @addtogroup HAL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @defgroup HAL_Exported_Macros HAL Exported Macros
  * @{
  */

#define PATTERN_PASS		(*((uint32_t*)0x40000100) = 0xFC)
#define PATTERN_FAIL		(*((uint32_t*)0x40000100) = 0xFD)

#define Pattern_Goto(GoP)		(*((uint32_t*)0x40000100) = GoP)

#define PATTERN_END			while(1)

#define REG_RESERVED		0x11111111
#define REG_READ_ONLY		0x22222222

#ifdef __cplusplus
}
#endif


#endif /* ___SYSTEM_HAL_DEF */


