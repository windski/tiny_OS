.code16

.text

.equ SETUPSEG, 0x9020
.equ STR_LEN, 36
.equ SYS_IMAGE, 0x1000
.equ INITSEG, 0x9000

.global _bootstart, begtext, begdata, begbss, endtext, enddata, endbss

.text
	begtext:
.data
	begdata:
.bss
	begbss:
.text

show:
	movw $SETUPSEG, %ax
	movw %ax, %es
	movb $0x03, %ah
	xor %bh, %bh
	int $0x10

	movw $0x000a, %bx
	movw $0x1301, %ax
	movw $STR_LEN, %cx
	movw $msg, %bp
	int $0x10

	ljmp $SETUPSEG, $_bootstart
_bootstart:
	movw $INITSEG, %ax
	movw %ax, %ds
	movb $0x03, %ah
	xor %bh, %bh
	int $0x10
	movw %dx, %ds:0

# get the extend momry size
	movb $0x88, %ah
	int $0x15
	movw %ax, %ds:2

	movb $0x0f, %ah
	int $0x10
	movw %bx, %ds:4
	movw %ax, %ds:6

	movb $0x12, %ah
	movb $0x10, %bl
	int $10
	movw %ax, %ds:8
	movw %bx, %ds:10
	movw %cx, %ds:12

# 复制硬盘参数信息

# 第一块硬盘参数信息
	movw $0x0000, %ax
	movw %ax, %ds
	lds %ds:4*0x41, %si
	movw $INITSEG, %ax
	movw %ax, %es
	movw $0x0000, %di
	movw $0x10, %cx
	rep movsb

# 第二块
	movw $0x0000, %ax
	movw %ax, %ds
	lds %ds:4*0x46, %si
	movw $INITSEG, %ax
	movw %ax, %es
	movw $0x0090, %di
	movw $0x10, %cx
	rep movsb

# check disk
	movw $0x1500, %ax
	movb $0x81, %dl
	int $0x13
	jc no_disk1
	cmp $3, %ah
	je is_disk1

no_disk1:
	movw $INITSEG, %ax
	movw %ax, %es
	movw $0x0090, %di
	movw $0x10, %cx
	movw $0x00, %ax
	rep stosb

is_disk1:
	cli
	
# move the system image
	movw $0x0000, %ax
	cld
do_move:
	movw %ax, %es
	add $0x1000, %ax
	cmp $0x9000, %ax
	jz end_move
	movw %ax, %ds
	xor %di, %di
	xor %si, %si
	movw $0x8000, %cx
	rep movsw
	jmp do_move

end_move:
	movw $SETUPSEG, %ax
	movw %ax, %ds
	lgdt gdt_48
	lidt idt_48

# 开启a20 地址线

# Fast A20 Gate
	# inb $0x92, %al
	# orb $0x02, %al
	# outb %al, $0x92

# read 0xee port to enable A20 line, write it to disable A20 line...
    in $0xee, %al
    xor %al, %al

# programming 8259A interrupts...
    movb $0x11, %al                # initialization sequence
    out %al, $0x20                 # send to 8259A-1
    .word 0x00eb, 0x00eb

    out %al, $0xa0                 # send to 8259A-1
    .word 0x00eb, 0x00eb

    movb $0x20, %al                # start of hardware int's(0x20)
    out %al, $0x21
    .word 0x00eb, 0x00eb

    movb $0x28, %al                # start of hardware int's(0x28)
    out %al, $0xa1
    .word 0x00eb, 0x00eb

    movb $0x04, %al                # set up 8259A-1 master
    out %al, $0x21
    .word 0x00eb, 0x00eb

    movb $0x02, %al                # setup 8259A-2 is alive
    out %al, $0xa1
    .word 0x00eb, 0x00eb

    movb $0x01, %al
    out %al, $0x21                 # sent ICW3 8086 model: genaral EOI..
    .word 0x00eb, 0x00eb

    out %al, $0xa1                 # same as above..
    .word 0x00eb, 0x00eb

    movb $0xff, %al
    out %al, $0x21                 # mask off all interrupts for now
    .word 0x00eb, 0x00eb
    out %al, $0xa1

# 开启保护模式!
	movl %cr0, %eax
	bts $0, %eax
	movl %eax, %cr0

# Jump to protected mode
	.equ sel_cs0, 0x0008
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	ljmp $sel_cs0, $0

gdt_48:
	.word 0x800
	.word 512+gdt, 0x9

gdt:
	.word 0, 0, 0, 0           # 保留

	.word 0x07ff
	.word 0x0000
	.word 0x9a00
	.word 0x00c0

	.word 0x07ff
	.word 0x0000
	.word 0x9200
	.word 0x00c0

idt_48:
    .word 0
    .word 0, 0

msg:
	.byte 13, 10
	.ascii "Welcome to my OPERATING SYSTEM!"
	.byte 13, 10, 13, 10

