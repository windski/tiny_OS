#define outb(value, port)   \
    __asm__("outb %%al, %%dx"::"a" (value), "d"(port))


#define inb(port) ({                    \
        unsigned char _v;               \
        __asm__ volatile(               \
                "inb %%dx, %%al"        \
                :"=a"(_v)               \
                :"d"(port)              \
                );                      \
                _v;                     \
    })


// 带有延时的outb, inb
#define outb_p(value, port)             \
    __asm__ volatile (                  \
            "outb %%al, %%dx\n\t"       \
            "jmp 1f\n\t"                \
            "1: jmp 1f\n\t"             \
            "1:"                        \
            :: "a" (value), "d" (port)  \
    )

#define inb_p(port) ({                  \
        unsigned char _v;               \
        __asm__ volatile(               \
                "inb %%dx, %%al\n\t"    \
                "jmp 1f\n\t"            \
                "1: jmp 1f\n\t"         \
                "1:"                    \
                : "=a" (_v)             \
                : "d" (port)            \
                );                      \
                _v;                     \
    })

