#ifndef STDDEF_H_
#define STDDEF_H_

#ifndef SIZE_T
#define SIZE_T
typedef unsigned long size_t;
#endif

#undef NULL
#define NULL ((void)*0)

#endif
