#include <linux/kernel.h>

void panic(const char *str)
{
    printk("Kernel panic: %s\n", str);
    for(; ; );
}

