ARCH=i386
CC=gcc
ASM=nasm
CFLAGS=-m32 -nostdlib -nostartfiles -fno-builtin -fno-exceptions -ffreestanding -O2 -Wall -Isrc
LDFLAGS=-m elf_i386 -T src/linker.ld

KERNEL_OBJS=src/start.o src/kernel.o src/drivers.o src/filesystem.o src/efh.o src/hpm.o src/network.o src/string.o
KERNEL_BIN=kernel.bin

all: iso

src/start.o: src/start.asm
	$(ASM) -f elf32 -o $@ $<

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(KERNEL_BIN): $(KERNEL_OBJS)
	ld $(LDFLAGS) -o $@ $(KERNEL_OBJS)

iso: $(KERNEL_BIN)
	mkdir -p iso/boot/grub
	cp $(KERNEL_BIN) iso/boot/kernel.bin
	printf 'set timeout=0\nmenuentry "HubbleOS" { multiboot /boot/kernel.bin }\n' > iso/boot/grub/grub.cfg
	grub-mkrescue -o hubbleos.iso iso 2>/dev/null || echo "Note: grub-mkrescue not available; ISO generation skipped"

run: iso
	qemu-system-i386 -cdrom hubbleos.iso -m 256 2>/dev/null || echo "qemu-system-i386 not available"

clean:
	rm -f src/*.o $(KERNEL_BIN)
	rm -rf iso hubbleos.iso

.PHONY: all iso run clean
