##################################################
# Makefile
##################################################

BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel.asm
BOOT_BIN:=boot.bin
LDR_BIN:=loader.bin
KERNEL_BIN:=kernel.bin
INCLUDE_PATH:=include/
BOOT:=boot/
LIB:=lib/
IMG:=a.img
FLOPPY:=/mnt/floppy/
GCCFLG:=-m32 -c -I $(INCLUDE_PATH) -fno-stack-protector -fno-builtin
ASMFLG:= -f elf -I $(INCLUDE_PATH)
LDFLG:= -m elf_i386 -s -Ttext 0x30400
OBJS:=kernel.o start.o global.o kliba.o string.o klib.o \
	i8259.o protect.o main.o clock.o syscall.o process.o keyboard.o tty.o console.o \
	printf.o vsprintf.o misc.o
.PHONY : everything

everything : $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LDR_BIN) $(FLOPPY) -v
	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
	sudo umount $(FLOPPY)

clean :
	rm -f *.bin *.o

run :
	bochs

boot.bin : boot/boot.asm $(INCLUDE_PATH)/*
	nasm -I $(BOOT) $< -o $@

loader.bin : boot/loader.asm $(INCLUDE_PATH)/*
	nasm -I $(BOOT) $< -o $@

kernel.bin : $(OBJS) $(INCLUDE_PATH)/*
	ld $(LDFLG) -o $@ $(OBJS)

start.o:kernel/start.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o start.o kernel/start.c

kliba.o:lib/kliba.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

string.o:lib/string.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

syscall.o:kernel/syscall.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

kernel.o:kernel/kernel.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o kernel.o $<

i8259.o:kernel/i8259.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

global.o:kernel/global.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

protect.o:kernel/protect.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

klib.o:lib/klib.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

main.o:kernel/main.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

clock.o:kernel/clock.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

process.o:kernel/process.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

keyboard.o:kernel/keyboard.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

tty.o:kernel/tty.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

console.o:kernel/console.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

printf.o:kernel/printf.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

vsprintf.o:kernel/vsprintf.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

misc.o:lib/misc.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<