.code16

.equ DEMOSEG, 0x1020
.equ STR_LEN, 36

.text
show:
	movw $DEMOSEG, %ax
	movw %ax, %es
	movb $0x03, %ah
	xor %bh, %bh
	int $0x10

	movw $0x000a, %bx
	movw $0x1301, %ax
	movw $STR_LEN, %cx
	movw $msg, %bp
	int $0x10

loop__:
	jmp loop__

msg:
	.byte 13, 10
	.ascii "Welcome to my OPERATING SYSTEM!"
	.byte 13, 10, 13, 10

