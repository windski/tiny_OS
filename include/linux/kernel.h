#ifndef _KERNEL_H_
#define _KERNEL_H_

void printk(char *format, ...);
int video_getx(void);
int video_gety(void);
void video_init(void);
void video_clear(void);
void update_cursor(int row, int col);
void vga_putchar_at(char ch, int x, int y, int attr);
void vga_putchar(char ch);
void roll_screen(void);
void k_memcpy(char *dest, char *src, int count, int size);
void k_print_num(int num, int base, int sign);

int video_x, video_y;


#endif /* _KERNEL_H_ */
