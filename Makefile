# Makefile for PS3 VKCube Port using Tiny3D/PSL1GHT

export PS3DEV	?= $(CURDIR)/ps3dev
export PSL1GHT	?= $(PS3DEV)

TARGET		:= notvkcube
TITLE		:= NOTVKCUBE
APPID		:= NOTVKCUBE
CONTENTID	:= UP0001-NOTVKCUBE_00-0000000000000000

OFILES		:= main.o
BUILDDIR	?= build

SFOXML		:= sfo.xml

SDK_URL		:= https://github.com/ps3dev/ps3dev/releases/download/nightly-2026-07-26/ps3dev-linux-X64.tar.gz
SDK_TAR		:= ps3dev-linux-X64.tar.gz

-include $(PS3DEV)/ppu_rules

# Override GNU Make's default LD (which is 'ld') so that the PS3 compiler is used for linking.
export LD	:= $(PS3DEV)/ppu/bin/ppu-gcc
export CC	:= $(PS3DEV)/ppu/bin/ppu-gcc

# Compiler and Linker flags
CFLAGS		+= -mcpu=cell -O2 -Wall -I$(PS3DEV)/ppu/include -I$(PS3DEV)/portlibs/ppu/include
LDFLAGS		+= -L$(PS3DEV)/ppu/lib -L$(PS3DEV)/portlibs/ppu/lib
# Link core libraries and add librt for real-time POSIX extensions (e.g. clock/timer functions)
LIBS		+= -ltiny3d -lgcm_sys -lnet -lio -lsysutil -lm -lrt

all: pkg

$(TARGET).elf: $(OFILES)
	@echo "Linking ELF: $@"
	$(CC) $(OFILES) $(LDFLAGS) $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

pkg: $(TARGET).elf
	@echo "--- Running sprx_linker stage on $< ---"
	@mkdir -p $(BUILDDIR)/pkg/USRDIR
	$(PS3DEV)/ppu/bin/ppu-strip $(TARGET).elf -o $(BUILDDIR)/$(TARGET).elf
	$(PS3DEV)/bin/sprxlinker $(BUILDDIR)/$(TARGET).elf
	$(PS3DEV)/bin/make_self $(BUILDDIR)/$(TARGET).elf $(TARGET).self
	$(PS3DEV)/bin/fself $(BUILDDIR)/$(TARGET).elf $(TARGET).fake.self
	@echo "--- Packaging and compiling EBOOT.BIN with sprx_linker processed ELF ---"
	@cp pkg/ICON0.PNG $(BUILDDIR)/pkg/ICON0.PNG 2>/dev/null || true
	$(PS3DEV)/bin/make_self_npdrm $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/pkg/USRDIR/EBOOT.BIN $(CONTENTID)
	$(PS3DEV)/bin/sfo --fromxml $(SFOXML) $(BUILDDIR)/pkg/PARAM.SFO 2>/dev/null || cp pkg/PARAM.SFO $(BUILDDIR)/pkg/PARAM.SFO
	$(PS3DEV)/bin/pkg --contentid $(CONTENTID) $(BUILDDIR)/pkg/ $(TARGET).pkg
	cp $(TARGET).pkg $(TARGET).gnpdrm.pkg
	$(PS3DEV)/bin/package_finalize $(TARGET).gnpdrm.pkg 2>/dev/null || true

clean:
	@echo "Cleaning build artifacts and temporary files..."
	gio trash $(OFILES) $(TARGET).elf $(TARGET).self $(TARGET).pkg $(TARGET).fake.self $(TARGET).gnpdrm.pkg $(BUILDDIR) 2>/dev/null || true

prepare:
	@echo "Downloading PS3 SDK from $(SDK_URL)..."
	curl -L -o $(SDK_TAR) $(SDK_URL)
	@echo "Extracting PS3 SDK to project directory..."
	tar -xzf $(SDK_TAR) -C .
	@echo "Cleaning up archive..."
	gio trash $(SDK_TAR) 2>/dev/null || true
	@echo "PS3 SDK environment successfully set up in ./ps3dev."
	@echo "Project is ready to build. Run 'make' to compile."

.PHONY: all clean pkg prepare
