#include <linux/kernel.h>

#define invalidate() \
    __asm__ volatile("mov %%eax, %%cr3"::"a"(0))

unsigned long put_page(unsigned long page, unsigned long addr);

void try_oom()
{
    for(int i = 0; i < 20 * 1024 * 1024; i++) {
        do_no_page(0, i);
    }

    return ;
}


// linear address transform to Page Table Entry.
unsigned long *linear_to_pte(unsigned long addr)
{
    // get the Page Directory Entry
    unsigned long *pde = (unsigned long *)((addr >> 20) & 0xffc);

    // if the page is not exist...
    if(!(*pde & 1) || (pde > 0x1000)) {
        return 0;
    }
    
    // and it is page table address
    pde = (unsigned long *)(*pde & 0xfffff000);
    // Page Table Address + page_table index = PTE

    return pde + ((addr >> 12) & 0x3ff);
}


void print_mminfo(unsigned long addr)
{
    unsigned long *pte = linear_to_pte(addr);

    printk("linear address: 0x%x, PTE address: 0x%x Flags [", addr, pte);

    if(*pte & 0x1)
        printk("P ");

    if(*pte & 0x2) {
        printk("R/W ");
    } else {
        printk("RO ");
    }

    if(*pte & 0x4) {
        printk("U/S ");
    } else {
        printk("S ");
    }

    printk("]\n");
    printk("Phyaddr: 0x%x", (*pte & 0xfffff000));
}

int mmtest_main(void)
{
    printk("\n\nRunning memory test program..\n");
    printk("try to put some pages\n");

    put_page(0x300000, 0xdad233);

    printk("\nPrint the page info of address at 0xdad233\n");
    print_mminfo(0xdad233);
    /* try_oom(); */
}

