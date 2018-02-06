#include <linux/kernel.h>

extern int video_x;
extern int video_y;

void main()
{
    video_init();
    printk("Hello World\n");
    printk(" :) \n Welcome to my Operating System!(NO, lol)\n");
    while(1);
}
