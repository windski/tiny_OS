.code16
.equ BOOTSEC, 0x07c0

.global _bootstart

.text

ljmp $BOOTSEC, $_bootstart

_bootstart:
	movb $0x03, %ah
	int $0x10

	movw $BOOTSEC, %ax
	movw %ax, %es
	movw $string, %bp
	movw $0x1301, %ax
	movw $0x0007, %bx
	movw $13, %cx
	int $0x10

loop:
	jmp loop

string:
	.ascii "Hello World"
	.byte 13, 10

.= 510

boot_flag:
.word 0xaa55
