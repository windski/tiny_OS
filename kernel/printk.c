#include <linux/kernel.h>
#include <asm/io.h>
#include <stddef.h>
#include <stdarg.h>

#define VIDEO_MEM 0xB8000
#define VIDEO_X 80
#define VIDEO_Y 25
#define CALC_MEM(x, y) (2 * ((x) + 80 * (y)))

#define PAGE_SIZE 4096

long user_stack[PAGE_SIZE >> 2];

extern int video_x;
extern int video_y;

char *video_buffer = (char *)VIDEO_MEM;

struct video_info {
    unsigned int retval;
    unsigned int colormode;
    unsigned int feature;
};

int video_getx(void)
{
    return video_x;
}

int video_gety(void)
{
    return video_y;
}

void video_init(void)
{
    struct video_info *info = (struct video_info *)0x9000;

    video_x = 0;
    video_y = 0;
    video_clear();
    update_cursor(video_y, video_x);
}

void video_clear(void)
{
    int i, j;
    video_x = 0;
    video_y = 0;
    for(i = 0; i < VIDEO_X; i++) {
        for(j = 0; j < VIDEO_Y; j++) {
            vga_putchar_at(' ', i, j, 0x0f);
        }
    }
    return ;
}

void vga_putchar_at(char ch, int x, int y, int attr)
{
    if(x >= VIDEO_X)
        x = VIDEO_X;
    if(y >= VIDEO_Y)
        y = VIDEO_Y;

    // Previous code : (video_buffer + 2 * x + 80 * y) (suck)
    *(video_buffer + 2 * (x + 80 * y)) = ch;
    *(video_buffer + 2 * (x + 80 * y) + 1) = attr;
    return ;
}


void update_cursor(int row, int col)
{
    unsigned int pos = (row * VIDEO_X) + col;

    // LOW Cursor port to VGA Index Register
    outb(0x0f, 0x3d4);
    outb((unsigned char)(pos & 0xff), 0x3d5);
    // High .....
    outb(0x0e, 0x3d4);
    outb((unsigned char)((pos >> 8) & 0xff), 0x3d5);
    return ;
}


void printk(char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    char ch, *str;

    while(*format) {
        ch = *format++;
        if(ch != '%') {
            vga_putchar(ch);
            continue;
        }

        ch = *format++;
        if(ch == '\0')
            break;
        switch(ch) {
            case 'd':
                k_print_num(va_arg(ap, int), 10, 1);
                break;
            case 'u':
                k_print_num(va_arg(ap, int), 10, 0);
                break;
            case 'x':
                k_print_num(va_arg(ap, int), 16, 0);
                break;
            case 'p':
                k_print_num(va_arg(ap, int), 16, 0);
                break;
            case 's':
                str = va_arg(ap, char*);
                while(*str) {
                    vga_putchar(*str);
                    str++;
                }
                break;
            case '%':
                vga_putchar('%');
        }
    }
}


void k_print_num(int num, int base, int sign)
{
    char digits[] = "0123456789ABCDEF";
    char buff[64] = "";
    int i, count = 0;

    if(sign && num < 0) {
        vga_putchar('-');
        num = -num;
    }

    if(num == 0) {
        vga_putchar('0');
        return ;
    }
    while(num) {
        buff[count] = digits[num % base];
        count++;
        num /= base;
    }

    for(i = count - 1; i >= 0; i--) {
        vga_putchar(buff[i]);
    }
    return ;
}


void vga_putchar(char ch)
{
    if(ch == '\n') {
        video_x = 0;
        video_y++;
    } else {
        vga_putchar_at(ch, video_x, video_y, 0x0f);
        video_x++;
    }

    if(video_x >= VIDEO_X) {
        video_x = 0;
        video_y++;
    }

    if(video_y >= VIDEO_Y) {
        roll_screen();
        video_x = 0;
        video_y = VIDEO_Y - 1;
    }

    update_cursor(video_y, video_x);
    return ;
}


void roll_screen(void)
{
    int i;
    // Copy line A + 1 to line A
    for(i = 1; i < VIDEO_Y; i++) {
        k_memcpy(video_buffer + (i - 1) * 80 * 2,
                video_buffer + i * 80 * 2,
                VIDEO_X,
                sizeof(char) * 2);
    }
    // Clear the last line
    for(i = 0; i < VIDEO_X; i++) {
        vga_putchar_at(' ', i, VIDEO_Y - 1, 0x0f);
    }
    return ;
}


void k_memcpy(char *dest, char *src, int count, int size)
{
    int i, j;
    for(i = 0; i < count; i++) {
        for(j = 0; j < size; j++) {
            *(dest + i * size + j) = *(src + i * size + j);
        }
    }
    return ;
}

