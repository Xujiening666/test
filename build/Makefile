# Scan which dir exist 

AUDIO_MAKE      = $(shell if [ -d "${PWD}/audio" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
DRIVER_MAKE     = $(shell if [ -d "${PWD}/driver" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
COMPONENT_MAKE  = $(shell if [ -d "${PWD}/component" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
KERNEL_MAKE     = $(shell if [ -d "${PWD}/kernel" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
RFC_MAKE        = $(shell if [ -d "${PWD}/rfc" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
NET_MAKE        = $(shell if [ -d "${PWD}/net" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
CUSTOMER_MAKE   = $(shell if [ -d "${PWD}/customer" ]; then echo "${MAKE}" ; else echo "@echo" ; fi;)
#$(warning "audio(${AUDIO_MAKE})")
#$(warning "driver(${DRIVER_MAKE})")
#$(warning "component(${COMPONENT_MAKE})")
#$(warning "kernel(${KERNEL_MAKE})")
#$(warning "rfc(${RFC_MAKE})")
#$(warning "net(${NET_MAKE})")

# Rules for building application

################################################################################

# Include the common make definitions.
include ./module.mk
include ./gcc.mk
include ./common.mk

################################################################################
PROJECT = sl

LINKER_SCRIPT = ./sl.ld

CC_FLAGS += -Werror

################################################################################
ifeq ($(MDK_DEBUG_ENABLE),1)
	ELF_EXT = axf
else
	ELF_EXT = elf
endif

.PHONY: app lst size clean loader ./out/$(PROJECT).$(ELF_EXT) lib lib_clean loader_clean

app:
	$(MAKE) -C $(ROOT_PATH)/app/pmu_config install 1>/dev/null
	$(MAKE) -C $(ROOT_PATH)/app install 1>/dev/null

sysconfig:

loader:
	$(MAKE) -C $(ROOT_PATH)/app/loader/build app
	
loader_clean:
	$(MAKE) -C $(ROOT_PATH)/app/loader/build clean

lst: ./out/$(PROJECT).lst

size:
	$(SIZE) ./out/$(PROJECT).$(ELF_EXT)

clean:
	$(MAKE) -C $(ROOT_PATH)/app/pmu_config/ clean 1>/dev/null
	$(MAKE) -C $(ROOT_PATH)/app clean  1>/dev/null

lib:
	#SubWCRev.exe $(ROOT_PATH) $(ROOT_PATH)/build/tool/version.tmpl $(ROOT_PATH)/inc/sys/version.h 1>/dev/null
	$(DRIVER_MAKE) -C $(ROOT_PATH)/driver install  1>/dev/null
	$(KERNEL_MAKE) -C $(ROOT_PATH)/kernel install   1>/dev/null
	$(COMPONENT_MAKE) -C $(ROOT_PATH)/component install  1>/dev/null
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/algorithms install 1>/dev/null
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/framework install 1>/dev/null
	$(RFC_MAKE) -C $(ROOT_PATH)/rfc install  1>/dev/null
	$(NET_MAKE) -C $(ROOT_PATH)/net install  1>/dev/null
	#$(CUSTOMER_MAKE) -C $(ROOT_PATH)/customer install  1>/dev/null
lib_clean:
	$(DRIVER_MAKE) -C $(ROOT_PATH)/driver clean  1>/dev/null
	$(KERNEL_MAKE) -C $(ROOT_PATH)/kernel clean   1>/dev/null
	$(COMPONENT_MAKE) -C $(ROOT_PATH)/component clean  1>/dev/null
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/algorithms clean 1>/dev/null
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/framework clean 1>/dev/null
	$(RFC_MAKE) -C $(ROOT_PATH)/rfc clean  1>/dev/null
	$(NET_MAKE) -C $(ROOT_PATH)/net clean  1>/dev/null
	#$(CUSTOMER_MAKE) -C $(ROOT_PATH)/customer clean  1>/dev/null

driver:
	$(DRIVER_MAKE) -C $(ROOT_PATH)/driver install 1>/dev/null

driver_clean:
	$(DRIVER_MAKE) -C $(ROOT_PATH)/driver clean 1>/dev/null

kernel:
	$(KERNEL_MAKE) -C $(ROOT_PATH)/kernel install 1>/dev/null

kernel_clean:
	$(KERNEL_MAKE) -C $(ROOT_PATH)/kernel clean 1>/dev/null

component:
	$(COMPONENT_MAKE) -C $(ROOT_PATH)/component install 1>/dev/null

component_clean:
	$(COMPONENT_MAKE) -C $(ROOT_PATH)/component clean 1>/dev/null

audio:
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/algorithms install 1>/dev/null

audio_clean:
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/algorithms clean 1>/dev/null

audiocomp:
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/framework install 1>/dev/null

audiocomp_clean:
	$(AUDIO_MAKE) -C $(ROOT_PATH)/audio/framework clean 1>/dev/null

net:
	$(NET_MAKE) -C $(ROOT_PATH)/net install 1>/dev/null

net_clean:
	$(NET_MAKE) -C $(ROOT_PATH)/net clean 1>/dev/null
	
customer:
	$(CUSTOMER_MAKE) -C $(ROOT_PATH)/customer install 1>/dev/null

customer_clean:
	$(CUSTOMER_MAKE) -C $(ROOT_PATH)/customer clean 1>/dev/null

rfc:
	$(RFC_MAKE) -C $(ROOT_PATH)/rfc install 1>/dev/null

rfc_clean:
	$(RFC_MAKE) -C $(ROOT_PATH)/rfc clean 1>/dev/null

################################################################################
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
