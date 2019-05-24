# 时钟中断实现

.global timer_interrupt

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

