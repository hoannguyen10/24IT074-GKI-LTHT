
#include "ls.h"
#include <ctype.h>


char *safe_strncpy(char *dst, const char *src, size_t n)
{
    if (n == 0)
        return dst;
    strncpy(dst, src, n - 1);
    dst[n - 1] = '\0';
    return dst;
}


int is_printable_name(const char *name)
{
    const unsigned char *p = (const unsigned char *)name;
    while (*p) {
        if (!isprint((int)*p))
            return 0;
        p++;
    }
    return 1;
}


void print_name(const char *name, const Options *opts)
{
    const unsigned char *p = (const unsigned char *)name;

    while (*p) {
        if (!isprint((int)*p) && opts->opt_q)
            putchar('?');
        else
            putchar((int)*p);
        p++;
    }
}
