#ifndef HEAD_H_
#define HEAD_H_

typedef struct desc_struct {
    unsigned long a, b;
} desc_table[256];


extern desc_table _idt, _gdt;

#endif
