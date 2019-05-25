#ifndef SYS_H_
#define SYS_H_

#include <linux/sched.h>

int stub_systemcall()
{
    return 0;
}
extern int sys_fork();
extern int sys_pause();

fn_ptr system_call_table[] = {
    stub_systemcall,
    stub_systemcall,
    sys_fork,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,


    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,        //20


    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    sys_pause,
    stub_systemcall,


    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,        // 40


    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,


    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,        // 60


    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,

    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,
    stub_systemcall,


    stub_systemcall,
    stub_systemcall,
};


#endif
