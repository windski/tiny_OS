#ifndef MM_H_
#define MM_H_

#define PAGE_SIZE 4096

typedef unsigned long u_long;

extern u_long get_free_page(void);
extern u_long put_page(u_long page, u_long addr);
extern void free_page(u_long addr);

extern void do_no_page(unsigned long error_code, unsigned long addr);
extern void calc_mem(void);
extern void print_mminfo(unsigned long addr);

#endif
