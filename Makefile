#  copyright (c) 2015 Espressif System
#
ifndef PDIR

endif

ifeq ($(COMPILE), xcc)
    AR = xt-ar
    CC = xt-xcc
    NM = xt-nm
    CPP = xt-xt++
    OBJCOPY = xt-objcopy
    OBJDUMP = xt-objdump
else
    AR = xtensa-esp108-elf-ar
    CC = xtensa-esp108-elf-gcc
    NM = xtensa-esp108-elf-nm
    CPP = xtensa-esp108-elf-g++
    OBJCOPY = xtensa-esp108-elf-objcopy
    OBJDUMP = xtensa-esp108-elf-objdump
endif

BIN_PATH ?= ./bin

ESP_COMPILE_APP_CPU ?= 0
ESP_COMBINE_APP_CPU_BIN ?= 0

ESP_LOCAL_LD ?= 0
ESP_MAP_MODE ?= 1.1

ifeq ($(ESP_COMPILE_APP_CPU), 1)
	LD_FILE_CORE = $(SDK_PATH)/ld/app.map.ld
	LD_FILE = $(SDK_PATH)/ld/app.map.ld $(SDK_PATH)/ld/app.rom.addr.ld
else
	ifeq ($(ESP_LOCAL_LD), 1)
		LD_FILE_CORE = ./ld/$(ESP_LD_FILE)
		LD_FILE = $(LD_FILE_CORE) $(SDK_PATH)/ld/pro.rom.addr.ld
	else
		LD_FILE_CORE = $(SDK_PATH)/ld/pro.map$(ESP_MAP_MODE).ld
		LD_FILE = $(LD_FILE_CORE) $(SDK_PATH)/ld/pro.rom.addr.ld
	endif
endif

CSRCS ?= $(wildcard *.c)
CPPSRCS ?= $(wildcard *.cpp)
ASRCs ?= $(wildcard *.s)
ASRCS ?= $(wildcard *.S)
SUBDIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))

ODIR := .output
OBJODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/obj

OBJS := $(CSRCS:%.c=$(OBJODIR)/%.o)     \
        $(CPPSRCS:%.cpp=$(OBJODIR)/%.o) \
        $(ASRCs:%.s=$(OBJODIR)/%.o)     \
        $(ASRCS:%.S=$(OBJODIR)/%.o)

DEPS := $(CSRCS:%.c=$(OBJODIR)/%.d)     \
        $(CPPSRCS:%.cpp=$(OBJODIR)/%.d) \
        $(ASRCs:%.s=$(OBJODIR)/%.d)     \
        $(ASRCS:%.S=$(OBJODIR)/%.d)

LIBODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/lib
OLIBS := $(GEN_LIBS:%=$(LIBODIR)/%)

IMAGEODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/image
OIMAGES := $(GEN_IMAGES:%=$(IMAGEODIR)/%)

BINODIR := $(ODIR)/$(TARGET)/$(FLAVOR)/bin
OBINS := $(GEN_BINS:%=$(BINODIR)/%)

CCFLAGS +=                  \
    -g                      \
    -Wpointer-arith         \
    -Wundef                 \
    -Werror                 \
    -Wl,-EL                 \
    -fno-inline-functions   \
    -nostdlib               \
    -mlongcalls             \
    -mtext-section-literals \
    -ffunction-sections     \
    -fdata-sections
#   -Wall

CFLAGS = $(CCFLAGS) $(DEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)
DFLAGS = $(CCFLAGS) $(DDEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)

#############################################################
# Functions
#

define ShortcutRule
$(1): .subdirs $(2)/$(1)
endef

define MakeLibrary
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(LIBODIR)/$(1).a: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(LIBODIR)
	$$(if $$(filter %.a,$$?),mkdir -p $$(EXTRACT_DIR)_$(1))
	$$(if $$(filter %.a,$$?),cd $$(EXTRACT_DIR)_$(1); $$(foreach lib,$$(filter %.a,$$?),$$(AR) xo $$(UP_EXTRACT_DIR)/$$(lib);))
	$$(AR) ru $$@ $$(filter %.o,$$?) $$(if $$(filter %.a,$$?),$$(EXTRACT_DIR)_$(1)/*.o)
	$$(if $$(filter %.a,$$?),$$(RM) -r $$(EXTRACT_DIR)_$(1))
endef

define MakeImage
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(IMAGEODIR)/$(1).out: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(IMAGEODIR)
	$$(CC) $$(LDFLAGS) $$(if $$(LINKFLAGS_$(1)),$$(LINKFLAGS_$(1)),$$(LINKFLAGS_DEFAULT) $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))) -o $$@ 
endef

define CheckBinPath
if test ! -d $(BIN_PATH); then \
echo "BIN_PATH : $(BIN_PATH) not EXIST, creat it!";	\
mkdir -p $(BIN_PATH);	\
fi;
endef

$(BINODIR)/%.bin: $(IMAGEODIR)/%.out
	@$(CheckBinPath)

	@$(RM) -r $(BIN_PATH)/user.S $(BIN_PATH)/user.dump

	@$(OBJDUMP) -x -s $< > $(BIN_PATH)/user.dump
	@$(OBJDUMP) -S $< > $(BIN_PATH)/user.S

	@$(OBJCOPY) --only-section .text -O binary $< eagle.app.v7.text.bin
	@$(OBJCOPY) --only-section .data -O binary $< eagle.app.v7.data.bin
	@$(OBJCOPY) --only-section .rodata -O binary $< eagle.app.v7.rodata.bin
	@$(OBJCOPY) --only-section .irom0.text -O binary $< eagle.app.v7.irom0text.bin
	@$(OBJCOPY) --only-section .drom0.text -O binary $< eagle.app.v7.drom0text.bin
	
ifeq ($(ESP_COMPILE_APP_CPU), 0)
	@$(RM) -r $(BIN_PATH)/irom0_flash.bin $(BIN_PATH)/drom0.bin
	@$(RM) -r $(BIN_PATH)/user.ota
	
	@python $(SDK_PATH)/tools/gen_appbin.py $< $(LD_FILE_CORE) $(ESP_COMPILE_APP_CPU) $(ESP_COMBINE_APP_CPU_BIN) $(SDK_PATH)/bin/app_cpu.bin $(BIN_PATH)
else
	@$(RM) -r $(BIN_PATH)/app_cpu.bin
	@python $(SDK_PATH)/tools/gen_appbin.py $< $(LD_FILE_CORE) $(ESP_COMPILE_APP_CPU) $(ESP_COMBINE_APP_CPU_BIN) $(BIN_PATH)/app_cpu.bin $(BIN_PATH)
endif

	@rm -f eagle.app.v7.*

#############################################################
# Rules base
# Should be done in top-level makefile only
#

all:	.subdirs $(OBJS) $(OLIBS) $(OIMAGES) $(OBINS) $(SPECIAL_MKTARGETS)

clean:
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clean;)
	$(RM) -r $(ODIR)/$(TARGET)/$(FLAVOR)

clobber: $(SPECIAL_CLOBBER)
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clobber;)
	$(RM) -r $(ODIR)

.subdirs:
	@set -e; $(foreach d, $(SUBDIRS), $(MAKE) -C $(d);)

#.subdirs:
#	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d))

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OBJODIR)/%.o: %.c
	@mkdir -p $(OBJODIR);
	$(CC) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.c
	@mkdir -p $(OBJODIR);
	@echo DEPEND: $(CC) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
$(OBJODIR)/%.o: %.cpp
	@mkdir -p $(OBJODIR);
	$(CPP) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.cpp
	@mkdir -p $(OBJODIR);
	@echo DEPEND: $(CPP) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CPP) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.s
	@mkdir -p $(OBJODIR);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJODIR)/%.d: %.s
	@mkdir -p $(OBJODIR); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.S
	@mkdir -p $(OBJODIR);
	$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<

$(OBJODIR)/%.d: %.S
	@mkdir -p $(OBJODIR); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(foreach lib,$(GEN_LIBS),$(eval $(call ShortcutRule,$(lib),$(LIBODIR))))

$(foreach image,$(GEN_IMAGES),$(eval $(call ShortcutRule,$(image),$(IMAGEODIR))))

$(foreach bin,$(GEN_BINS),$(eval $(call ShortcutRule,$(bin),$(BINODIR))))

$(foreach lib,$(GEN_LIBS),$(eval $(call MakeLibrary,$(basename $(lib)))))

$(foreach image,$(GEN_IMAGES),$(eval $(call MakeImage,$(basename $(image)))))

#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

INCLUDES += -I $(SDK_PATH)/driver_lib/include
INCLUDES += -I $(SDK_PATH)/extra_include -I $(SDK_PATH)/include
INCLUDES += -I $(SDK_PATH)/third_party/include
INCLUDES += -I $(SDK_PATH)/third_party/include/cjson
INCLUDES += -I $(SDK_PATH)/third_party/include/freertos
INCLUDES += -I $(SDK_PATH)/third_party/include/http
INCLUDES += -I $(SDK_PATH)/third_party/include/lwip -I $(SDK_PATH)/third_party/include/lwip/port -I $(SDK_PATH)/third_party/include/lwip/posix
INCLUDES += -I $(SDK_PATH)/third_party/include/ssl

ESP_TOOL ?= $(SDK_PATH)/tools/esptool.py
ESP_PORT ?= /dev/ttyUSB0
ESP_BAUD ?= 230400

ESP_FS ?= 1MB
ESP_FF ?= 40m
ESP_FM ?= qio

# 1MB spi Flash
ifeq ($(ESP_FS), 1MB)
ESP_RFINIT_ADDR = 0xFC000
ESP_SYSPARAM_ADDR = 0xFE000
endif

# 2MB spi Flash
ifeq ($(ESP_FS), 2MB)
ESP_RFINIT_ADDR = 0x1FC000
ESP_SYSPARAM_ADDR = 0x1FE000
endif

# 4MB spi Flash
ifeq ($(ESP_FS), 4MB)
ESP_RFINIT_ADDR = 0x3FC000
ESP_SYSPARAM_ADDR = 0x3FE000
endif

# 8MB spi Flash
ifeq ($(ESP_FS), 8MB)
ESP_RFINIT_ADDR = 0x7FC000
ESP_SYSPARAM_ADDR = 0x7FE000
endif

# 16MB spi Flash
ifeq ($(ESP_FS), 16MB)
ESP_RFINIT_ADDR = 0xFFC000
ESP_SYSPARAM_ADDR = 0xFFE000
endif

flash_all:
	$(ESP_TOOL) -c ESP32 -p $(ESP_PORT) -b $(ESP_BAUD) write_flash -fs $(ESP_FS) -fm $(ESP_FM) -ff $(ESP_FF)	\
	0x0 $(SDK_PATH)/bin/boot.bin	\
	0x04000 $(BIN_PATH)/drom0.bin 0x40000 $(BIN_PATH)/irom0_flash.bin	\
	$(ESP_SYSPARAM_ADDR) $(SDK_PATH)/bin/blank.bin

flash_boot:
	$(ESP_TOOL) -c ESP32 -p $(ESP_PORT) -b $(ESP_BAUD) write_flash -fs $(ESP_FS) -fm $(ESP_FM) -ff $(ESP_FF)	\
	0x0 $(SDK_PATH)/bin/boot.bin

flash_app:
	$(ESP_TOOL) -c ESP32 -p $(ESP_PORT) -b $(ESP_BAUD) write_flash	\
	0x04000 $(BIN_PATH)/drom0.bin 0x40000 $(BIN_PATH)/irom0_flash.bin

flash_blank:
	$(ESP_TOOL) -c ESP32 -p $(ESP_PORT) -b $(ESP_BAUD) write_flash	\
	$(ESP_SYSPARAM_ADDR) $(SDK_PATH)/bin/blank.bin
