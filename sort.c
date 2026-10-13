

#include "ls.h"


static time_t pick_time(const struct stat *sb, const Options *opts)
{
    if (opts->opt_c)
        return sb->st_ctime;   /* status-change time */
    if (opts->opt_u)
        return sb->st_atime;   /* last-access time   */
    return sb->st_mtime;       /* last-modification time (default) */
}

static int cmp_name(const void *a, const void *b)
{
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;
    return strcmp(fa->name, fb->name);
}

/* By size, largest first */
static int cmp_size(const void *a, const void *b)
{
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;
    if (fb->sb.st_size > fa->sb.st_size) return  1;
    if (fb->sb.st_size < fa->sb.st_size) return -1;
    /* Ties broken by name */
    return strcmp(fa->name, fb->name);
}

/* By chosen time, most recent first */
static int cmp_time(const void *a, const void *b)
{
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;
    time_t ta = pick_time(&fa->sb, g_opts);
    time_t tb = pick_time(&fb->sb, g_opts);
    if (tb > ta) return  1;
    if (tb < ta) return -1;
    /* Ties broken by name */
    return strcmp(fa->name, fb->name);
}


void sort_entries(FileEntry *entries, int count, const Options *opts)
{
    int (*comparator)(const void *, const void *) = NULL;

    /* -f means no sorting */
    if (opts->opt_f)
        return;

    /* Choose comparator */
    if (opts->opt_t)
        comparator = cmp_time;
    else if (opts->opt_S)
        comparator = cmp_size;
    else
        comparator = cmp_name;

    /* Store opts pointer for the comparator functions */
    g_opts = opts;

    qsort(entries, (size_t)count, sizeof(FileEntry), comparator);

    /* -r reverses the sorted result */
    if (opts->opt_r) {
        int left = 0, right = count - 1;
        while (left < right) {
            FileEntry tmp  = entries[left];
            entries[left]  = entries[right];
            entries[right] = tmp;
            left++;
            right--;
        }
    }
}
