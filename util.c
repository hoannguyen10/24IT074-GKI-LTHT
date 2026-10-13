/*
 * util.c - Utility helpers for ls(1)
 *
 * Contains small, reusable functions for safe string operations and
 * controlled printing of file names (handling non-printable characters).
 *
 * Author : 24IT074 - Nguyen Thi Thuy Hoan
 * Course : System Programming (Lap Trinh He Thong)
 */

#include "ls.h"
#include <ctype.h>

/*
 * safe_strncpy - Copy at most n-1 bytes from src to dst and always
 *               NUL-terminate.  Returns dst.
 */
char *safe_strncpy(char *dst, const char *src, size_t n)
{
    if (n == 0)
        return dst;
    strncpy(dst, src, n - 1);
    dst[n - 1] = '\0';
    return dst;
}

/*
 * is_printable_name - Return 1 if every byte in 'name' is printable,
 *                     0 otherwise.
 */
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

/*
 * print_name - Print a file name according to the active -q / -w flags.
 *
 *   -q  : replace non-printable bytes with '?'
 *   -w  : print bytes as-is (raw)
 *   default (-q when stdout is a tty, else -w) is set in main().
 */
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
