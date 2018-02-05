#ifndef _KERNEL_H_
#define _KERNEL_H_

void printk(char *format, ...);
int video_getx();
int video_gety();
void video_init();
void video_clear();
void update_cursor(int row, int col);
void vga_putchar_at(char ch, int x, int y, int attr);

int video_x, video_y;


#endif /* _KERNEL_H_ */
