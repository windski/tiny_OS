all: bootsect.o

.PHONY=clean run_qemu

run_qemu:
	@qemu-system-i386 -boot a -fda bootsect

bootsect.o:
	as --32 bootsect.s -o bootsect.o

link: bootsect.o ld-bootsect.ld
	- ld -T ld-bootsect.ld bootsect.o -o bootsect
	- objcopy -O binary bootsect

clean:
	- rm ./*.o bootsect
