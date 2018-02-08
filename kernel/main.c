#include <linux/kernel.h>
#include <asm/system.h>

extern int video_x;
extern int video_y;


void main()
{
    int ret;
    video_init();
    printk("Hello World\n");
    printk(" :) \nWelcome to my Operating System!(NO, lol)\n");

    printk("[ Memory Testing.. ]\n\n");
    mem_init(0x100000, 0x1000000);
    ret = mmtest_main();

    while(1);
}
