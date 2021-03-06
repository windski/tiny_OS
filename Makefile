include Makefile.header

LDFLAGS += -Ttext 0 -e _startup -nostdlib
.PHONY: clean run_qemu
OBJS = boot/system.o kernel/kernel.o mm/mm.o
all: sys_image

boot/system.o:
	@ make system.o -C boot

boot/bootsect:
	@ make bootsect -C boot

boot/setup:
	@ make setup -C boot

kernel/kernel.o:
	@ make -C kernel

mm/mm.o:
	@make -C mm

system: $(OBJS)
	@ $(LD) $(LDFLAGS) $(OBJS) -o system.sym
	@ $(STRIP) system.sym -o system.o
	@ $(OBJCOPY) -O binary -R .note -R .comment system.o system

sys_image: boot/setup boot/bootsect system
	@ dd if=boot/bootsect of=sys_image bs=512 count=1
	@ dd if=boot/setup of=sys_image bs=512 count=4 seek=1
	@ dd if=system of=sys_image bs=512 seek=5
	@ echo "Build System Image Done!!"

run_qemu: sys_image
	qemu-system-i386 -boot a -fda sys_image

clean:
	-@ rm sys_image *.o *.sym system
	-@ make clean -C boot
	-@ make clean -C kernel
	-@ make clean -C mm
