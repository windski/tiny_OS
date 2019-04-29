#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/system.h>
#include <asm/io.h>
#include <linux/sched.h>

void init_timer()
{
    int division = 1193180 / HZ;
}

