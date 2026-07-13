# Makefile for PS3 VKCube Port using Tiny3D/PSL1GHT

# Define project target and objects first, so they are available when ppu_rules is included
TARGET		:=	notvkcube
TITLE		:=	NOTVKCUBE
APPID		:=	NOTVKCUBE
CONTENTID	:=	UP0001-NOTVKCUBE_00-0000000000000000

OFILES		:=	main.o
BUILDDIR	?=	build

ifeq ($(strip $(PS3DEV)),)
  ifeq ($(strip $(DEVKITPS3)),)
    export PS3DEV := /home/cruslan/Applications/Build/PS3-VKCube-PPE/ps3dev
  else
    export PS3DEV := $(DEVKITPS3)
  endif
endif

export PSL1GHT := $(PS3DEV)

# Override GNU Make's default LD (which is 'ld') so that the PS3 compiler is used for linking.
export LD := ppu-gcc

# Compiler and Linker flags
CFLAGS		+= -g -O2 -Wall -I$(PORTLIBS)/include $(LIBPSL1GHT_INC)
LDFLAGS		+= $(LIBPSL1GHT_LIB) -L$(PORTLIBS)/lib
# Link core libraries and add librt for real-time POSIX extensions (e.g. clock/timer functions)
LIBS		+= -ltiny3d -lgcm_sys -lnet -lio -lsysutil -lm -lrt

# Include SDK ppu_rules only when not executing the clean target.
# This prevents make from failing if the SDK directory is missing during clean.
ifeq ($(filter clean,$(MAKECMDGOALS)),)
include $(PSL1GHT)/ppu_rules
endif

all: $(TARGET).elf $(TARGET).self $(TARGET).pkg

# Explicitly override rules to ensure the sprxlinker tool is run on the elf
# in a transparent and reproducible way before converting it to SELF/PKG formats.
$(TARGET).elf: $(OFILES)
	@echo "Linking ELF: $@"
	$(LD) $(OFILES) $(LDFLAGS) $(LIBS) -o $@

$(TARGET).self: $(TARGET).elf
	@echo "--- Running sprx_linker stage on $< ---"
	@mkdir -p $(BUILDDIR)
	$(STRIP) $< -o $(BUILDDIR)/$(TARGET).elf
	sprxlinker $(BUILDDIR)/$(TARGET).elf
	make_self $(BUILDDIR)/$(TARGET).elf $@
	fself $(BUILDDIR)/$(TARGET).elf $(TARGET).fake.self

$(TARGET).pkg: $(TARGET).self
	@echo "--- Packaging and compiling EBOOT.BIN with sprx_linker processed ELF ---"
	@mkdir -p $(BUILDDIR)/pkg/USRDIR
	@cp $(ICON0) $(BUILDDIR)/pkg/ICON0.PNG
	make_self_npdrm $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/pkg/USRDIR/EBOOT.BIN $(CONTENTID)
	sfo --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(BUILDDIR)/pkg/PARAM.SFO
	pkg --contentid $(CONTENTID) $(BUILDDIR)/pkg/ $@
	cp $@ $(basename $@).gnpdrm.pkg
	package_finalize $(basename $@).gnpdrm.pkg

clean:
	@echo cleaning ...
	@rm -f $(OFILES) $(TARGET).elf $(TARGET).self $(TARGET).pkg $(TARGET).fake.self $(TARGET).gnpdrm.pkg
	@rm -rf $(BUILDDIR)

.PHONY: all clean
