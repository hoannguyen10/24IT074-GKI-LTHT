
#include "ls.h"


static int should_show(const char *name, const Options *opts)
{
    if (name[0] != '.')
        return 1;                       /* never hide non-dot files */

    if (opts->opt_a)
        return 1;                       /* -a: show all dot files   */

    if (opts->opt_A) {
        /* -A: show dot files but NOT '.' and '..' */
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            return 0;
        return 1;
    }

    return 0;  /* default: hide dot files */
}


void list_entries(FileEntry *entries, int count, const Options *opts,
                  const char *path)
{
    int i;

    /* Print "total N" line before long or -s listings, per the manual */
    if (opts->opt_l || opts->opt_n || opts->opt_s) {
        blkcnt_t total = 0;
        for (i = 0; i < count; i++)
            total += entries[i].sb.st_blocks;

        /* Convert 512-byte blocks to the display unit */
        char blkbuf[32];
        if (opts->opt_h) {
            off_t bytes = (off_t)total * 512;
            format_size(bytes, opts, blkbuf, sizeof(blkbuf));
        } else if (opts->opt_k) {
            long long kb = ((long long)total * 512 + 1023) / 1024;
            snprintf(blkbuf, sizeof(blkbuf), "%lld", kb);
        } else {
            snprintf(blkbuf, sizeof(blkbuf), "%lld", (long long)total);
        }
        printf("total %s\n", blkbuf);
    }

    /* Print each entry */
    for (i = 0; i < count; i++) {
        if (opts->opt_l || opts->opt_n)
            print_long_entry(&entries[i], opts);
        else
            print_short_entry(&entries[i], opts);
    }
}


void list_directory(const char *path, const Options *opts, int print_header)
{
    DIR           *dirp;
    struct dirent *dp;
    FileEntry     *entries = NULL;
    int            count   = 0;
    int            capacity = 64;

    if (print_header)
        printf("%s:\n", path);

    dirp = opendir(path);
    if (!dirp) {
        fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
        return;
    }

    entries = malloc(capacity * sizeof(FileEntry));
    if (!entries) {
        perror("malloc");
        closedir(dirp);
        return;
    }

    /* Read every entry in the directory */
    while ((dp = readdir(dirp)) != NULL) {
        /* Apply dot-file filter */
        if (!should_show(dp->d_name, opts))
            continue;

        /* Grow the array if needed */
        if (count == capacity) {
            capacity *= 2;
            FileEntry *tmp = realloc(entries, capacity * sizeof(FileEntry));
            if (!tmp) {
                perror("realloc");
                break;
            }
            entries = tmp;
        }

        FileEntry *fe = &entries[count];

        /* Store the base name */
        safe_strncpy(fe->name, dp->d_name, sizeof(fe->name));

        /* Build full path for lstat() */
        if (strcmp(path, ".") == 0)
            safe_strncpy(fe->fullpath, dp->d_name, sizeof(fe->fullpath));
        else
            snprintf(fe->fullpath, sizeof(fe->fullpath),
                     "%s/%s", path, dp->d_name);

        /* lstat so we don't follow symlinks */
        if (lstat(fe->fullpath, &fe->sb) < 0) {
            fprintf(stderr, "ls: %s: %s\n", fe->fullpath, strerror(errno));
            continue;
        }

        count++;
    }

    closedir(dirp);

    /* Sort */
    sort_entries(entries, count, opts);

    /* Display */
    list_entries(entries, count, opts, path);

    if (opts->opt_R) {
        for (int i = 0; i < count; i++) {
            if (!S_ISDIR(entries[i].sb.st_mode))
                continue;
            if (strcmp(entries[i].name, ".") == 0 ||
                strcmp(entries[i].name, "..") == 0)
                continue;
            putchar('\n');
            list_directory(entries[i].fullpath, opts, 1 /* print header */);
        }
    }

    free(entries);
}


void list_path(const char *path, const Options *opts)
{
    struct stat sb;

    if (lstat(path, &sb) < 0) {
        fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
        return;
    }

    if (S_ISDIR(sb.st_mode) && !opts->opt_d) {
       
        list_directory(path, opts, 0 /* no header for single operand */);
    } else {
   
        FileEntry fe;
        safe_strncpy(fe.name, path, sizeof(fe.name));
        safe_strncpy(fe.fullpath, path, sizeof(fe.fullpath));
        fe.sb = sb;

        if (opts->opt_l || opts->opt_n)
            print_long_entry(&fe, opts);
        else
            print_short_entry(&fe, opts);
    }
}
