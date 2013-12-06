#
# Kurzanleitung
# =============
#
# make		-- Baut den Kernel.
# make all
#
# make install	-- Baut den Kernel und transferiert ihn auf den Server.
# 		   Das Board holt sich diesen Kernel beim nächsten Reset.
#
# make clean	-- Löscht alle erzeugten Dateien.
#
BIN = bin
#
# Quellen
#
LSCRIPT = kernel.lds
OBJ = system/start.o system/stacks_asm.o system/exception_handler.o system/exception_handler_asm.o system/initKernel.o system/thread.o
OBJ += driver/dbgu.o driver/aic.o driver/mem_ctrl.o driver/sys_timer.o driver/led.o
OBJ += lib/printf.o lib/systemtests.o lib/regcheck.o lib/regcheck_asm.o lib/buffer.o lib/utils.o lib/console.o lib/scanf.o

OBJ := $(OBJ:%=$(BIN)/%)

#
# Konfiguration
#
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -Wextra -ffreestanding -mcpu=arm920t -O2
CPPFLAGS = -Iinclude
#LIBGCC := $(shell $(CC) -print-libgcc-file-name)

DEP = $(OBJ:.o=.d)

#
# Regeln
#
.PHONY: all 
all: kernel

-include $(DEP)

$(BIN)/%.o: %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

$(BIN)/%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -o $@ -c $<

kernel: $(LSCRIPT) $(OBJ)
	$(LD) -T$(LSCRIPT) -o $@ $(OBJ) $(LIBGCC)

kernel.bin: kernel
	$(OBJCOPY) -Obinary --set-section-flags .bss=contents,alloc,load,data $< $@

kernel.img: kernel.bin
	mkimage -A arm -T standalone -C none -a 0x20000000 -d $< $@

.PHONY: install
install: kernel.img
	arm-install-image $<
	arm-none-eabi-objdump -d kernel > obj.dump
	
.PHONY: run
run: kernel.img
	arm-install-image $<
	arm-none-eabi-objdump -d kernel > obj.dump
	veryminicom

.PHONY: clean
clean:
	rm -f kernel kernel.bin kernel.img obj.dump
	rm -f $(OBJ)
	rm -f $(DEP)
