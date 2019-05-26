CC = gcc
AS = as
LD = ld
OBJCOPY = objcopy
STRIP = strip

OBJS = printk.o main.o panic.o asm.o traps.o sched.o system_call.o fork.o

LDFLAGS += -r
CFLAGS += -I../include

.PHONY: clean all

all:kernel.o

kernel.o: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.o $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<


clean:
	-@ rm -f $(OBJS) kernel.o
