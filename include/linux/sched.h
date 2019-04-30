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

#endif

