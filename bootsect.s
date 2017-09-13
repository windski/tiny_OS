.code16
.equ BOOTSEG, 0x07c0
.equ INITSEG, 0x9000
.equ DEMO, 0x1000

.global _bootstart

.text
ljmp $BOOTSEG, $_bootstart

_bootstart:
	movb $0x03, %ah
	int $0x10

	movw $BOOTSEG, %ax
	movw %ax, %es
	movw $string, %bp
	movw $0x1301, %ax
	movw $0x0007, %bx
	movw $13, %cx
	int $0x10

_load__image:
	movw $0x0000, %dx
	movw $0x0002, %cx
	movw $DEMO, %ax
	movw %ax, %es
	movw $0x0200, %bx
	movb $0x02, %ah
	movb $4, %al
	int $0x13
	jnc _load__ok
	jmp _load__image

_load__ok:
	movw $DEMO, %ax
	movw %ax, %ds

	ljmp $0x1020, $0


string:
	.ascii "Hello World"
	.byte 13, 10

.= 510

boot_flag:
.word 0xaa55
