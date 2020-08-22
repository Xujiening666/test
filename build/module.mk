
#------------------------ Modules selection ---------------------------------------------------------
############################### Driver Module ##################################
DRV_CHIP := y
DRV_CMSIS := y
DRV_CORE :=
DRV_DRIVERLIB := y
DRV_MOD_END := y

HAL_AUSS       := y
HAL_ASRC       := y
HAL_CCU        := y
HAL_DMA        := y
HAL_GPIO       := y
HAL_TWI        := y
HAL_I2S        := y
HAL_IRRX       := y
HAL_IWDG       := n
HAL_KEYADC     := y
HAL_LEDC       := y
HAL_USB        := n
HAL_NORF       := n
HAL_PMU        := y
HAL_PWM        := y
HAL_RTC        := y
HAL_SD         := y
HAL_SPDIF      := y
HAL_SPI        := y
HAL_TIM        := n
HAL_TIMESTAMP  := n
HAL_TRNG       := n
HAL_UART       := n
HAL_TIMER	   := y
HAL_EID        := y

#kernel modules
KER_OS_FREERTOS := y
KER_OSLIB := y

################################ net modules ####################################
NET_BTC_CONTROLLER := y
NET_BLEC_CONTROLLER := y
NET_BT_HOST := y
NET_BTC_SLTBT := y
NET_BTC_RFC := y

############################### component modules ###############################
COMP_NORFLASH := y
COMP_USB_HOST_CORE := y
COMP_USB_HOST_CLASS_MSC := y
COMP_USB_HOST_INTERFACE := y
COMP_USB_DEV_CORE := y
COMP_USB_DEV_CLASS_COMP := y
COMP_USB_DEV_INTERFACE := y

#USB-MSC-PRJ/
COMP_FAT_FS := y

# soundlib
SOUNDLIB := y

#Audio Algorithms
AALG_VERSION   := v2_0
AALG           := y
AALG_WAV       :=
AALG_APE       := y
AALG_AAC       := y
AALG_OGG       := y
AALG_WMA       := y
AALG_MP3       := y
AALG_FLAC      :=
AALG_SBC       := y
AALG_CVSD      :=
AALG_WAVENC    :=
AALG_MP3ENC    := y
AALG_MP2ENC    := y
AALG_OPUSENC   := y
#spectrum is included by eq, whitch located in ram now
AALG_EQ        := y
AALG_DRC       := y
AALG_ECHO      := y
AALG_REVERB    := y
AALG_SDPRO     := y
AALG_SRC       := y
AALG_TSPS      := y
AALG_TBVC      := y
AALG_DR        := y
AALG_AENC      := y
AALG_ADMFD     := y
AALG_ADMOMINI  := y
AALG_HC        := y
AALG_NHS       := y
AALG_PLC       :=
AALG_WAV_ENC   := y

#Audio Framework
AFW_AIN        := y
AFW_AOUT       := y
AFW_STREAMIO   := y
AFW_PLAYER     := y
AFW_RECORDER   := y
AUDIO_TESTER   := n

# common libc
COMP_LIBC  := y

COMP_DUALCORE := y

# module
COMP_MODULE := y

# system
COMP_SYSTEM := y

# sd
COMP_SD := y
