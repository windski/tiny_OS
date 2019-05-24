#include <linux/kernel.h>
#include <asm/system.h>

extern int video_x;
extern int video_y;

extern void mem_init(unsigned long, unsigned long);
extern void mmtest_main(void);
extern void print_mminfo(unsigned long);

void main()
{
    video_init();
    printk("Hello World\n");
    printk(" :) \nWelcome to my Operating System!(NO, lol)\n");

    /* sched_init(); */
    trap_init();
    init_timer();

    mem_init(0x100000, 0x3000000);
    /* mmtest_main(); */

    sti();

    while(1);
}
