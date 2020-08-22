#ifndef _UTILS_H_
#define _UTILS_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void UtilsDelay(unsigned long ulCount);
extern unsigned int HAL_GetTick(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //__UTILS_H__
