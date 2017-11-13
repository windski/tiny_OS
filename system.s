.code32

.text

_start:
# set up data segment register
    movl $0x10, %eax
    movw %ax, %es
    movw %ax, %ds
    movw %ax, %fs
    movw %ax, %gs
# TODO: setup temp kernel stack
    

setup_idt:
    
setup_gdt:


loop:
	jmp loop
