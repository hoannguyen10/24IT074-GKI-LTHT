/*
 * options.c - Command-line option parsing for ls(1)
 *
 * Implements parse_options(), which walks through argv[] and sets the
 * appropriate fields in the Options struct.  Option processing stops at
 * the first non-option argument or at "--".
 *
 * Author : 24IT074 - Nguyen Thi Thuy Hoan
 * Course : System Programming (Lap Trinh He Thong)
 */

#include "ls.h"

/*
 * print_usage - Print a short usage string and exit with a non-zero status.
 */
static void print_usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n", prog);
    exit(1);
}

/*
 * parse_options - Walk argv[], toggling flags in *opts for every recognised
 *                option character.
 *
 * Parameters:
 *   argc      - argument count from main()
 *   argv      - argument vector from main()
 *   opts      - pointer to the Options struct to populate
 *   first_arg - OUTPUT: set to the index of the first non-option argument
 */
void parse_options(int argc, char *argv[], Options *opts, int *first_arg)
{
    int i, j;

    for (i = 1; i < argc; i++) {
        /* "--" ends option processing explicitly */
        if (strcmp(argv[i], "--") == 0) {
            i++;
            break;
        }

        /* Options must start with '-' */
        if (argv[i][0] != '-')
            break;

        /* Walk each character in the option cluster (e.g. "-lR") */
        for (j = 1; argv[i][j] != '\0'; j++) {
            switch (argv[i][j]) {
            case 'A': opts->opt_A = 1; break;
            case 'a': opts->opt_a = 1; break;
            case 'c':
                /* -c and -u override each other; last one wins */
                opts->opt_c = 1;
                opts->opt_u = 0;
                break;
            case 'd': opts->opt_d = 1; opts->opt_R = 0; break;
            case 'F': opts->opt_F = 1; break;
            case 'f': opts->opt_f = 1; break;
            case 'h':
                /* -h and -k override each other; last one wins */
                opts->opt_h = 1;
                opts->opt_k = 0;
                break;
            case 'i': opts->opt_i = 1; break;
            case 'k':
                opts->opt_k = 1;
                opts->opt_h = 0;
                break;
            case 'l':
                /* -l and -n override each other; last one wins */
                opts->opt_l = 1;
                opts->opt_n = 0;
                break;
            case 'n':
                opts->opt_n = 1;
                opts->opt_l = 0;
                break;
            case 'q':
                /* -q and -w override each other; last one wins */
                opts->opt_q = 1;
                opts->opt_w = 0;
                break;
            case 'R': opts->opt_R = 1; opts->opt_d = 0; break;
            case 'r': opts->opt_r = 1; break;
            case 'S': opts->opt_S = 1; break;
            case 's': opts->opt_s = 1; break;
            case 't': opts->opt_t = 1; break;
            case 'u':
                opts->opt_u = 1;
                opts->opt_c = 0;
                break;
            case 'w':
                opts->opt_w = 1;
                opts->opt_q = 0;
                break;
            default:
                fprintf(stderr, "ls: illegal option -- %c\n", argv[i][j]);
                print_usage(argv[0]);
            }
        }
    }

    *first_arg = i;
}
