.code32

.global divide_error, debug, int3, overflow, bounds, invalid_op, coprocessor_segment_overrun
.global irq13, double_fault, invalid_TSS, nmi, reserved, stack_segment, segment_not_present
.global general_protection, coprocessor_error

# flag here..
.global coprocessor_error, parallel_interrupt, device_not_available


divide_error:
    pushl $do_divide_error
no_error_code:
    xchgl %eax, (%esp)
    
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %edi
    pushl %esi
    pushl %ebp
    pushw %ds
    pushw %es
    pushw %fs
    pushl $0                # error code = 0
    lea 44(%esp), %edx

    pushl %edx
    movl $0x10, %edx
    movw %dx, %ds
    movw %dx, %es
    movw %dx, %fs
    call *%eax
    addl $8, %esp           # 丢弃最后入栈的两个参数, 让栈指针回到 %fs 中
    popw %fs
    popw %es
    popw %ds
    popl %ebp
    popl %esi
    popl %edi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    iret                    # interrupt return

debug:
    pushl $do_int3
    jmp no_error_code

nmi:
    pushl $do_nmi
    jmp no_error_code

int3:
    pushl $do_int3
    jmp no_error_code

overflow:
    pushl $do_overflow
    jmp no_error_code

bounds:
    pushl $do_bounds
    jmp no_error_code

invalid_op:
    pushl $do_invalid_op
    jmp no_error_code

coprocessor_segment_overrun:
    pushl $do_coprocessor_segment_overrun
    jmp no_error_code

reserved:
    pushl $do_reserved
    jmp no_error_code

# int45 -- irq13
irq13:
    pushl %eax
    xorb %al, %al
    outb %al, $0xf0
    movb $0x20, %al
    outb %al, $0x20
    jmp 1f
1:
    jmp 1f
1:
    outb %al, $0xa0        # 向8259A 发送 EOI 
    popl %eax
    jmp coprocessor_error


double_fault:
    pushl $do_double_fault
error_code:
    xchgl %eax, 4(%esp)
    xchgl %ebx, (%esp)
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    pushl %ebp
    pushw %ds
    pushw %fs
    pushw %es
    pushl %eax
    lea 44(%esp), %eax
    pushl %eax
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    call *%ebx
    addl $8, %esp
    popw %es
    popw %fs
    popw %ds
    popl %ebp
    popl %edi
    popl %esi
    popl %edx
    popl %ebx
    popl %eax
    iret

invalid_TSS:
    pushl $do_invalid_TSS
    jmp error_code

segment_not_present:
    pushl $do_segment_not_present
    jmp error_code

stack_segment:
    pushl $do_stack_segment
    jmp error_code

general_protection:
    pushl $do_general_protection
    jmp error_code

# coprocessor error 
# TODO: 以后会有的,*(FLAG~)
coprocessor_error:
    pushl $do_stub
    jmp error_code

parallel_interrupt:
    pushl $do_stub
    jmp error_code

device_not_available:
    pushl $do_stub
    jmp error_code

