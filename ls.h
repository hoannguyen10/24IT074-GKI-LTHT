

#ifndef LS_H
#define LS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


typedef struct {
    int opt_A;   /* -A : list all except '.' and '..'              */
    int opt_a;   /* -a : include entries starting with '.'         */
    int opt_c;   /* -c : use ctime for sorting / printing          */
    int opt_d;   /* -d : list directories as plain files           */
    int opt_F;   /* -F : append type indicator                     */
    int opt_f;   /* -f : do not sort                               */
    int opt_h;   /* -h : human-readable sizes                      */
    int opt_i;   /* -i : print inode number                        */
    int opt_k;   /* -k : sizes in kilobytes                        */
    int opt_l;   /* -l : long format                               */
    int opt_n;   /* -n : like -l but show numeric uid/gid          */
    int opt_q;   /* -q : print non-printable chars as '?'          */
    int opt_R;   /* -R : recursive listing                         */
    int opt_r;   /* -r : reverse sort order                        */
    int opt_S;   /* -S : sort by size (largest first)              */
    int opt_s;   /* -s : print block usage                         */
    int opt_t;   /* -t : sort by modification time                 */
    int opt_u;   /* -u : use atime for sorting / printing          */
    int opt_w;   /* -w : force raw printing of non-printable chars */
} Options;


typedef struct {
    char  name[1024];      
    char  fullpath[4096]; 
    struct stat sb;        
} FileEntry;


 * Function prototypes (implemented across multiple .c files)
 * ----------------------------------------------------------------------- */

/* options.c */
void parse_options(int argc, char *argv[], Options *opts, int *first_arg);

/* list.c */
void list_path(const char *path, const Options *opts);
void list_directory(const char *path, const Options *opts, int print_header);
void list_entries(FileEntry *entries, int count, const Options *opts, const char *path);

/* sort.c */
void sort_entries(FileEntry *entries, int count, const Options *opts);

/* format.c */
void print_long_entry(const FileEntry *e, const Options *opts);
void print_short_entry(const FileEntry *e, const Options *opts);
char *format_size(off_t size, const Options *opts, char *buf, size_t bufsz);
char *format_blocks(blkcnt_t blocks, const Options *opts, char *buf, size_t bufsz);
void print_mode(mode_t mode);
char get_type_indicator(const FileEntry *e);

/* util.c */
char *safe_strncpy(char *dst, const char *src, size_t n);
int   is_printable_name(const char *name);
void  print_name(const char *name, const Options *opts);

#endif
