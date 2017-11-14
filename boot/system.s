# This program will be covered by the page table...

.code32
.global _startup
.text

_startup:
# set up data segment register
    movl $0x10, %eax
    movw %ax, %es
    movw %ax, %ds
    movw %ax, %fs
    movw %ax, %gs
    lss start_stack, %esp               # setup system stack
    call setup_idt
    call setup_gdt

setup_idt:
    lea ignore_int, %esp
    movl $0x0008000, %eax
    movw %dx, %ax
    movw $0x8e00, %dx
    lea _idt, %edi
    movl $256, %ecx

rp_sidt:
    movl %eax, (%edi)
    movl %edx, 4(%edi)
    addl $8, %edi
    dec %ecx
    jne rp_sidt
    lidt idt_descr                       # load IDT Register
    ret

setup_gdt:
    lgdt gdt_descr
    ret

.align 2
ignore_int:
    pushl %eax
    pushl %ecx
    pushl %edx
    pushw %ds
    pushw %es
    pushw %fs
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
#TODO: complete printk.c code...

    popw %fs
    popw %es
    popw %ds
    popl %edx
    popl %ecx
    popl %eax
    iret

.align 2
.word 0

idt_descr:
    .word 256 * 8 - 1
    .long _idt

.align 2
.word 0


gdt_descr:
    .word 256 * 8 - 1
    .long _gdt
    .align 8


loop:
	jmp loop

_idt:
    .fill 256, 8, 0

_gdt:
    .quad 0x0000000000000000             # NULL descriptor
# Base Address = 0x00000000
# Limit = 0xfff
# TYPE  = 0xA Executable Read
# Code Segment
    .quad 0x00c09a0000000fff             # 16MB
# Base Address = 0x00000000
# Limit = 0xfff
# TYPE  = 0x2 Read/Write
# Data Segment
    .quad 0x00c0920000000fff             # 16MB
    .quad 0x0000000000000000             # unuse

    .fill 252, 8, 0
