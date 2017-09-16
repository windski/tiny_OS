#######################################################
# Assembly code git@github.com:VOID001/neu-os.git     #
# This is AUTHOR....                                  #
#######################################################

# memory layout:
# 0x9000:0000 bootsect
# 0x9000:0200 setup
# 0x1000:0000 system


.code16

.global _bootstart           # start from
.global begtext, begdata, begbss, endtext, enddata, endbss

.text
begtext:
.data
begdata:
.bss
begbss:

.text

.equ SETUPLEN, 0x04
.equ SYSSIZE, 0x3000          # System size in clicks(1 click = 16 bits)
.equ BOOTSEG, 0x07c0
.equ INITSEG, 0x9000          # Move the bootsect.s here
.equ DEMO_system, 0x1000      # Toys System will load...here(real mode)
.equ SETUPSEG, 0x9020         # setup.s will move here

.equ ROOT_DEV, 0x301          # 钦点/dev/fda为系统镜像所在的设备
.equ ENDSEG, DEMO_system + SYSSIZE

ljmp $BOOTSEG, $_bootstart

_bootstart:
	movw $BOOTSEG, %ax
	movw %ax, %ds

# 将启动扇区从0x07c0:0000 移动到0x9000:0000
# rep mov usage:
# source        `ds:si = 0x07c0:0000`
# destination   `es:di = 0x9000:0000`
# moves         `%cx = 256`
# 因为是movsw所以每次移动一个Word(2Byte) 256 * 2 = 512 Byte	即为启动扇区的大小

	movw $INITSEG, %ax;
	movw %ax, %es
	movw $256, %cx
	xor %si, %si               # clear..
	xor %di, %di
	rep movsw                  # move..

	ljmp $INITSEG, $go

go:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0xff00, %sp          # setup stack..

_load__image:
	movw $0x0000, %dx
	movw $0x0002, %cx
	movw $INITSEG, %ax
	movw %ax, %es
	movw $0x0200, %bx
	movb $0x02, %ah
	movb $4, %al
	int $0x13
	jnc _load__ok              # no error.. move on..
	movw $0x0000, %dx
	movw $0x0000, %ax          # BIOS interupt, Service 0: Reset Disk
	int $0x13
	jmp _load__image

_load__ok:
	movb $0x00, %dl
	movw $0x0800, %ax
	int $0x13
	movb $0x00, %ch
	movw %cx, %cs:sectors+0
	movw $INITSEG, %ax
	movw %ax, %es

_print_msg:
	movb $0x03, %ah
	xor %bh, %bh
	int $0x10

	movw $str_len, %cx         # set the output length
	movw $0x0007, %bx          # page 0, color = 0x07 随便选一个颜色...
	movw $string, %bp
	movw 0x1301, %ax
	int $0x10

	movw $DEMO_system, %ax
	movw %ax, %es
	call read_it
	call kill_motor

	movw %cs:root_dev, %ax
	cmp $0, %ax
	jne root_defined           # ROOT_DEV != 0, Define root
	movw %cs:sectors+0, %bx    # else check for the root dev
	movw $0x0208, %ax
	cmp $15, %bx
	je root_defined
	movw $0x021c, %ax
	cmp $18, %bx
	je root_defined

undef_root:                    # if not root found, loop it..
	jmp undef_root

root_defined:
	movw %ax, %cs:root_dev+0

# everything loaded into memory, we should jump to setup   

	ljmp $SETUPSEG, $0

sread:
	.word 1 + SETUPLEN
head:
	.word 0
track:
	.word 0

read_it:
	movw %es, %ax
	test $0x0fff, %ax
go_die_unh:                    # 雾
	jne go_die_unh             # If es is not at 64KB(0x1000) Boundary, then stop here
	xor %bx, %bx
rp_read:
	movw %es, %ax
	cmp $ENDSEG, %ax
	jb ok1_read                # if $ENDSEG > %es, then continue reading, else just return
	ret

ok1_read:
	movw %cs:sectors+0, %ax
	sub sread, %ax
	movw %ax, %cx              # calc. how much sectors left to read
	shl $9, %cx
	add %bx, %cx               # current bytes read in now
	jnc ok2_read
	je ok2_read
	xor %ax, %ax
	sub %bx, %ax
	shr $9, %ax

ok2_read:
	call read_track
	movw %ax, %cx
	add sread, %ax

	cmp %cs:sectors+0, %ax
	jne ok3_read
	movw $1, %ax
	sub head, %ax
	jne ok4_read
	incw track

ok4_read:
	movw %ax, head
	xor %ax, %ax

ok3_read:
	movw %ax, sread
	shl $9, %cx
	add %cx, %bx
	jnc rp_read
	movw %es, %ax
	add $0x1000, %ax
	movw %ax, %es
	xor %bx, %bx
	jmp rp_read


read_track:
	pushw %ax
	pushw %bx
	pushw %cx
	pushw %dx
	movw track, %dx
	movw sread, %cx
	inc %cx
	movb %dl, %ch
	movw head, %dx
	movb %dl, %dh
	movb $0, %dl
	and $0x0100, %dx
	movb $2, %ah
	int $0x13
	jc bad_rt
	popw %dx
	popw %cx
	popw %bx
	popw %ax
	ret

bad_rt:
	movw $0, %ax
	movw $0, %dx
	int $0x13
	popw %dx
	popw %cx
	popw %bx
	popw %ax
	jmp read_track

kill_motor:
	push %dx
	movw $0x3f2, %dx
	movb $0, %al
	outsb
	popw %dx
	ret

sectors:
	.word 0

str_len:
	.word 17

string:
	.byte 13, 10
	.ascii "Hello World"
	.byte 13, 10, 13, 10

.= 508

root_dev:
	.word ROOT_DEV
boot_flag:
	.word 0xaa55

.text
	endtext:
.data
	enddata:
.bss
	endbss:

