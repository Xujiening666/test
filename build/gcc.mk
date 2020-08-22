# Common rules for GCC Makefile

################################################################################
ROOT_PATH = ..
CC_DIR = $(ROOT_PATH)/tools/toolchain/bin
CC_PREFIX = $(CC_DIR)/arm-none-eabi-

DEBUG = 0
HARDFP = 1
MDK_DEBUG_ENABLE = 1
################################################################################
CP = cp -u

################################################################################
AS = $(CC_PREFIX)as
CC = $(CC_PREFIX)gcc
CPP = $(CC_PREFIX)g++
LD = $(CC_PREFIX)ld
AR = $(CC_PREFIX)ar
OBJCOPY = $(CC_PREFIX)objcopy
OBJDUMP = $(CC_PREFIX)objdump
READELF = $(CC_PREFIX)readelf
SIZE = $(CC_PREFIX)size

ifeq ($(HARDFP),1)
	FLOAT_ABI = hard
else
	FLOAT_ABI = softfp
endif

ifeq ($(MDK_DEBUG_ENABLE),1)
	DEBUG_OPT = -gdwarf-2
else
	DEBUG_OPT = -g
endif

# Set the compiler CPU/FPU options.
CPU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=$(FLOAT_ABI)


CC_FLAGS = $(CPU) -c $(DEBUG_OPT) -fno-common -fmessage-length=0 -Wall \
	-fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer \
	-MMD -MP


CC_SYMBOLS = -DTARGET_M4 -DTARGET_CORTEX_M \
	-DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC \
	-D__CORTEX_M4 -DARM_MATH_CM4 -D__FPU_PRESENT=1 \
	-DUSE_FREERTOS

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -Wl,-Map=$(PROJECT).map,--cref

LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys


CC_FLAGS += -DNDEBUG -Os

CC_FLAGS += -DUSE_ALPS_CONFIG

#CC_FLAGS += -Werror
#CC_FLAGS += -g

INCLUDE_PATHS =

################################################################################
%.o: %.asm
	$(CC) $(CPU) -c -x assembler-with-cpp $(INCLUDE_PATHS) -o $@ $<

%.o: %.s
	$(CC) $(CPU) -c -x assembler-with-cpp $(INCLUDE_PATHS) -o $@ $<

%.o: %.S
	$(CC) $(CPU) -c -x assembler-with-cpp $(INCLUDE_PATHS) -o $@ $<

%.o: %.c
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99 $(INCLUDE_PATHS) -o $@ $<

%.o: %.cpp
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<


