#ifndef _UPDATE_H_
#define _UPDATE_H_
#include "sys_inc.h"
#include "integer.h"
#include "sys_types.h"

// function try_update would return one of the velue below once error happened
enum update_ret {
    UPDATE_RET_MOUNT_ERROR            = -1,
    UPDATE_RET_FILE_NOT_EXIST         = 1,
    UPDATE_RET_FILE_HEADER_ERROR,
    UPDATE_RET_FILE_DATA_CRC_ERROR,
    UPDATE_RET_FILE_DATA_HEADER_ERROR,
    UPDATE_RET_FILE_GET_CODE_ERROR,
    UPDATE_RET_FILE_STAMP_IS_SAME,
    UPDATE_RET_MALLOC_ERROR,
    UPDATE_RET_UNSUPPORT_FORMAT,
    UPDATE_RET_USER_TERMINATED
};

typedef struct _UPDATE_CALLBACK
{
    bool (*PreUpdate)(uint32_t cur_dev);    //return 0 will break update process
    void (*ProcessedUpdate)(void);
    bool (*BeginUpdateFlash)(void);
}UpdateCallback;

enum _LASTERROR_
{
    UpdateSuccess = 0,
    UnkownDev,
    SdInitFail,
    CodeFlagUnfix,
    StorageErrorCrc,
    NorCheckCrcError,
};

//update process use os, make the os run normally
void register_updatecallback(UpdateCallback* pUpdateCallback);
int try_update(uint32_t cur_dev);
void update_setvolume(uint32_t volume);

#endif
