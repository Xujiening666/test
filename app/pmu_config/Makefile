# Rules for building application

################################################################################
PMU_CONFIG_ROOT_PATH = ../../

# Common rules for GCC Makefile

################################################################################
CC_DIR = $(PMU_CONFIG_ROOT_PATH)/tools/toolchain/bin
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

ifeq ($(GC_SECTIONS),1)
	LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -Wl,-Map=$(PROJECT).map,--cref
else
	LD_FLAGS = $(CPU) --specs=nano.specs -Wl,-Map=$(PROJECT).map,--cref
endif

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
###############################################################################

PATH_BASIC := -I$(PMU_CONFIG_ROOT_PATH)/inc
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/sys
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/app

PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/driver
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/driver/core
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/driver/chip
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/driver/cmsis
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/driver/driverlib
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/driver/driverlib/soundlib

PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/kernel
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/kernel/freertos
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/kernel/freertos/portable/cm4f
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/kernel/oslib

PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/norflash
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/comm/libc
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/comm/dualc
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/audio
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/module
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/system
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/sd
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/fs
PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/comp/update

PATH_BASIC += -I$(PMU_CONFIG_ROOT_PATH)/inc/net

################################################################################
PROJECT = pmu_config

################################################################################
# Source files
################################################################################

DIRS_PROJECT += $(PMU_CONFIG_ROOT_PATH)/app/pmu_config

SRCS_PROJECT += $(basename $(foreach dir,$(DIRS_PROJECT),$(wildcard $(dir)/*.[cs])))
OBJS_PROJECT = $(addsuffix .o,$(SRCS_PROJECT))
OBJS = $(OBJS_PROJECT)

################################################################################
# Inc files
################################################################################
PATH_APP = -I$(PMU_CONFIG_ROOT_PATH)/app
PATH_APP += -I$(PMU_CONFIG_ROOT_PATH)/app/pmu_config


INCLUDE_PATHS += $(PATH_BASIC)
INCLUDE_PATHS += $(PATH_APP)

################################################################################
# Lib files
################################################################################
LIBRARY_PATHS =

# There are strong and weak symbols in "libraries" and "app", it may link
# to the weak symbol as they are statc libraries, so use --whole-archive to
# solve this problem.
LIBRARIES = -Wl,--whole-archive -Wl,--no-whole-archive

LINKER_SCRIPT = pmu_config.ld

CC_FLAGS += -Werror

################################################################################
ifeq ($(MDK_DEBUG_ENABLE),1)
	ELF_EXT = axf
else
	ELF_EXT = elf
endif

.PHONY: install clean $(PMU_CONFIG_ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)

install: ./out/$(PROJECT).bin
	rm $(PROJECT).d
	rm $(PROJECT).o
	rm $(PROJECT).map
    ./out/$(PROJECT).$(ELF_EXT): $(OBJS)
	$(CC) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $(OBJS) $(LIBRARIES) $(LD_SYS_LIBS)

    ./out/$(PROJECT).bin: ./out/$(PROJECT).$(ELF_EXT)
	$(OBJCOPY) -O binary $< $@
	#$(OBJDUMP) -D ./out/$(PROJECT).$(ELF_EXT) > ./out/$(PROJECT).dis
	#$(READELF) -a ./out/$(PROJECT).$(ELF_EXT) > ./out/$(PROJECT).elf_header
	./tool/makeCodeScriptParser
	#./tool/makeup
	mv ./out/pmu_config_temp.bin ../../build/out/pmuStartupConfig.bin

    #./out/$(PROJECT).hex: ./out/$(PROJECT).$(ELF_EXT)
	#$(OBJCOPY) -O ihex $< $@

    #./out/$(PROJECT).lst: ./out/$(PROJECT).$(ELF_EXT)
	#$(OBJDUMP) -Sdh $< > $@

size:
	#$(SIZE) $(PMU_CONFIG_ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)

clean:
#	-rm -f $(OBJS) $(DEPS)
#	find $(PMU_CONFIG_ROOT_PATH)/app -name "*.[do]" | xargs rm -f
	-rm -f out/*

################################################################################
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
