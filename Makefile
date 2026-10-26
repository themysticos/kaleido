CC      = gcc
CFLAGS  = -m32 -std=gnu11 -ffreestanding -fno-pic -fno-stack-protector \
          -fno-builtin -Wall -Wextra -O2 -Ikernel
LDFLAGS = -m32 -T boot/linker.ld -ffreestanding -nostdlib -no-pie

OBJS = boot/boot.o kernel/kmain.o kernel/vga.o kernel/market.o

# дефолтная цель — сразу ISO
all: iso

kernel.elf: $(OBJS) boot/linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

boot/boot.o: boot/boot.asm
	nasm -f elf32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: kernel.elf grub.cfg
	mkdir -p isodir/boot/grub
	cp kernel.elf isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o kaleido.iso isodir

run: iso
	qemu-system-i386 -cdrom kaleido.iso -nographic

clean:
	rm -f $(OBJS) kernel.elf kaleido.iso
	rm -rf isodir

.PHONY: all iso run clean
