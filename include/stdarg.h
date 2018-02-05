#ifndef _STDARG_H_
#define _STDARG_H_

typedef char *va_list;

/* Amount of space required in an argument list for an arg of type TYPE.
 * TYPE may alternatively be an expression whose type is used. */

#define __va_rounded_size(TYPE)   \
    (((sizeof(TYPE) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#ifndef __sparc__
# define va_start(AP, LASTARG)                          \
    (AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))
#else
# define va_start(AP, LASTARG)                          \
    (__builtin_savergs(),                               \
     AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))
#endif

void va_end(va_list);   /* Defined in gunlib */
#define va_end(AP)

#define va_arg(AP, TYPE)                \
    (AP += __va_rounded_size(TYPE),     \
     *((TYPE *)(AP - __va_rounded_size(TYPE))))

#endif /* _STDARG_H_ */
