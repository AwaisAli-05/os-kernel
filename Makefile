CC = gcc
LD = ld
ASM = nasm

all: os-image.bin

os-image.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > os-image.bin
	# FIX: Pad to 1MB so BIOS satisfies the "Read 15 sectors" request
	truncate -s 1M os-image.bin

kernel.bin: kernel.o interrupt.o
	$(LD) -m elf_i386 -T linker.ld kernel.o interrupt.o -o kernel.bin --oformat binary

interrupt.o: interrupt.asm
	$(ASM) -f elf32 interrupt.asm -o interrupt.o

kernel.o: kernel.c
	# FIX: Added -fno-stack-protector to prevent kernel crashes
	$(CC) -m32 -ffreestanding -fno-stack-protector -fno-pic -fno-pie -c kernel.c -o kernel.o

boot.bin: boot.asm
	$(ASM) -f bin boot.asm -o boot.bin

run: all
	qemu-system-i386 -drive format=raw,file=os-image.bin

clean:
	rm -f *.bin *.o os-image.bin