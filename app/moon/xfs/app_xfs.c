#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "xfs.h"


/* If music mode and record mode disable, ignore fatfs_streamio */
#define FATFS_STREAMIO_IGNORE  ((!MODE_MUSIC_EN) && (!MODE_RECORD_EN))

/* If music mode and record mode and update mode disable, ignore filesystem */
#define FILESYSTEM_IGNORE ((!MODE_MUSIC_EN) && (!MODE_RECORD_EN) && (!MODE_DISKUPDATE_EN))

#if FATFS_STREAMIO_IGNORE
void* create_fatfs_stream(void)
{
    return NULL;
}
#endif


#if FILESYSTEM_IGNORE
void fatfs_init(void)
{
}

FRESULT xfs_cmd(UINT cmd, UINT val)
{
    return FR_INT_ERR;
}

/* scan music files */
uint32_t xfscan(void)
{
    return 0;
}

/* Open a music file */
FRESULT xfopen_num(XFILE *fp, char *name, uint32_t num)
{
    return FR_INT_ERR;
}

FRESULT xfs_dir_seek(uint32_t dir_num)
{
    return FR_INT_ERR;
}

FRESULT xfs_get_current_num(uint32_t *p_dir_num,
    uint32_t *p_num)
{
    return FR_INT_ERR;
}

FRESULT xfs_get_total_num(uint32_t *p_total_dir_num,
    uint32_t *p_total_num)
{
    return FR_INT_ERR;
}

FRESULT xfs_get_folder_file_num_range(uint32_t cur_num,
    uint32_t *p_folder_min_file_num,
    uint32_t *p_folder_file_count)
{
    return FR_INT_ERR;
}

/* Open or create a file */
FRESULT xfopen(XFILE* fp, const TCHAR* path, BYTE mode)
{
    return FR_INT_ERR;
}

/* Close an open file object */
FRESULT xfclose(XFILE* fp)
{
    return FR_INT_ERR;
}

/* Read data from the file */
FRESULT xfread(XFILE* fp, void* buff, UINT btr, UINT* br)
{
    return FR_INT_ERR;
}

/* Write data to the file */
FRESULT xfwrite(XFILE* fp, const void* buff, UINT btw, UINT* bw)
{
    return FR_INT_ERR;
}

/* Move file pointer of the file object */
FRESULT xflseek(XFILE* fp, FSIZE_t ofs)
{
    return FR_INT_ERR;
}

/* Flush cached data of the writing file */
FRESULT xfsync(XFILE* fp)
{
    return FR_INT_ERR;
}

/* Open a directory */
FRESULT xfopendir(XDIR* dp, const TCHAR* path)
{
    return FR_INT_ERR;
}

/* Close an open directory */
FRESULT xfclosedir(XDIR* dp)
{
    return FR_INT_ERR;
}

/* Read a directory item */
FRESULT xfreaddir(XDIR* dp, XFILINFO* fno)
{
    return FR_INT_ERR;
}

/* Create a sub directory */
FRESULT xfmkdir(const TCHAR* path)
{
    return FR_INT_ERR;
}

/* Mount/Unmount a logical drive */
FRESULT xfmount(XFS* fs, const TCHAR* path, BYTE opt)
{
    return FR_INT_ERR;
}

bool xfeof(XFILE* fp)
{
    return false;
}

FSIZE_t xftell(XFILE* fp)
{
    return 0;
}

FSIZE_t xfsize(XFILE* fp)
{
    return 0;
}

FRESULT xfunmount(const TCHAR* path)
{
    return FR_INT_ERR;
}

int fsca_disk_read_continue(void)
{
    return 0;
}
#endif
