#
# Kurzanleitung
# =============
#
# make		-- Baut den Kernel.
# make all
#
# make install	-- Baut den Kernel und transferiert ihn auf den Server.
#		   object dump wird nach obj.dump geschrieben
# 		   Das Board holt sich diesen Kernel beim nächsten Reset.
#
#make run       -- make install
#                  start von veryminicon
#
# make clean	-- Löscht alle erzeugten Dateien.
#
BIN = bin
DRIVER = driver
SYSTEM = system
LIB = lib
USR = user
#
# Quellen
#
LSCRIPT = kernel.lds
OBJ = $(SYSTEM)/start.o $(SYSTEM)/stacks_asm.o $(SYSTEM)/initKernel.o $(SYSTEM)/interrupt_handler.o $(SYSTEM)/interrupt_handler_asm.o $(SYSTEM)/thread.o
OBJ += $(DRIVER)/dbgu.o $(DRIVER)/aic.o $(DRIVER)/mem_ctrl.o $(DRIVER)/sys_timer.o $(DRIVER)/led.o $(DRIVER)/pmc.o $(DRIVER)/mmu_asm.o
OBJ += $(LIB)/printf.o $(LIB)/systemtests.o $(LIB)/regcheck.o $(LIB)/regcheck_asm.o $(LIB)/utils.o $(LIB)/shell.o $(LIB)/reg_operations_asm.o $(LIB)/buffer.o $(LIB)/list.o
OBJ += $(USR)/syscall.o $(USR)/syscall_asm.o $(USR)/application.o

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
	
kernel.bin.gz: kernel.bin
	gzip $< $@

kernel.img: kernel.bin.gz
	mkimage -A arm -T standalone -C gzip -a 0x20000000 -d $< $@

.PHONY: install
install: kernel.img
	rm -f kernel.bin.gz
	arm-install-image $<
	arm-none-eabi-objdump -d kernel > obj.dump
	
.PHONY: run
run: kernel.img
	rm -f kernel.bin.gz
	arm-install-image $<
	arm-none-eabi-objdump -d kernel > obj.dump
	veryminicom

.PHONY: clean
clean:
	rm -f kernel kernel.bin kernel.img obj.dump
	rm -f $(OBJ)
	rm -f $(DEP)
