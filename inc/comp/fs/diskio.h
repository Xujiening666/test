/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2014          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"


/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

typedef struct __STRUCT_DISK_IO_CALLBACK{
	DSTATUS (*pMMCDiskStatus)(void);
	DSTATUS (*pMMCDiskInit)(void);
	DSTATUS (*pMMCDiskRead)(BYTE *buff,DWORD sector,UINT count);
	DRESULT (*pMMCDiskWrite)(BYTE *buff,DWORD sector,UINT count);
	DRESULT (*pMMCDiskIoctl)(BYTE cmd,void *buff);
	DRESULT	(*pMMCReadFailCallback)(void);
}DISK_IO_CALLFOPS;

DSTATUS sd_disk_status(void);
DSTATUS sd_disk_initialize(void);
DSTATUS disk_init_bypass(void);
DRESULT sd_disk_read_start(BYTE *buff, DWORD sector);
DRESULT sd_disk_read2(BYTE *buff, DWORD sector, UINT count);
DRESULT sd_disk_write(BYTE *buff, DWORD sector, UINT count);
DRESULT sd_disk_write2(BYTE *buff, DWORD sector);
DRESULT sd_disk_ioctl(BYTE cmd, void *buff);
uint8_t wait_sd_ready(uint32_t timeout);
DRESULT usb_get_status(void);
uint32_t fsca_get_usb_capacity();
void fsca_set_usb_capacity(uint32_t capacity);
/* -------------- fix sl28xx rom bug -------- */
DSTATUS SL28xx_USB_disk_status(void);
DSTATUS SL28xx_USB_disk_initialize(void);
DRESULT SL28xx_USB_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT SL28xx_USB_disk_write(BYTE *buff, DWORD sector, UINT count);
DRESULT SL28xx_USB_disk_ioctl(
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
);

/*---------------------------------------*/
/* Prototypes for disk control functions */


DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
void disk_config(DISK_IO_CALLFOPS fops);


/* Definitions of physical drive number for each drive */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_USB		1	/* Example: Map USB MSD to physical drive 1 */


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at _FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at _MAX_SS != _MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at _USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

#define SECTOR_SZ       512
#define FS_CACHE_COUNT  4


typedef DSTATUS (*xfs_disk_status_func)(void);
typedef DSTATUS (*xfs_disk_initialize_func)(void);
typedef DRESULT (*xfs_disk_read_start_func)(BYTE *buff, DWORD sector);
typedef DSTATUS (*xfs_disk_get_status_func)(void);
typedef DRESULT (*xfs_disk_read_func)(BYTE *buff, DWORD sector, UINT count);
typedef DRESULT (*xfs_disk_write_func)(BYTE* buff, DWORD sector, UINT count);
typedef DRESULT (*xfs_disk_ioctl_func)(BYTE cmd, void* buff);
typedef DRESULT (*xfs_disk_failcb_func)(void);


typedef struct {
    xfs_disk_status_func status;
    xfs_disk_initialize_func init;
    xfs_disk_read_start_func read_start;
    xfs_disk_get_status_func get_status;
    xfs_disk_read_func read;
    xfs_disk_write_func write;
    xfs_disk_ioctl_func ioctl;
    xfs_disk_failcb_func failcb;
} xfs_disk_interface_t;

typedef struct fs_cache_s {
    BYTE tmp[SECTOR_SZ];
    BYTE *cache[FS_CACHE_COUNT];

    DWORD r_sec;
    DWORD w_sec;
    DWORD tmp_sec;
    DWORD total_sec;

    volatile uint32_t wptr;
    volatile uint32_t rptr;
    volatile uint32_t disk_status;
    volatile bool wait_flag;
    volatile bool lock;
    volatile bool single_mode;
    volatile bool tmp_init;
    bool cache_enable;
    uint32_t timeout;
} fs_cache_t;

extern fs_cache_t *fsca;

void fsca_init(bool sdcard);
void fsca_deinit(void);
void fsca_enable(bool enable);
void fsca_timeout_set(uint32_t time);
int fsca_disk_read_continue(void);

void xfs_config_sdmmc_interface(void);
void xfs_config_usb_interface(void);
void xfsdisk_delay_for_bt_enable(BOOL bEnable);
uint8_t xfs_is_usb_unit_ready();

int disk_is_lock(void);
int fs_cache_wait_complete(void);
int fsca_read_disk(BYTE *buff, DWORD sector);
int fsca_disk_read_start(void);
void USB_disk_read_dma(BYTE *buff, DWORD sector, UINT count);
void USB_disk_write_dma (BYTE *buff, DWORD sector,UINT count);


#ifdef __cplusplus
}
#endif

#endif
