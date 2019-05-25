# 时钟中断实现

.global timer_interrupt, sys_fork, system_call

# 堆栈中各个寄存器存在的偏移位置
EAX     = 0x00
EBX     = 0x04
ECX     = 0x08
EDX     = 0x0C
FS      = 0x10
ES      = 0x14
DS      = 0x18
EIP     = 0x1C
CS      = 0x20
EFLAGS  = 0x24
OLDESP  = 0x28
OLDSS   = 0x2C


# task_struct中的变量的偏移量
state = 0
counter = 4


nr_system_call = 72

.align 2
bad_system_call:
    movl $-1, %eax
    iret

.align 2
reschedule:
    pushl $ret_from_sys_call
    jmp schedule


# 0x80 中断入口, Linux的系统调用入口点
system_call:
    cmpl $nr_system_call - 1, %eax
    ja bad_system_call
    push %ds
    push %es
    push %fs
    pushl %edx
    pushl %ecx
    pushl %ebx
    movl $0x10, %edx
# `ds', `es'指向内核数据段
    mov %dx, %ds
    mov %dx, %es
    movl $0x17, %edx
# 使fs指向局部数据段
    mov %dx, %fs
    call *system_call_table(, %eax, 4)
# 将返回值压栈
    pushl %eax
    movl $current, %eax
    cmpl $0, state(%eax)
    jne reschedule
    cmpl $0, counter(%eax)
    je reschedule
# 返回后出栈处理
ret_from_sys_call:
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    pop %fs
    pop %es
    pop %ds
    iret



# 这里将`jiffies' 每10毫秒加1, 然后发送结束中断给8259A, 然后使用当前的特权级别调用c 函数 do_timer()
timer_interrupt:
    push %ds
    push %es
    push %fs
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax
    movl $0x10, %eax
    mov %ax, %ds
    mov %ax, %es
    # movl $0x17, %eax
    # mov %ax, %fs
    incl jiffies

    movb $0x20, %al
    outb %al, $0x20

    movl CS(%esp), %eax
    andl $3, %eax
    pushl %eax

    call do_timer
    addl $4, %esp

    movl $25, %eax

    jmp ret_from_sys_call

.align 2


sys_fork:
# find_empty_process 将在fork.c 中实现
    call find_empty_process
    testl %eax, %eax
    js 1f
    push %fs
    pushl %esi
    pushl %edi
    pushl %ebp
    pushl %eax
    call copy_process
    addl $20, %esp
1:
    ret



