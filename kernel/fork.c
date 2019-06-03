#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/system.h>

#include <stddef.h>

long last_pid = 0;


// 设置新任务的代码和数据段地址.限长并复制页表.
int copy_mem(int nr, struct task_struct *p)
{
    unsigned long old_data_base, new_data_base, data_limit;
    unsigned long old_code_base, new_code_base, code_limit;

    code_limit = get_limit(0x0f);
    data_limit = get_limit(0x17);

    old_code_base = get_base(current->ldt[1]);
    old_data_base = get_base(current->ldt[2]);
    if(old_code_base != old_data_base) {
        panic("We do NOT support that..");
    }
    
    if(data_limit < code_limit) {
        panic("bad data limit");
    }

    new_data_base = new_code_base = nr * 0x4000000;
    p->start_code = new_code_base;
    set_base(p->ldt[1], new_code_base);
    set_base(p->ldt[2], new_data_base);

    if(copy_page_tables(old_data_base, new_data_base, data_limit)) {
        free_page_tables(new_data_base, data_limit);
        return -1;
    }

    return 0;
}



// 为新进程取得不重复的进程号last_pid
int find_empty_process(void)
{
    int i;

repeat:
    if((++last_pid) < 0)
        last_pid = 1;

    for(i = 0; i < NR_TASK; i++) {
        if(task[i] && task[i]->pid == last_pid)
            goto repeat;
    }

    for(i = 1; i < NR_TASK; i++) {
        if(!task[i]) {
            return i;
        }
    }

    return -1;
}


int
copy_process(int nr, long ebp, long edi, long esi, long gs, long none,
        long ebx, long ecx, long edx,  long fs, long es, long ds,
        long eip, long cs, long eflags, long esp, long ss)
{
    struct task_struct *p;
    /* int i; */

    p = (struct task_struct *)get_free_page();
    if(!p) {
        // allocate memory failure
        return -1;
    }

    task[nr] = p;
    *p = *current;

    p->state = TASK_UNINTERRUPTIBLE;
    p->pid = last_pid;
    p->counter = p->priority;
    p->father = current->pid;
    p->signal = 0;
    p->alarm = 0;
    p->leader = 0;
    p->utime = p->stime = 0;
    p->cutime = p->cstime = 0;
    p->start_time = jiffies;
    p->tss.back_link = 0;
    // 给当前任务分配了一个内存页面, 所以让它刚好指向该页顶端
    p->tss.esp0 = PAGE_SIZE + (long)p;
    // 指向内核数据段
    p->tss.ss0 = 0x10;
    p->tss.eip = eip;
    p->tss.eflags = eflags;
    p->tss.eax = 0;
    p->tss.ecx = ecx;
    p->tss.edx = edx;
    p->tss.ebx = ebx;
    p->tss.esp = esp;
    p->tss.ebp = ebp;
    p->tss.esi = esi;
    p->tss.edi = edi;

    p->tss.es = es & 0xffff;
    p->tss.cs = cs & 0xffff;
    p->tss.ss = ss & 0xffff;
    p->tss.ds = ds & 0xffff;
    p->tss.gs = gs & 0xffff;
    p->tss.fs = fs & 0xffff;
    p->tss.ldt = _LDT(nr);
    p->tss.trace_bitmap = 0x80000000;

    if(last_used_math == current) {
        __asm__("clts; fnsave %0"::"m" (p->tss.i387));
    }


    if(copy_mem(nr, p)) {
        task[nr] = NULL;
        free_page((long)p);
        return -1;
    }


    set_tss_desc(_gdt + (nr << 1) + FIRST_TSS_ENTRY, &(p->tss));
    set_ldt_desc(_gdt + (nr << 1) + FIRST_LDT_ENTRY, &(p->ldt));

    p->state = TASK_RUNNING;
    return last_pid;
}

