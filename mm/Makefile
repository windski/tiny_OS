include ../Makefile.header

.PHONY = clean all

LDFLAGS += -r
CFLAGS += -I../include

OBJS = memory.o

.c.o:
	@ $(CC) $(CFLAGS) -c $< -o $*.o

.s.o:
	@ $(AS) $< -o $*.o

.c.s:
	@ $(CC) $(CFLAGS) -S $< -o $*.s

mm.o: $(OBJS)
	@ $(LD) $(LDFLAGS) $(OBJS) -o mm.o

clean:
	@ rm -f *.o

