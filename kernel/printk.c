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

struct {
    long *a;
    short b;
} start_stack = { &user_stack[PAGE_SIZE >> 2], 0x10 };

extern int video_x;
extern int video_y;

char *video_buffer = VIDEO_MEM;

struct video_info {
    unsigned int retval;
    unsigned int colormode;
    unsigned int feature;
};

int video_getx()
{
    return video_x;
}

int video_gety()
{
    return video_y;
}

void video_init()
{
    struct video_info *info = 0x9000;

    video_x = 0;
    video_y = 0;
    video_clear();
    update_cursor(video_y, video_x);
}

void video_clear()
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
    if(x >= 80)
        x = 80;
    if(y >= 25)
        y = 25;

    // Previous code : (video_buffer + 2 * x + 80 * y) (suck)
    *(video_buffer + 2 * (x + 80 * y)) = ch;
    *(video_buffer + 2 * (x + 80 * y) + 1) = attr;
    return ;
}


void update_cursor(int row, int col)
{
    unsigned int pos = (row * VIDEO_X) + col;

    // LOW Cursor port to VGA Index Register
    outb(0x3d4, 0x0f);
    outb(0x3d5, (unsigned char)(pos & 0xff));
    // High .....
    outb(0x3d4, 0x0e);
    outb(0x3d5, (unsigned char)((pos >> 8) & 0xff));
}

