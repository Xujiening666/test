# Rules for building application

################################################################################
ROOT_PATH = ..

# Include the common make definitions.
include $(ROOT_PATH)/build/module.mk
include $(ROOT_PATH)/build/gcc.mk
include $(ROOT_PATH)/build/common.mk
include $(ROOT_PATH)/build/make.in

################################################################################
PROJECT = sl

################################################################################
# Source files
################################################################################
DIRS_PROJECT = $(ROOT_PATH)/app/
DIRS_PROJECT += $(ROOT_PATH)/app/task
DIRS_PROJECT += $(ROOT_PATH)/app/stars
DIRS_PROJECT += $(ROOT_PATH)/app/stars/codec
DIRS_PROJECT += $(ROOT_PATH)/app/stars/gpio
DIRS_PROJECT += $(ROOT_PATH)/app/stars/i2s
DIRS_PROJECT += $(ROOT_PATH)/app/stars/pmu
DIRS_PROJECT += $(ROOT_PATH)/app/stars/rtc
DIRS_PROJECT += $(ROOT_PATH)/app/stars/sd
DIRS_PROJECT += $(ROOT_PATH)/app/stars/twi
DIRS_PROJECT += $(ROOT_PATH)/app/stars/trng
DIRS_PROJECT += $(ROOT_PATH)/app/stars/uart
DIRS_PROJECT += $(ROOT_PATH)/app/stars/usb
DIRS_PROJECT += $(ROOT_PATH)/app/stars/spdif
DIRS_PROJECT += $(ROOT_PATH)/app/stars/ledc
DIRS_PROJECT += $(ROOT_PATH)/app/stars/irrx
DIRS_PROJECT += $(ROOT_PATH)/app/stars/spi
DIRS_PROJECT += $(ROOT_PATH)/app/stars/pwm
DIRS_PROJECT += $(ROOT_PATH)/app/stars/timer

DIRS_PROJECT += $(ROOT_PATH)/app/moon
DIRS_PROJECT += $(ROOT_PATH)/app/moon/audio
#DIRS_PROJECT += $(ROOT_PATH)/app/moon/audio/plugin
DIRS_PROJECT += $(ROOT_PATH)/app/moon/device
DIRS_PROJECT += $(ROOT_PATH)/app/moon/disp
DIRS_PROJECT += $(ROOT_PATH)/app/moon/motor
DIRS_PROJECT += $(ROOT_PATH)/app/moon/eq
DIRS_PROJECT += $(ROOT_PATH)/app/moon/fm
DIRS_PROJECT += $(ROOT_PATH)/app/moon/key
DIRS_PROJECT += $(ROOT_PATH)/app/moon/scene
DIRS_PROJECT += $(ROOT_PATH)/app/moon/setting
DIRS_PROJECT += $(ROOT_PATH)/app/moon/system
DIRS_PROJECT += $(ROOT_PATH)/app/moon/update
DIRS_PROJECT += $(ROOT_PATH)/app/moon/xfs
DIRS_PROJECT += $(ROOT_PATH)/app/moon/bt_test
DIRS_PROJECT += $(ROOT_PATH)/app/moon/ble
DIRS_PROJECT += $(ROOT_PATH)/app/moon/ai
SRCS_PROJECT += $(basename $(foreach dir,$(DIRS_PROJECT),$(wildcard $(dir)/*.[cs])))
OBJS_PROJECT = $(addsuffix .o,$(SRCS_PROJECT))
OBJS = $(OBJS_PROJECT)

################################################################################
# Inc files
################################################################################
PATH_APP = -I$(ROOT_PATH)/app
PATH_APP += -I$(ROOT_PATH)/app/task

PATH_APP += -I$(ROOT_PATH)/app/stars
PATH_APP += -I$(ROOT_PATH)/app/stars/codec
PATH_APP += -I$(ROOT_PATH)/app/stars/gpio
PATH_APP += -I$(ROOT_PATH)/app/stars/i2s
PATH_APP += -I$(ROOT_PATH)/app/stars/pmu
PATH_APP += -I$(ROOT_PATH)/app/stars/rtc
PATH_APP += -I$(ROOT_PATH)/app/stars/sd
PATH_APP += -I$(ROOT_PATH)/app/stars/twi
PATH_APP += -I$(ROOT_PATH)/app/stars/trng
PATH_APP += -I$(ROOT_PATH)/app/stars/uart
PATH_APP += -I$(ROOT_PATH)/app/stars/usb
PATH_APP += -I$(ROOT_PATH)/app/stars/spdif
PATH_APP += -I$(ROOT_PATH)/app/stars/ledc
PATH_APP += -I$(ROOT_PATH)/app/stars/irrx
PATH_APP += -I$(ROOT_PATH)/app/stars/spi
PATH_APP += -I$(ROOT_PATH)/app/stars/pwm
PATH_APP += -I$(ROOT_PATH)/app/stars/timer

PATH_APP += -I$(ROOT_PATH)/app/moon
PATH_APP += -I$(ROOT_PATH)/app/moon/audio
#PATH_APP += -I$(ROOT_PATH)/app/moon/audio/plugin
PATH_APP += -I$(ROOT_PATH)/app/moon/device
PATH_APP += -I$(ROOT_PATH)/app/moon/disp
PATH_APP += -I$(ROOT_PATH)/app/moon/motor
PATH_APP += -I$(ROOT_PATH)/app/moon/eq
PATH_APP += -I$(ROOT_PATH)/app/moon/fm
PATH_APP += -I$(ROOT_PATH)/app/moon/key
PATH_APP += -I$(ROOT_PATH)/app/moon/scene
PATH_APP += -I$(ROOT_PATH)/app/moon/setting
PATH_APP += -I$(ROOT_PATH)/app/moon/system
PATH_APP += -I$(ROOT_PATH)/app/moon/update
PATH_APP += -I$(ROOT_PATH)/app/moon/bt_test
PATH_APP += -I$(ROOT_PATH)/app/moon/ble
INCLUDE_PATHS += $(PATH_BASIC)
INCLUDE_PATHS += $(PATH_APP)

################################################################################
# Lib files
################################################################################
LIBRARY_PATHS = -L$(ROOT_PATH)/lib

# There are strong and weak symbols in "libraries" and "app", it may link
# to the weak symbol as they are statc libraries, so use --whole-archive to
# solve this problem.
LIBRARIES = -Wl,--whole-archive -ldriver -lkernel -lcomponent -laudio -laudiocomp -lnet -lrfc -Wl,--no-whole-archive

LINKER_SCRIPT = $(ROOT_PATH)/build/sl.ld

CC_FLAGS += -Werror

################################################################################
ifeq ($(MDK_DEBUG_ENABLE),1)
	ELF_EXT = axf
else
	ELF_EXT = elf
endif

.PHONY: install clean $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)

install: $(ROOT_PATH)/build/out/$(PROJECT).bin $(ROOT_PATH)/build/out/$(PROJECT).hex size

    $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT): $(OBJS)
	$(CC) $(LD_FLAGS) $(COMPILER_OPTION) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $(OBJS) $(LIBRARIES) $(LD_SYS_LIBS)

    $(ROOT_PATH)/build/out/$(PROJECT).bin: $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)
	$(OBJCOPY) -O binary $< $@ $(BIN_UTILS_OPTION) 1>/dev/null
	$(OBJDUMP) -D $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT) > $(ROOT_PATH)/build/out/$(PROJECT).dis
	#$(READELF) -a $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT) > $(ROOT_PATH)/build/out/$(PROJECT).elf_header 1>/dev/null
	#$(ROOT_PATH)/build/bin2hex $(ROOT_PATH)/build/out/$(PROJECT).bin $(ROOT_PATH)/build/out/ram.hex 1>/dev/null
	$(ROOT_PATH)/build/tool/makeCodeScriptParser 1>/dev/null
	$(ROOT_PATH)/build/tool/makeup 1>/dev/null
	rm -rf $(ROOT_PATH)/build/out/sl_temp.bin
	mv sl.map $(ROOT_PATH)/build/out/sl.map

    $(ROOT_PATH)/build/out/$(PROJECT).hex: $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)
	#$(OBJCOPY) -O ihex $< $@ $(BIN_UTILS_OPTION) 1>/dev/null

    $(ROOT_PATH)/build/out/$(PROJECT).lst: $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)
	$(OBJDUMP) -Sdh $< > $@ 1>/dev/null

size:
	$(SIZE) $(ROOT_PATH)/build/out/$(PROJECT).$(ELF_EXT)

clean:
#	-rm -f $(OBJS) $(DEPS)
	find $(ROOT_PATH)/app -name "*.[do]" | xargs rm -f
	-rm -f $(ROOT_PATH)/build/out/*

################################################################################
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
