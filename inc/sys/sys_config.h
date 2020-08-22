#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_


/***************************************************************************
 *               system config
 ***************************************************************************/
#define SYS_CHIP_ID                 CHIP_SL6800     //芯片型号
#define SYS_PLATFORM                PLATFORM_CHIP   //平台型号
#define SYS_STACK_SIZE              512             //系统堆栈大小，单位:word

/***************************************************************************
 *               dual cpu
***************************************************************************/
#define DUAL_CPU_EN                 1               //使能双核

/***************************************************************************
 *               pmu config
***************************************************************************/
#define PMU_EXIST                   1               //PMU是否存在
#define PMU_VERSION                 1               //PMU版本

/***************************************************************************
 *               Interrupt Priority
***************************************************************************/
/* 中断优先级低于等于5的ISR，不能使用FreeRTOS接口
 * 数值越小，优先级越高
 */
#define IRQn_BROM_WR_PRIO           0
#define IRQn_AUSSTX_DMA_PRIO        2
#define IRQn_AUSSRX_DMA_PRIO        2
#define IRQn_I2STX_DMA_PRIO         2
#define IRQn_I2SRX_DMA_PRIO         2
#define IRQn_ASRC_DMA_PRIO          6

#define IRQn_RTC_PRCM_PRIO          6
#define IRQn_USB_PRIO               6
#define IRQn_BT_PRIO                7
#define IRQn_BLE_PRIO               7
#define IRQn_SDMMC_PRIO             8
#define IRQn_GPIOB_PRIO             8
#define IRQn_GPIOC_PRIO             8
#define IRQn_NORF_PRIO              9
#define IRQn_MSGC0_PRIO             15

#define IRQn_TIMER0_PRIO            10
#define IRQn_TIMER1_PRIO            10
#define IRQn_TIMER2_PRIO            10
#define IRQn_TIMER3_PRIO            10

/***************************************************************************
 *               Task Priority
***************************************************************************/
/* 数值越大，优先级越大
 * 任务优先级最高不能大于19
 */
#define UPDATE_TASK_HIGHEST_PRIO    19              //更新任务必须使用最高优先级
#define BT_ISR_BH_TASK_PRIO         19
#define PMU_ISR_BH_TASK_PRIO        1
#define BT_CTRL_TASK_PRIO           9
#define BT_AUDIO_TASK_PRIO          6
#define BT_HOST_TASK_PRIO           7
#define BLE_HOST_TASK_PRIO          7
#define DEC_TASK_PRIO               1
#define KARAOKE_TASK_PRIO           9
#define LINEIN_TASK_PRIO            9
#define BLE_APP_TASK_PRIO           1
#define MAIN_TASK_PRIO              1
#define RECORDE_TASK_PRIO           1
#define TRACK_TASK_PRIO             9
#define SPDIF_TASK_PRIO             9
#define TIMER_TASK_PRIO             1

/***************************************************************************
 *               Task stack size in word
***************************************************************************/
#define BT_ISR_BH_TASK_STACK_SIZE   (256)
#define PMU_ISR_BH_TASK_STACK_SIZE  (64)
#define BT_CTRL_TASK_STACK_SIZE     (256+64)
#define BT_AUDIO_TASK_STACK_SIZE    (256)
#define BT_HOST_TASK_STACK_SIZE     (256)
#define BLE_HOST_TASK_STACK_SIZE    (256)
#define DEC_TASK_STACK_SIZE         (768)
#define KARAOKE_TASK_STACK_SIZE     (256)
#define LINEIN_TASK_STACK_SIZE      (200)
#define MAIN_TASK_STACK_SIZE        (512)//(512+64)
#define RECORDE_TASK_STACK_SIZE     (256)
#define TRACK_TASK_STACK_SIZE       (256)
#define SPDIF_TASK_STACK_SIZE       (256)
#define BLE_APP_TASK_STACK_SIZE     (128)/* Modify by arunboy 20200601 */
#define BLE_SEND_RECORD_FILE        (256)


/***************************************************************************
 *               bt config
 ***************************************************************************/
#define BT_ISR_BH_IN_TASK           1               //BT ISR使用任务作为下半部

/***************************************************************************
                 watchdog config
 ***************************************************************************/
#define WATCHDOG_EN                 0               //使用看门狗，如果出现挂住会重启系统
#define WATCHDOG_TIMEOUT            5               //单位:秒

/***************************************************************************
 *               power/sleep config
 ***************************************************************************/
#define BT_CON_AUTO_SLEEP_EN        0               //蓝牙连接时自动睡眠，防止某些手机不会自动发送SNIFF请求
#define BT_CON_AUTO_SLEEP_WAIT      40000           //单位:毫秒, iphone < 30秒

#define BT_DIS_AUTO_SLEEP_EN        0               //蓝牙断开时自动睡眠，在没有使能蓝牙断开时自动关机功能时，可以节省系统功耗
#define BT_DIS_AUTO_SLEEP_MAX       800             //unit: slot

/***************************************************************************
 *               debug config
 ***************************************************************************/
#define DEBUG_WR_IRQ_EN             1               //使能写保护功能，保护代码段，防止系统堆栈溢出
#define DEBUG_FAULT_DIAGNOSIS       1               //使能异常信息解析功能，更加方便阅读，需要占用更多代码空间
#define DEBUG_EXCEPTION_INIT        1               //使能异常报警功能
#define DEBUG_LOG_EN                1               //UART打印总开关
#define DEBUG_DUMP_TASK_EN          0
#define DEBUG_DEEP_SLEEP_EN         0               //使能休眠led指示灯
#define DEBUG_BT_DEBUG_EN           0

#endif


