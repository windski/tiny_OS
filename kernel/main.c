#include <linux/kernel.h>
#include <asm/system.h>

extern int video_x;
extern int video_y;


void main()
{
    video_init();
    printk("Hello World\n");
    printk(" :) \nWelcome to my Operating System!(NO, lol)\n");

    while(1);
}
