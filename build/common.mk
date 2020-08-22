
#------------------------ Include selection ---------------------------------------------------------
PATH_BASIC := -I$(ROOT_PATH)/inc
PATH_BASIC += -I$(ROOT_PATH)/inc/sys
PATH_BASIC += -I$(ROOT_PATH)/inc/app

PATH_BASIC += -I$(ROOT_PATH)/inc/driver
PATH_BASIC += -I$(ROOT_PATH)/inc/driver/core
PATH_BASIC += -I$(ROOT_PATH)/inc/driver/chip
PATH_BASIC += -I$(ROOT_PATH)/inc/driver/cmsis
PATH_BASIC += -I$(ROOT_PATH)/inc/driver/driverlib
PATH_BASIC += -I$(ROOT_PATH)/inc/driver/driverlib/soundlib

PATH_BASIC += -I$(ROOT_PATH)/inc/kernel
PATH_BASIC += -I$(ROOT_PATH)/inc/kernel/freertos
PATH_BASIC += -I$(ROOT_PATH)/inc/kernel/freertos/portable/cm4f
PATH_BASIC += -I$(ROOT_PATH)/inc/kernel/oslib

PATH_BASIC += -I$(ROOT_PATH)/inc/comp/
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/norflash
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/comm/libc
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/comm/dualc
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/audio
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/module
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/system
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/sd
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/fs
PATH_BASIC += -I$(ROOT_PATH)/inc/comp/update

PATH_BASIC += -I$(ROOT_PATH)/inc/net

PATH_BASIC += -I$(ROOT_PATH)/inc/rfc
