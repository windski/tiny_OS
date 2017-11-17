# This program will be covered by the page table...

.code32
.text

.global _startup, _idt, _gdt, tmp_floppy_area

_pg_dir:
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
    movl $0x10, %eax
    movw %ax, %es
    movw %ax, %ds
    movw %ax, %fs
    movw %ax, %gs
    lss start_stack, %esp

# checkout A20 line
    xorl %eax, %eax
1:  incl %eax
    movl %eax, 0x000000
    cmpl %eax, 0x100000
    je 1b

# check math chip
    movl %cr0, %eax
    andl $0x80000011, %eax
    orl $2, %eax                       # set MP
    movl %eax, %cr0
    call check_x87
    jmp after_page_tables

check_x87:
    fninit
    fstsw %ax
    cmpb $0, %al
    je 1f
    movl %cr0, %eax
    xorl $6, %eax
    movl %eax, %cr0
    ret
.align 2

1:  .byte 0xdb, 0xe4                   # 287 coprocess code
    ret


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

.org 0x1000
pg0:

.org 0x2000
pg1:

.org 0x3000
pg2:

.org 0x4000
pg3:

.org 0x5000

_tmp_floppy_area:
    .fill 1024, 1, 0

after_page_tables:
    pushl $0
    pushl $0
    pushl $0
    pushl $L6
    pushl $main
    jmp setup_pages

L6:
    jmp L6

int_msg:
    .asciz "Unknown interrupt\n\r"

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
setup_pages:
    movl $1024 * 5, %ecx
    xorl %eax, %eax
    xorl %edi, %edi
    cld;rep;stosl
    movl $pg0 + 7, _pg_dir
    movl $pg1 + 7, _pg_dir + 4
    movl $pg2 + 7, _pg_dir + 8
    movl $pg3 + 7, _pg_dir + 12
    movl $pg3 + 4092, %edi
    movl $0xfff007, %eax
    std
1:  stosl
    subl $0x1000, %eax
    jge 1b
    xorl %eax, %eax
    movl %eax, %cr3
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    ret

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
