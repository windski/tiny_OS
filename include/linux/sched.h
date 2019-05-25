#ifndef SCHED_H_
#define SCHED_H_

#define HZ 100

// 最多拥有的进程数
#define NR_TASK 64

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASK - 1]


#define TASK_RUNNING 0
#define TASK_INTERRUPTIBLE 1
#define TASK_UNINTERRUPTIBLE 2
#define TASK_ZOMBIE 3
#define TASK_STOPPED 4

#include <signal.h>
#include <linux/head.h>
#include <linux/mm.h>


typedef int (*fn_ptr)();


extern void schedule(void);

// 数学协处理器使用的结构, 主要用于保存进程切换时i387的状态
struct i387_struct {
    // Control word
    long cwd;
    // Status word
    long swd;
    // Tag word
    long twd;
    long fip;
    // code segment register
    long fcs;
    // operator offset
    long foo;
    // 内存操作数的段值
    long fos;
    // 8个10字节的协处理器累加值
    long st_space[20];
};


// 任务状态段(Task Status Segment)数据结构
struct tss_struct {
    long back_link;
    long esp0;
    long ss0;
    long esp1;
    long ss1;
    long esp2;
    long ss2;
    long cr3;
    long eip;
    long eflags;
    long eax, ecx, edx, ebx;
    long esp;
    long ebp;
    long esi;
    long edi;
    long es;
    long cs;
    long ss;
    long ds;
    long fs;
    long gs;
    long ldt;
    long trace_bitmap;
    struct i387_struct i387;
};


struct task_struct {
    long state;
    long counter;
    long priority;
    long signal;
    struct sigaction sigaction[32];
    long blocked;

    // various fields
    int exit_code;
    unsigned long start_code, end_code, end_data;
    // 总长度
    long brk;
    // 堆栈地址
    long start_stack;
    long pid, father, pgrp, session, leader;
    // 用户标识号, 有效用户id, 保存的用户id
    unsigned short uid, euid, suid;
    unsigned short gid, egid, sgid;
    long alarm;
    // 用户态运行时间, 系统态.., 子进程用户态.., 子进程系统态..,
    long utime, stime, cutime, cstime;
    long satrt_time;
    unsigned short used_math;

    // file system
    int tty;
    // TODO: file system

    // 本任务的局部描述符表.. 0 -> 空, 1 -> 代码段, 2 -> 数据和堆栈段
    struct desc_struct ldt[3];
    // 本进程的TSS
    struct tss_struct tss;
};


#define INIT_TASK       \
/* state info */        {0, 15, 15,         \
/* signals */           0, {{}, }, 0,       \
/* exit_code, brk */    0, 0, 0, 0, 0, 0,   \
/* pid */               0, -1, 0, 0, 0,     \
/* uid */               0, 0, 0, 0, 0, 0,   \
/* alarm, ... */        0, 0, 0, 0, 0, 0,   \
/* math */              0,                  \
/* tty */               0,                  \
/* ldt */   {                               \
                {0, 0},                     \
                {0x9f, 0xc0fa00},           \
                {0x9f, 0xc0f200},           \
            },                              \
/* TSS */   { 0,                            \
                PAGE_SIZE+(long)&init_task, \
                0x10, 0, 0, 0, 0,           \
                (long)&_pg_dir,              \
                0, 0, 0, 0, 0, 0, 0, 0,     \
                0, 0, 0x17, 0x17, 0x17,     \
                0x17, 0x17, 0x17,           \
                _LDT(0), 0x80000000,        \
                {}                          \
            }                               \
}


#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)

// 在全局表中的第n个任务的TSS描述符的索引号
// 第一个TSS的入口为4 << 3
#define _TSS(n) ((((unsigned long) n) << 4) + (FIRST_TSS_ENTRY) << 3)
#define _LDT(n) ((((unsigned long) n) << 4) + (FIRST_LDT_ENTRY) << 3)

// 加载第n个任务的任务管理器tr
#define ltr(n) volatile __asm__("ltr %%eax" :: "a"(_TSS(n)))
// 加载第n个任务的局部描述符表寄存器ldtr
#define lldt(n) volatile __asm__("lltd %%eax" :: "a"(_LDT(n)))

// 取当前的业务号
#define str(n) volatile __asm__(                \
        "str %%ax\n\t"                          \
        "subl %2, %%eax\n\t"                    \
        "shrl $4, %%eax\n\t"                    \
        : "=a" (n)                              \
        : "a" (0), "i" (FIRST_TSS_ENTRY) << 3   \
    )

// switch_to 将切换当前任务到任务nr,即n.首先检测任务n不是当前任务.如果是则直接退出.
// 如果切换到任务最近使用过的数学处理器,则还需要复位控制寄存器cr0中的TS flag
#define switch_to(n) {  \
    struct { long a, b; } __tmp;                    \
    __asm__ volatile ("cmpl %%ecx, current\n\t"     \
            "je 1f\n\t"                             \
            "movw %%dx, %1\n\t"                     \
            "xchgl %%ecx, current\n\t"              \
            "cmpl %%ecx, last_task_used_math\n\t"   \
            "jne 1f\n\t"                            \
            "clts\n"                                \
            "1:"                                    \
            ::"m" (*&__tmp.a), "m" (*&__tmp.b),     \
            "d" (_TSS(n)), "c" ((long) task[n]);    \
            );                                      \
}


// 设置位于地址addr 处描述符中的各基地址字段(基地址是`base')
// %0 -> addr + 2; %1 -> addr + 4; %2 -> addr + 7; edx -> base
#define _set_base(addr, base)                   \
    __asm__ volatile ("movw %%dx, %0\n\t"       \
            "rorl $16, %%edx\n\t"               \
            "movb %%dl, %1\n\t"                 \
            "movb %%dh, %2"                     \
            :: "m" (*((addr) + 2)),             \
             "m" (*((addr) + 4)),               \
             "m" (*((addr) + 7)),               \
             "d" (base) : "dx"                  \
            )


// 设置位于地址addr 处描述符中的段限长字段(段长为limit)
#define _set_limit(addr, limit)             \
    __asm__ volatile ("movw %%dx, %0\n\t"   \
            "rorl $16, %%edx\n\t"           \
            "movb %1, %%dh\n\t"             \
            "andb $0xf0, %%dh\n\t"          \
            "orb %%dh, %%dl\n\t"            \
            "movb %%dl, %1"                 \
            ::"m"(*(addr)), "m"(*(addr) + 6)\
            "d" (limit) : "dx"              \
            )


// 设置局部描述表中ldt 描述符的基地址字段.
#define set_base(ldt, base) _set_base( ((char *)&(ldt)), (base) )

// 设置局部描述符表ldt 的段长字段
#define set_limit(ldt, base) _set_limit( ((char *)&(ldt)), (base) )

// 从`addr'处取出段基地址, 与`_set_base'相反
#define _get_base(addr) ({          \
    unsigned long __base;           \
    __asm__ volatile (              \
            "movb %3, %%dh\n\t"     \
            "movb %2, %%dl\n\t"     \
            "shll $16, %%edx\n\t"   \
            "movw %1, %%dx\n\t"     \
            : "=d" (__base);        \
            : "m" ( *((addr) + 2) ),\
            "m" ( *((addr) + 4) ),  \
            "m" ( *((addr) + 7) )   \
            );                      \
    __base;                         \
    })

#define get_base(ldt) _get_base( ((char *)&(ldt)) )


// 取段选择符segment 的段长值; %0 -> 存放段长值; %1 -> 段选择符
#define get_limit(segment) ({       \
    unsigned long __limit;          \
    __asm__ volatile (              \
            "lsll %1, %0\n\t"       \
            "incl %0"               \
            : "=r" (__limit)        \
            : "r" (segment)         \
            );                      \
    __limit;                        \
    })

#endif

