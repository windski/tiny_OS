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

struct i387_struct {
// TODO: fill it
};


#endif

