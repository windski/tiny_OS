include ../Makefile.header

.PHONY: clean all

LDFLAGS += -r
CFLAGS += -I../include

OBJS = memory.o mm_test.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

.s.o:
	$(AS) $< -o $*.o

.c.s:
	$(CC) $(CFLAGS) -S $< -o $*.s

all: mm.o

mm.o: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o mm.o

clean:
	@ rm -f *.o

