##################################################
# Makefile
##################################################

BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel.asm
BOOT_BIN:=build/boot.bin
LDR_BIN:=build/loader.bin
KERNEL_BIN:=build/kernel.bin
INCLUDE_PATH:=include/
BOOT:=boot/
LIB:=lib/
IMG:=a.img
FLOPPY:=/mnt/floppy/
GCCFLG:=-m32 -c -I $(INCLUDE_PATH) -fno-stack-protector -fno-builtin
ASMFLG:= -f elf -I $(INCLUDE_PATH)
LDFLG:= -m elf_i386 -s -Ttext 0x30400
OBJS:=build/kernel.o build/start.o build/global.o build/kliba.o \
	build/string.o build/klib.o build/i8259.o build/protect.o \
	build/main.o build/clock.o build/syscall.o build/process.o \
	build/keyboard.o build/tty.o build/console.o build/printf.o build/vsprintf.o \
	build/systask.o build/misc.o
.PHONY : everything

# everything : $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
# 	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
# 	sudo mount -o loop $(IMG) $(FLOPPY)
# 	sudo cp $(LDR_BIN) $(FLOPPY) -v
# 	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
# 	sudo umount $(FLOPPY)
everything:a.img

clean :
	rm -f build/*
makeimage:
	bximage -mode=create -fd=1.44M -q $(IMG)
run :
	bochs
a.img:$(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LDR_BIN) $(FLOPPY) -v
	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
	sudo umount $(FLOPPY)
	
build/boot.bin : boot/boot.asm $(INCLUDE_PATH)/*
	nasm -I $(BOOT) $< -o $@

build/loader.bin : boot/loader.asm $(INCLUDE_PATH)/*
	nasm -I $(BOOT) $< -o $@

build/kernel.bin : $(OBJS) $(INCLUDE_PATH)/*
	ld $(LDFLG) -o $@ $(OBJS)

build/start.o:kernel/start.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/kliba.o:lib/kliba.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

build/string.o:lib/string.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

build/syscall.o:kernel/syscall.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

build/kernel.o:kernel/kernel.asm $(INCLUDE_PATH)/*
	nasm $(ASMFLG) -o $@ $<

build/i8259.o:kernel/i8259.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/global.o:kernel/global.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/protect.o:kernel/protect.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/klib.o:lib/klib.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/main.o:kernel/main.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/clock.o:kernel/clock.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/process.o:kernel/process.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/keyboard.o:kernel/keyboard.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/tty.o:kernel/tty.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/console.o:kernel/console.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/printf.o:kernel/printf.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/vsprintf.o:kernel/vsprintf.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/misc.o:lib/misc.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<

build/systask.o:kernel/systask.c $(INCLUDE_PATH)/*
	gcc $(GCCFLG) -o $@ $<