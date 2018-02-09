#include <linux/kernel.h>
#include <asm/system.h>

/* these are not to be changed without changing system.s */
#define LOW_MEM 0x100000
#define PAGING_MEMORY (15 * 1024 * 1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr) - LOW_MEM)>>12) // 计算当前物理地址对应页号
#define USED 100

// 内存不够了,发个信号让系统死掉
static inline void oom(void)
{
    panic("Out of memory!! =w= ");
}

// 刷新缓存页
#define invalidate() __asm__("movl %%eax, %%cr3"::"a" (0))
#define copy_page(from, to)                     \
    __asm__("cld; rep; movsl;"                  \
            ::"S" (from), "D" (to), "c" (1024))

static long HIGH_MEMORY = 0;

static unsigned char mem_map[PAGING_PAGES] = {0,};

void mem_init(long start_mem, long end_mem)
{
    HIGH_MEMORY = end_mem;
    int i;
    for(i = 0; i < PAGING_PAGES; i++) {
        mem_map[i] = USED;
    }
    i = MAP_NR(start_mem);
    end_mem -= start_mem;
    end_mem >> 12;                // 计算多少页需要设置为free
    while(end_mem --> 0) {          // <- 出现了(c 语言语法糖),趋近于(误..23333
        mem_map[i++] = 0;
    }
    return ;
}


// Just Free a Page of Memory..
// 这个函数会将mem_map中相应的byte 置0
void free_page(unsigned long addr)
{
    if(addr < LOW_MEM)
        return ;
    if(addr > HIGH_MEMORY) {
        printk("Trying to free nonexistent page\n");
        return ;
    }

    addr = MAP_NR(addr);        // 计算出addr的页码
    if(mem_map[addr]--)
        return ;
    mem_map[addr] = 0;
    panic("Trying to free free page!??");
}


int free_page_tables(unsigned long from, unsigned long size)
{
    unsigned long *pg_table, *pg_dir, nr;

    if(from & 0x3fffff)
        panic("free_page_tables called with wrong alignment");
    if(!from)
        panic("try to free up swapper memory space");

    size = (size + 0x3fffff) >> 22;         // 计算占的目录项数
    pg_dir = (unsigned long *)((from >> 20) & 0xffc);           //计算目录项地址

    for( ; size --> 0; pg_dir++) {
        // 如果该页目录没有被使用..
        if(!(*pg_dir & 1))
            continue;
        // 得到页表地址
        pg_table = (unsigned long *)(0xfffff000 & *pg_dir);

        // 释放页表
        for(nr = 0; nr < 1024; nr++) {
            if(*pg_table & 1)
                free_page(0xfffff000 & *pg_table);

            *pg_table = 0;
            pg_table++;
        }
        // 释放这个页目录占的页
        free_page(0xfffff000 & *pg_dir);
        pg_dir++;
    }

    invalidate();
    return 0;
}


void calc_mem(void)
{
    int i, j, k;
    int free = 0;
    long *pg_table, *pg_dir;

    for(i = 0; i < PAGING_PAGES; i++)
        if(!mem_map[i])
            free++;
    printk("%d pages free (of %d)\n", free, PAGING_PAGES);

    for(i = 2; i < 1024; i++) {
        if(pg_dir[i] & 1) {
            pg_table = (long *)(0xfffff000 & pg_dir[i]);
            for(j = k = 0; j < 1024; j++) {
                if(pg_table[j] & 1) {
                    k++;
                }
            }
            printk("Pg-dir[%d] uses %d pages\n", i, k);
        }
    }
}


// 正如名字的那样, 获取一个空闲的内存页
// 通常来说就是最后一个, 然后标记它为USED, 没有空闲的页返回0
unsigned long get_free_page(void)
{
    register unsigned long __res asm("ax");

    __asm__ volatile ("std; repne; scasb\n\t"
            "jne 1f\n\t"
            "movb $1, 1(%%edi)\n\t"
            "sall $12, %%ecx\n\t"
            "addl %2, %%ecx\n\t"
            "movl %%ecx, %%edx\n\t"
            "movl $1024, %%ecx\n\t"
            "leal 4092(%%edx), %%edi\n\t"
            "rep; stosl;\n\t"
            "movl %%edx, %%eax\n"
            "1: cld"
            : "=a" (__res)
            : "0" (0), "i" (LOW_MEM), "c" (PAGING_PAGES),
            "D" (mem_map + PAGING_PAGES - 1));

    return __res;
}


// 此函数将一页内存地址放入页表(页目录)...
// page为页面的物理地址,addr为线性地址
unsigned long put_page(unsigned long page, unsigned long addr)
{
    unsigned long *pg_table, tmp;

    if(page < LOW_MEM || page >= HIGH_MEMORY)
        printk("Trying to put page %p at %p\n", page, addr);

    // mem_map 中此页的状态为unser状态
    if(mem_map[MAP_NR(page)] != 1)
        printk("mem_map disagree with %x at %x\n", page, addr);

    // 计算页目录地址
    pg_table = (unsigned long *) ((addr >> 20) & 0xffc);
    // 如果页目录存在,直接取出
    if(*pg_table & 1) {
        printk("Page table now available\n");
        pg_table = (unsigned long *)(*pg_table & 0xfffff000);
    } else {
        if(!(tmp = get_free_page())) {
            printk("No FREE Page!\n");
            return 0;
        }
        *pg_table = tmp | 7;
        printk("tmp = %x\n, Page Table = %x\n", tmp, *pg_table);
        pg_table = (unsigned long *)tmp;
    }
    printk("Put Page success\n");
    pg_table[(addr >> 12) & 0x3ff] = page | 7;

    return page;
}


void get_empty_page(unsigned long addr)
{
    unsigned long tmp;

    if(!(tmp = get_free_page()) || !put_page(tmp, addr)) {
        free_page(tmp);
        oom();
    }
    return ;
}

// 缺页异常处理函数
void do_no_page(unsigned long error_code, unsigned long addr)
{
    unsigned long page;

    addr &= 0xfffff000;
    if(!(page = get_free_page()))
        oom();
    if(put_page(page, addr))
        return ;
    free_page(page);
    oom();
}


// 解除页面的写入保护
void un_wp_page(unsigned long *table_entry)
{
    unsigned long old_page, new_page;

    old_page = *table_entry & 0xfffff000;

    // 页面在1MB之上
    if(old_page >= LOW_MEM && mem_map[MAP_NR(old_page)] == 1) {
        *table_entry |= 2;
        invalidate();
        return ;
    }

    // 无法分配新的页面
    if(!(new_page = get_free_page())) {
        oom();
    }

    // 页面被共享,现在进行复制,为它开辟一块新的页面
    if(old_page >= LOW_MEM)
        mem_map[MAP_NR(old_page)]--;
    *table_entry = new_page |= 7;
    invalidate();
    copy_page(old_page, new_page);
    return ;
}


void do_wp_page(unsigned long error_code, unsigned long addr)
{
    un_wp_page((unsigned long *)(((addr >> 10) & 0xffc) +
                ((*(unsigned long *)((addr >> 20) & 0xffc))) & 0xfffff000));
}


// 写页面验证,保证页面页面也如的合法性,如果不合法, 调用相关的处理函数
void write_verify(unsigned long addr)
{
    unsigned long page;

    // 检查页目录是否存在
    if(!((page = *((unsigned long *) ((addr >> 20) & 0xffc)) ) & 1)) {
        return ;
    }

    // 取页表首地址
    page &= 0xfffff000;
    page += ((addr >> 10) & 0xffc);
    if((3 & *(unsigned long *)page) == 1)           // 页表P = 1, R/W = 0
        un_wp_page((unsigned long *)page);

    return ;
}

