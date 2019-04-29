#include <linux/kernel.h>
#include <asm/system.h>

extern int video_x;
extern int video_y;

extern void mem_init(unsigned long, unsigned long);
extern void mmtest_main(void);

void main()
{
    video_init();
    printk("Hello World\n");
    printk(" :) \nWelcome to my Operating System!(NO, lol)\n");

    mem_init(0x100000, 0x1000000);
    mmtest_main();

    while(1);
}
