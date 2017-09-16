all: image

.PHONY=clean run_qemu, image

image: link
	dd if=bootsect of=image_TINY bs=512 count=1
	dd if=setup of=image_TINY bs=512 count=4 seek=1
	dd if=system of=image_TINY bs=512 seek=5
	@ echo "\nImage write Done!!\n"

run_qemu:
	- @ qemu-system-i386 -boot a -fda image_TINY

bootsect.o: bootsect.s
	as --32 bootsect.s -o bootsect.o

setup.o: setup.s
	as --32 setup.s -o setup.o

system.o: system.s
	as --32 system.s -o system.o

link: bootsect.o ld-bootsect.ld setup.o system.o
	@ ld -T ld-bootsect.ld bootsect.o -o bootsect
	@ objcopy -O binary bootsect
	@ ld -T ld-bootsect.ld setup.o -o setup
	@ objcopy -O binary setup
	@ ld -T ld-bootsect.ld system.o -o system
	@ objcopy -O binary system

clean:
	- @ rm ./*.o bootsect setup system
