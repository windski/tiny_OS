#define sti() __asm__("sti"::)
#define cli() __asm__("cli"::)
#define nop() __asm__("nop"::)
#define iret() __asm__("iert"::)

#define _set_gate(gate_addr, type, dpl, addr)                       \
    __asm__("movw %%dx, %%ax\n\t"                                   \
            "movw %0, %%dx\n\t"                                     \
            "movl %%eax, %1\n\t"                                    \
            "movl %%edx, %2\n\t"                                    \
            :                                                       \
            :"i" ((short)(0x8000 + ((dpl) << 13) + ((type) << 8))), \
            "o" (*((char *) (gate_addr))),                          \
            "o" (*(4 + (char *)(gate_addr))),                       \
            "a" (0x00080000),                                       \
            "d" ((char *)(addr)))


// 陷阱门, type = 14
#define set_trap_gate(n, addr) _set_gate(&_idt[n], 0xF, 0, addr)

// 中断门, type = 15
#define set_intr_gate(n, addr) _set_gate(&_idt[n], 0xE, 0, addr)

#define set_system_gate(n, addr) _set_gate(&_idt[n], 0xF, 3, addr)

