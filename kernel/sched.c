// 可编程中断定时器的设置可以参考这里 -> https://wiki.osdev.org/Programmable_Interval_Timer

#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/system.h>
#include <asm/io.h>
#include <linux/sched.h>


union task_union {
    struct task_struct task;
    char stack[PAGE_SIZE];
};

static union task_union init_task = {INIT_TASK, };

struct task_struct *current = &(init_task);

long user_task[PAGE_SIZE >> 2];
long startup_time;

int count = 0;
long volatile j = 0;
void do_timer(long cpl)
{
    j++;
    count++;
    if(count == 10) {
        printk("jiffies = %d\n", j);
        count = 0;
    }
    outb(0x20, 0x20);
}


void test_timer_interrupt(void);

void init_timer()
{
    int division = 1193180 / HZ;

    // binary mode 3, LSB/MSB, ch0
    outb_p(0x36, 0x43);
    // LSB
    outb_p(division & 0xff, 0x40);
    // MSB
    outb(division >> 8, 0x40);

    // 设置时钟中断处理句柄
    set_intr_gate(0x20, &test_timer_interrupt);
    // 修改中断控制器屏蔽码,允许时钟中断
    outb(inb_p(0x21) & ~0x01, 0x21);
}

