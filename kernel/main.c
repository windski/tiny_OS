#include <linux/kernel.h>
#include <asm/system.h>

extern int video_x;
extern int video_y;

extern void mem_init(unsigned long, unsigned long);
extern void mmtest_main(void);
extern void trap_init(void);
extern void sched_init();

void main()
{
    video_init();
    printk("Hello World\n");
    printk(" :) \nWelcome to my Operating System!(NO, lol)\n");

    trap_init();
    sched_init();

    mem_init(0x100000, 0x3000000);
    /* mmtest_main(); */

    sti();

    while(1);
}
