BUILD_DIR=../build
IMG_DIR=..
TOOLS_DIR=../tools

CC=../tools/RetroFlex-BIOS-cross/bin/i386-elf-gcc
AS=nasm
LD=../tools/RetroFlex-BIOS-cross/bin/i386-elf-ld

CFLAGS=-m32 -march=i386 -mno-sse -ffreestanding -fno-stack-protector -fno-pic -fno-pie -nostdlib -Wall -Wextra -Werror -Wpedantic -Ofast -g
ASFLAGS=-f elf32 -g
LDFLAGS=-T bios.ld
LDFLAGS_DEBUG=-T bios.elf.ld

EMU_FLAGS=-debugcon stdio -bios $(BUILD_DIR)/bios.bin
EMU_DEBUG_FLAGS=-debugcon stdio -S -gdb tcp::1234 -bios $(BUILD_DIR)/bios.bin
EMU_DEBUG_KVM_FLAGS=-enable-kvm -cpu host -debugcon stdio -S -gdb tcp::1234 -bios $(BUILD_DIR)/bios.bin
EMU_KVM_FLAGS=-enable-kvm -cpu host -debugcon stdio -bios $(BUILD_DIR)/bios.bin

EMU=qemu-system-i386

bios:
	# 16-bit Real Mode Assembly code
	$(AS) $(ASFLAGS) bootstrap/bootstrap.asm -o $(BUILD_DIR)/bootstrap.o
	$(AS) $(ASFLAGS) bios.asm -o $(BUILD_DIR)/bios.o
	$(AS) $(ASFLAGS) ivt/ivt.asm -o $(BUILD_DIR)/ivt.o
	$(AS) $(ASFLAGS) vga/vga.asm -o $(BUILD_DIR)/vga.o

	# 32-bit Protected Mode/16-bit Real Mode Assembly code
	$(AS) $(ASFLAGS) pci/pci.asm -o $(BUILD_DIR)/pciasm.o
	$(AS) $(ASFLAGS) mainc.asm -o $(BUILD_DIR)/maincasm.o

	# 32-bit Protected Mode C code
	$(CC) $(CFLAGS) -c mainc.c -o $(BUILD_DIR)/mainc.o
	$(CC) $(CFLAGS) -c headers/util.c -o $(BUILD_DIR)/util.o
	$(CC) $(CFLAGS) -c headers/stdio.c -o $(BUILD_DIR)/stdio.o
	$(CC) $(CFLAGS) -c pci/pci.c -o $(BUILD_DIR)/pcic.o
	
	# Linking everything
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/bios.bin $(BUILD_DIR)/*.o # Link BIOS ROM
	$(LD) $(LDFLAGS_DEBUG) -o $(BUILD_DIR)/bios.elf $(BUILD_DIR)/*.o # Link debug ELF file

qemu:
	$(EMU) $(EMU_FLAGS) -m 8

qemu_debug:
	$(EMU) $(EMU_DEBUG_FLAGS) -m 8 # After QEMU runs, run GDB in a different terminal and put in "target remote :1234" then put in "file build/bios.elf" and then start debugging

qemu_debug_kvm:
	$(EMU) $(EMU_DEBUG_KVM_FLAGS) -m 8 # Might crash because of KVM issues with the BIOS. After QEMU runs, run GDB in a different terminal and put in "target remote :1234" then start debugging

qemu_kvm:
	$(EMU) $(EMU_KVM_FLAGS) -m 8 # Might crash because of KVM issues with the BIOS

cross:
	chmod +x $(TOOLS_DIR)/cross/build-cross.sh
	$(TOOLS_DIR)/cross/build-cross.sh

clean:
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
