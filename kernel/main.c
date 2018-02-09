#include <linux/kernel.h>
#include <asm/system.h>

int video_x;
int video_y;


void main()
{
    video_init();
    /* printk("Hello World\n");
     * printk(" :) \nWelcome to my Operating System!(NO, lol)\n"); */

    int i;
    for(i = 0; i < 100; i++) {
        printk("Test * %d\n", i);
    }

    /* printk("[ Memory Testing.. ]\n\n");
     * mem_init(0x100000, 0x1000000);
     * ret = mmtest_main(); */

    while(1);
}
