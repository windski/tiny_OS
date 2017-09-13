all: image

.PHONY=clean run_qemu, image

image: link
	- @ dd if=bootsect of=image_TINY bs=512 count=1
	- @ dd if=demo of=image_TINY bs=512 count=4 seek=1
	@ echo "\nImage write Done!!\n"

run_qemu:
	- @ qemu-system-i386 -boot a -fda image_TINY

bootsect.o: bootsect.s
	- @ as --32 bootsect.s -o bootsect.o

link: bootsect.o ld-bootsect.ld demo.o
	- @ ld -T ld-bootsect.ld bootsect.o -o bootsect
	- @ objcopy -O binary bootsect
	- @ ld -T ld-bootsect.ld demo.o -o demo
	- @ objcopy -O binary demo

demo.o: demo.s
	- @ as --32 demo.s -o demo.o

clean:
	- @ rm ./*.o bootsect image_TINY demo
