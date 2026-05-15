#include <stdio.h>

#if defined(__CC_ARM) && !defined(__MICROLIB)
#pragma import(__use_no_semihosting_swi)

struct __FILE
{
    int handle;
};

FILE __stdout;
FILE __stdin;

int ferror(FILE *f)
{
    (void)f;
    return EOF;
}

void _ttywrch(int ch)
{
    (void)ch;
}

void _sys_exit(int x)
{
    (void)x;
}
#endif
