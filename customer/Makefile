# Rules for building library

################################################################################
ROOT_PATH = ..

# Include the common make definitions.
include $(ROOT_PATH)/build/module.mk
include $(ROOT_PATH)/build/gcc.mk
include $(ROOT_PATH)/build/common.mk
include $(ROOT_PATH)/build/make.in
################################################################################
#  library path name , its name must be lead by 'lib'
LIB_CUSTOMER = libcustomer.a

# source files directory
INCS_CUSTOMER +=  ./
M_CUSTOMER_SRC_DIR = ./src/
DIRS_CUSTOMER += $(M_CUSTOMER_SRC_DIR)
SRCS_CUSTOMER += $(basename $(foreach dir,$(M_CUSTOMER_SRC_DIR),$(wildcard $(dir)/*.[cs])))

OBJS_CUSTOMER = $(addsuffix .o,$(SRCS_CUSTOMER))

# include files directory
PATH_CUSTOMER = -I$(ROOT_PATH)/inc

#specify system driver include files
PATH_CUSTOMER += -I$(ROOT_PATH)/driver/inc

INCLUDE_PATHS += $(PATH_BASIC)
INCLUDE_PATHS += $(PATH_CUSTOMER)

DIRS_INCNET:= $(shell find $(INCS_CUSTOMER) -maxdepth 4 -type d)
INCLUDE_PATHS += $(foreach dir,$(DIRS_INCNET),-I$(dir))


LIBS = $(LIB_CUSTOMER)
OBJS = $(OBJS_CUSTOMER)

CC_FLAGS += -Werror
################################################################################
.PHONY: lib install size clean

lib: $(LIBS)

$(LIB_CUSTOMER): $(OBJS_CUSTOMER)
	$(AR) -crs $@ $^ $(LINKER_OPTION)

install: $(LIBS)
	$(CP) -t $(ROOT_PATH)/lib $^

size:
	$(SIZE) -t $(LIBS)

clean:
#	-rm -f $(LIBS) $(OBJS) $(DEPS)
	-rm -f $(LIBS)
	find . -name "*.[do]" | xargs rm -f

################################################################################
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
