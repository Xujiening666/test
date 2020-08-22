#ifndef _SYSTEM_TYPES_H_
#define _SYSTEM_TYPES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#if 0
#include <stdint.h>

#else

#if   defined ( __CC_ARM )
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler */
  #define __STATIC_INLINE  static __inline

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler */
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler */
  #define __STATIC_INLINE  static __inline

#elif defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler */
  //#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler */
  #define __INLINE static __attribute__((__always_inline__)) inline
  #define __STATIC_INLINE  static inline

#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline

#elif defined ( __TMS470__ )
  #define __ASM            __asm                                      /*!< asm keyword for TI CCS Compiler */
  #define __STATIC_INLINE  static inline

#elif defined ( __TASKING__ )
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler */
  #define __STATIC_INLINE  static inline

#elif defined ( __CSMC__ )
  #define __packed
  #define __ASM            _asm                                      /*!< asm keyword for COSMIC Compiler */
  #define __INLINE         inline                                    /*!< inline keyword for COSMIC Compiler. Use -pc99 on compile line */
  #define __STATIC_INLINE  static inline

#else
  #error Unknown compiler
#endif

/* IO definitions (access restrictions to peripheral registers) */
/**
    \defgroup CMSIS_glob_defs CMSIS Global Defines

    <strong>IO Type Qualifiers</strong> are used
    \li to specify the access to peripheral variables.
    \li for automatic generation of peripheral register debug information.
*/
#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

#define _IN		// use for declare's mark
#define _OUT
#define _INOUT

typedef signed char             INT8;
typedef signed char             int8_t;
typedef unsigned char           UINT8;
typedef unsigned char           uint8_t;

typedef signed short            INT16;
typedef unsigned short          UINT16;
typedef signed short            int16_t;
typedef unsigned short          uint16_t;

typedef signed int              INT32;
typedef unsigned int            UINT32;
typedef signed int              int32_t;
typedef unsigned int            uint32_t;

typedef signed long long        INT64;
typedef unsigned long long      UINT64;
typedef signed long long        int64_t;
typedef unsigned long long      uint64_t;

//FIXME:add by wxp
typedef int                     ssize_t;
typedef unsigned int            size_t;
typedef int                     intptr_t;
typedef unsigned int            uintptr_t;

#ifndef FALSE
#define FALSE           (0)
#endif
#ifndef TRUE
#define TRUE            (!FALSE)
#endif

typedef uint32_t BOOL;
typedef unsigned long DWORD;

typedef INT32           RET_CODE;

typedef UINT32          HANDLE;
#define INVALID_HANDLE  ((HANDLE)0xFFFFFFFF)
typedef void*              VP;

//typedef short           WCHAR;
typedef short *         LPWCHAR;
typedef short *         LPWSTR;
typedef char            CHAR;
typedef char *          PCHAR;
typedef char *          LPCHAR;
typedef char *          LPSTR;
typedef void *          PVOID;
typedef void *          LPVOID;

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#endif

#define BIT(n)          (1<<(n))

#define BYTE0(n)        ((uint8_t)((uint16_t)(n)))
#define BYTE1(n)        ((uint8_t)(((uint16_t)(n))>>8))
#define BYTE2(n)        ((uint8_t)(((uint16_t)(((uint32_t)(n))>>8))>>8))
#define BYTE3(n)        ((uint8_t)(((uint16_t)(((uint32_t)(n))>>16))>>8))

#define R4BYTE(p)       (*(uint32_t *)(p))
#define R2BYTE(p)       (*(uint16_t *)(p))
#define R1BYTE(p)       (*(uint8_t *)(p))

#define W4BYTE(p)       (*(uint32_t *)(p))
#define W2BYTE(p)       (*(uint16_t *)(p))
#define W1BYTE(p)       (*(uint8_t *)(p))

#define get_cfg(domain, key) domain##_##key


//*****************************************************************************
//
// Define ERROR TYPES
//
//*****************************************************************************

/* Exported macro ------------------------------------------------------------*/
#ifndef NULL
#define NULL            ((void *) 0)
#endif

#ifndef HAL_NULL
  #define HAL_NULL      ((void *) 0)
#endif


//*****************************************************************************
//
// Define a boolean type, and values for true and false.
//
//*****************************************************************************
typedef unsigned char tBoolean;
typedef unsigned char bool;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

//*****************************************************************************
//
// Macros for hardware access, both direct and via the bit-band region.
//
//*****************************************************************************
#define HWREG(x)                                                              \
        (*((volatile unsigned long *)(x)))
#define HWREGH(x)                                                             \
        (*((volatile unsigned short *)(x)))
#define HWREGB(x)                                                             \
        (*((volatile unsigned char *)(x)))
#define HWREGBITW(x, b)                                                       \
        HWREG(((unsigned long)(x) & 0xF0000000) | 0x02000000 |                \
              (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITH(x, b)                                                       \
        HWREGH(((unsigned long)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITB(x, b)                                                       \
        HWREGB(((unsigned long)(x) & 0xF0000000) | 0x02000000 |               \
               (((unsigned long)(x) & 0x000FFFFF) << 5) | ((b) << 2))

static inline int isspace(int c)
{
    return c == ' ' || ((unsigned)c-'\t') < 5;
}

static inline int isdigit(int a)
{
    return (((unsigned)(a)-'0') < 10);
}

#ifdef __cplusplus
}
#endif


#endif // __HW_TYPES_H__
