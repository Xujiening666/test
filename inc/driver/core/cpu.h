#ifndef _CPU_H_
#define _CPU_H_

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

enum scb_reg_e {
    REG_HFSR            = 0x0,
    REG_MMFAR,
    REG_BFAR,
    REG_HFSR_VECTTBL,
    REG_HFSR_FORCED,
    REG_DFSR,
};


//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************
extern unsigned long CPUcpsid(void);
extern unsigned long CPUcpsie(void);
extern unsigned long CPUprimask(void);
extern void CPUwfi(void);
extern unsigned long CPUbasepriGet(void);
extern void CPUbasepriSet(unsigned long ulNewBasepri);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __CPU_H__
