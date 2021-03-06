// 可编程中断定时器的设置可以参考这里 -> https://wiki.osdev.org/Programmable_Interval_Timer

#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/system.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/sys.h>

#include <stddef.h>


union task_union {
    struct task_struct task;
    char stack[PAGE_SIZE];
};

static union task_union init_task = {INIT_TASK, };

struct task_struct *current = &(init_task.task);
struct task_struct *last_used_math = NULL;
struct task_struct *task[NR_TASK] = {&(init_task.task), };

long user_task[PAGE_SIZE >> 2];
long startup_time;


struct {
    long *a;
    short b;
} start_stack = {&user_task[PAGE_SIZE >> 2], 0x10};


int count = 0;
long volatile jiffies = 0;
void do_timer(long cpl)
{
    count++;
    if(count == 10) {
        printk("jiffies = %d\n", jiffies);
        count = 0;
    }
    outb(0x20, 0x20);
}


// defined in `system_call.s`
extern void timer_interrupt(void);
extern int system_call();


int sys_pause(void)
{
    current->state = TASK_INTERRUPTIBLE;
    schedule();
    return 0;
}


void sched_init()
{
    int division = 1193180 / HZ;

    int i;
    struct task_struct *p;


    // binary mode 3, LSB/MSB, ch0
    outb_p(0x36, 0x43);
    // LSB
    outb_p(division & 0xff, 0x40);
    // MSB
    outb(division >> 8, 0x40);

    // 设置时钟中断处理句柄
    set_intr_gate(0x20, &timer_interrupt);
    // 修改中断控制器屏蔽码,允许时钟中断
    outb(inb_p(0x21) & ~0x01, 0x21);
}


void schedule(void)
{
    // TODO:

}

