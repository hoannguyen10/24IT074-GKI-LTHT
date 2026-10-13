#include "ls.h"
#include <unistd.h>


static int cmp_str(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[])
{
    Options opts;
    int     first_arg = 1;
    int     i;

    /* Zero-initialise every flag */
    memset(&opts, 0, sizeof(opts));

   
    parse_options(argc, argv, &opts, &first_arg);

   
    if (!opts.opt_q && !opts.opt_w) {
        if (isatty(STDOUT_FILENO))
            opts.opt_q = 1;
        else
            opts.opt_w = 1;
    }

    /* -f implies -a and disables sorting */
    if (opts.opt_f) {
        opts.opt_a = 1;
    }

    /* Super-user always sees all entries (-A behaviour) */
    if (getuid() == 0)
        opts.opt_A = 1;

  
    int operand_count = argc - first_arg;

    if (operand_count == 0) {
      
        list_path(".", &opts);
    } else if (operand_count == 1) {
        
        list_path(argv[first_arg], &opts);
    } else {
      
        struct stat sb;
        char **files = malloc(operand_count * sizeof(char *));
        char **dirs  = malloc(operand_count * sizeof(char *));
        int    nfiles = 0, ndirs = 0;

        if (!files || !dirs) {
            perror("malloc");
            exit(1);
        }

        for (i = first_arg; i < argc; i++) {
            if (lstat(argv[i], &sb) < 0) {
                /* Report the error but continue */
                fprintf(stderr, "ls: %s: %s\n", argv[i], strerror(errno));
                continue;
            }
          
            if (S_ISDIR(sb.st_mode) && !opts.opt_d)
                dirs[ndirs++] = argv[i];
            else
                files[nfiles++] = argv[i];
        }

        /* Sort each group lexicographically */
        if (!opts.opt_f) {
            if (nfiles > 1)
                qsort(files, nfiles, sizeof(char *), cmp_str);
            if (ndirs > 1)
                qsort(dirs,  ndirs,  sizeof(char *), cmp_str);
        }

        /* Display non-directory operands */
        for (i = 0; i < nfiles; i++)
            list_path(files[i], &opts);

        /* Display directory operands (print name header when > 1 dir) */
        for (i = 0; i < ndirs; i++) {
            if (nfiles > 0 || ndirs > 1)
                printf("%s:\n", dirs[i]);
            list_path(dirs[i], &opts);
            if (i < ndirs - 1)
                putchar('\n');
        }

        free(files);
        free(dirs);
    }

    return 0;
}
