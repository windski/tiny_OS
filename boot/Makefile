include ../Makefile.header

LDFLAGS += -Ttext 0

all: bootsect setup system.o

.PHONY=clean run_qemu all

# image: bootsect setup system
	# -@dd if=bootsect of=system_image bs=512 count=1
	# -@dd if=setup of=system_image bs=512 count=4 seek=1
	# -@dd if=system of=system_image bs=512 seek=5
	# @ echo "\nImage write Done!!\n"

# run_qemu: image
	# - @ qemu-system-i386 -boot a -fda system_image

bootsect: bootsect.s
	$(AS) -n bootsect.s -o bootsect.o
	$(LD) $(LDFLAGS) -o bootsect bootsect.o
	@ cp -f bootsect bootsect.debug
	$(OBJCOPY) -R .pdr -R .comment -R.note -S -O binary bootsect

setup: setup.s
	$(AS) -n setup.s -o setup.o
	$(LD) $(LDFLAGS) -o setup setup.o
	@ cp -f setup setup.debug
	$(OBJCOPY) -R .pdr -R .comment -R.note -S -O binary setup

system.o: system.s
	$(AS) -n system.s -o system.o
	@# $(LD) $(LDFLAGS) -o system system.o
	@# cp -f system system.debug
	@# $(OBJCOPY) -R .pdr -R .comment -R.note -S -O binary system
	
clean:
	- @ rm ./*.o ./*.debug bootsect setup
