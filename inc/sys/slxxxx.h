#ifndef _SLxxxx_H_
#define _SLxxxx_H_

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/**
  * @brief Configuration of the RISC32 Processor and Core Peripherals
  */
#define __RISC32_REV              0x0001U  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1U       /*!< SL28XX provides an MPU                     */
#define __INTC_PRIO_BITS          4U       /*!< SL28XX uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0U       /*!< Set to 1 if different SysTick Config is used  */
#ifndef __FPU_PRESENT
#define __FPU_PRESENT             1U       /*!< FPU present                                   */
#endif /* __FPU_PRESENT */

/**
 * @brief slxxxx Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum {
/******  RISC32 Processor Exceptions Numbers ****************************************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                                           */
  MemoryManagement_IRQn         = -12,      /*!< 4 RISC32 Memory Management Interrupt                   */
  BusFault_IRQn                 = -11,      /*!< 5 RISC32 Bus Fault Interrupt                                    */
  UsageFault_IRQn               = -10,      /*!< 6 RISC32 Usage Fault Interrupt                                 */
  SVCall_IRQn                   = -5,       /*!< 11 RISC32 SV Call Interrupt                                     */
  DebugMonitor_IRQn             = -4,       /*!< 12 RISC32 Debug Monitor Interrupt                           */
  PendSV_IRQn                   = -2,       /*!< 14 RISC32 Pend SV Interrupt                                    */
  SysTick_IRQn                  = -1,       /*!< 15 RISC32 System Tick Interrupt                              */
/******  SLxxxx specific Interrupt Numbers **********************************************************************/
  IRQn_WDOG                     = 0,        /*!< Window WatchDog Interrupt                                          */
  IRQn_GPIOB                    = 1,        /*!< GPIO B global Interrupt                                    */
  IRQn_GPIOC                    = 2,        /*!< GPIO C global Interrupt                                        */
  IRQn_GPIOD                    = 3,        /*!< GPIO D global Interrupt                                                */
  IRQn_KADC                     = 4,        /*!< KADC global Interrupt                              */
  IRQn_TIMER0                   = 5,        /*!< TIMER0 global Interrupt                                                */
  IRQn_TIMER1                   = 6,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_UART0                    = 7,        /*!< UART0 global  Interrupt                                                */
  IRQn_UART1                    = 8,        /*!< UART1 global  Interrupt                                                */
  IRQn_SPI                      = 9,        /*!< SPI global  Interrupt                                                  */
  IRQn_TWI                      = 10,       /*!< TWI global  Interrupt                                                  */
  IRQn_BT                       = 11,       /*!< BT Controller global  Interrupt                                    */
  IRQn_TWI1                     = 12,       /*!< TWI1 global  Interrupt                                             */
  IRQn_TWI2                     = 13,       /*!< TWI2 global  Interrupt                                              */
  IRQn_SDMMC                    = 14,       /*!< SDMMC global  Interrupt                                            */
  IRQn_BLE                      = 15,       /*!< BLE global  Interrupt                                              */
  IRQn_AUDIO_APB2DPV            = 16,       /*!< AUDIO_APB2DPV global  Interrupt                                    */
  IRQn_AUDIO_PCM                = 17,       /*!< AUDIO_PCM global  Interrupt                                    */
  IRQn_AUDIO_ARBS               = 18,       /*!< AUDIO_ARBS global  Interrupt                                   */
  IRQn_USB                      = 19,       /*!< USB global  Interrupt                                                  */
  IRQn_I2S                      = 20,       /*!< I2S global  Interrupt                                                  */
  IRQn_SIC                      = 21,       /*!< SIC global  Interrupt                                                  */
  RTC_TIMER                     = 22,       /*!< RTC_TIMER global  Interrupt                                            */
  RTC_PRCM                      = 23,       /*!< RTC_PRCM global  Interrupt                                           */
  IRQn_TRNG                     = 24,       /*!< TRNG global  Interrupt                                     */
  IRQn_NORF                     = 25,       /*!< NORF global  Interrupt                                     */
  IRQn_AUSS_ANC                 = 26,       /*!< AUSS ANC global  Interrupt                                 */
  IRQn_SPDIF                    = 27,       /*!< SPDIF global  Interrupt                                             */
  IRQn_EID                      = 28,       /*!< EID global  Interrupt                                                  */
  IRQn_MSGC0                    = 29,       /*!< CPU0 MSG global Interrupt(CPU1->CPU0), CPU1 can't see                                                 */
  IRQn_MDM                      = 30,       /*!< BTSS MDM                                                          */
  IRQn_TKC                      = 31,       /*!< TKC  global  Interrupt                                                      */
  IRQn_DMA0                     = 32,       /*!< RESERVED                                                       */
  IRQn_DMA1                     = 33,       /*!< RESERVED                                                       */
  IRQn_DMA2                     = 34,       /*!< RESERVED                                                           */
  IRQn_DMA3                     = 35,       /*!< RESERVED                                                           */
  IRQn_MSGC1                    = 36,       /*!< CPU1 MSG global Interrupt(CPU0->CPU1), CPU0 can't see                                                           */
  IRQn_TIMER2                   = 37,       /*!< RESERVED                                                       */
  IRQn_TIMER3                   = 38,       /*!< RESERVED                                                           */
  IRQn_AUSS                     = 39,       /*!< RESERVED                                                           */
  IRQn_BROM_WR                  = 40,       /*!< RESERVED                                                       */
  IRQn_IR                       = 41,       /*!< IR                                                */
  IRQn_LEDC                     = 42,       /*!< LEDC                                                                       */
  IRQn_UART2                    = 43,       /*!< UART2                                               */
  IRQn_TKC_SLD                  = 44,       /*!< TKC_SLD                                           */
  IRQn_ASRC                     = 45,       /*!< ASRC                                       */
  IRQn_DMA4                     = 46,       /*!< DMAC_CH4                                                   */
  IRQn_RESVD63                  = 47        /*!< RESERVED                                                           */
} IRQn_Type;



typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0U,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macro
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

#define GET_U32(p)			(*(uint32_t *)(p))
#define GET_U16(p)			(*(uint16_t *)(p))
#define GET_U8(p)			(*(uint8_t *)(p))

#define readb(reg)              (*(volatile uint8_t  *)(reg))
#define readw(reg)              (*(volatile uint16_t *)(reg))
#define readl(reg)              (*(volatile uint32_t *)(reg))

#define writeb(val, reg)        (*(volatile uint8_t *)(reg) = (val))
#define writew(val, reg)        (*(volatile uint16_t *)(reg) = (val))
#define writel(val, reg)        (*(volatile uint32_t *)(reg) = (val))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLxxxx_H */

