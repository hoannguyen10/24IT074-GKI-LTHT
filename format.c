

#include "ls.h"
#include <time.h>
#include <math.h>

void print_mode(mode_t mode)
{
    char m[11];
    m[10] = '\0';

    /* --- Entry type character --- */
    if      (S_ISREG(mode))  m[0] = '-';
    else if (S_ISDIR(mode))  m[0] = 'd';
    else if (S_ISLNK(mode))  m[0] = 'l';
    else if (S_ISCHR(mode))  m[0] = 'c';
    else if (S_ISBLK(mode))  m[0] = 'b';
    else if (S_ISFIFO(mode)) m[0] = 'p';
    else if (S_ISSOCK(mode)) m[0] = 's';
    else                     m[0] = '?';

    /* --- Owner permissions --- */
    m[1] = (mode & S_IRUSR) ? 'r' : '-';
    m[2] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID) {
        m[3] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        m[3] = (mode & S_IXUSR) ? 'x' : '-';
    }

    /* --- Group permissions --- */
    m[4] = (mode & S_IRGRP) ? 'r' : '-';
    m[5] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID) {
        m[6] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        m[6] = (mode & S_IXGRP) ? 'x' : '-';
    }

    /* --- Other permissions (includes sticky bit) --- */
    m[7] = (mode & S_IROTH) ? 'r' : '-';
    m[8] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX) {
        m[9] = (mode & S_IXOTH) ? 't' : 'T';
    } else {
        m[9] = (mode & S_IXOTH) ? 'x' : '-';
    }

    printf("%s", m);
}

char get_type_indicator(const FileEntry *e)
{
    mode_t m = e->sb.st_mode;

    if (S_ISDIR(m))  return '/';
    if (S_ISLNK(m))  return '@';
    if (S_ISSOCK(m)) return '=';
    if (S_ISFIFO(m)) return '|';
    /* Executable regular file */
    if (S_ISREG(m) && (m & (S_IXUSR | S_IXGRP | S_IXOTH)))
        return '*';
    return '\0';
}

char *format_size(off_t size, const Options *opts, char *buf, size_t bufsz)
{
    if (opts->opt_h) {
        /* Human-readable: pick the largest unit where value >= 1 */
        double val = (double)size;
        const char *units[] = { "B", "K", "M", "G", "T", "P", NULL };
        int ui = 0;
        while (val >= 1024.0 && units[ui + 1]) {
            val /= 1024.0;
            ui++;
        }
        if (ui == 0)
            snprintf(buf, bufsz, "%g%s", val, units[ui]);
        else
            snprintf(buf, bufsz, "%.1f%s", val, units[ui]);
    } else if (opts->opt_k) {
        /* Kilobytes, rounded up */
        long long kb = ((long long)size + 1023) / 1024;
        snprintf(buf, bufsz, "%lld", kb);
    } else {
        snprintf(buf, bufsz, "%lld", (long long)size);
    }
    return buf;
}

char *format_blocks(blkcnt_t blocks, const Options *opts, char *buf, size_t bufsz)
{
    off_t bytes = (off_t)blocks * 512;

    if (opts->opt_h) {
        return format_size(bytes, opts, buf, bufsz);
    } else if (opts->opt_k) {
        long long kb = ((long long)bytes + 1023) / 1024;
        snprintf(buf, bufsz, "%lld", kb);
    } else {
        /* 512-byte blocks, partial units rounded up */
        snprintf(buf, bufsz, "%lld", (long long)blocks);
    }
    return buf;
}

static time_t pick_time(const struct stat *sb, const Options *opts)
{
    if (opts->opt_c) return sb->st_ctime;
    if (opts->opt_u) return sb->st_atime;
    return sb->st_mtime;
}

void print_long_entry(const FileEntry *e, const Options *opts)
{
    const struct stat *sb = &e->sb;
    char sizebuf[32];
    char blkbuf[32];

    /* Optional: inode number */
    if (opts->opt_i)
        printf("%7lu ", (unsigned long)sb->st_ino);

    /* Optional: block usage */
    if (opts->opt_s) {
        format_blocks(sb->st_blocks, opts, blkbuf, sizeof(blkbuf));
        printf("%s ", blkbuf);
    }

    /* File mode */
    print_mode(sb->st_mode);
    printf(" ");

    /* Number of hard links */
    printf("%3lu ", (unsigned long)sb->st_nlink);

    /* Owner and group */
    if (opts->opt_n) {
        /* Numeric IDs */
        printf("%-8u %-8u ", (unsigned)sb->st_uid, (unsigned)sb->st_gid);
    } else {
        /* Symbolic names (fall back to numeric if not found) */
        struct passwd *pw = getpwuid(sb->st_uid);
        struct group  *gr = getgrgid(sb->st_gid);
        if (pw) printf("%-8s ", pw->pw_name);
        else    printf("%-8u ", (unsigned)sb->st_uid);
        if (gr) printf("%-8s ", gr->gr_name);
        else    printf("%-8u ", (unsigned)sb->st_gid);
    }

    /* Size (or major/minor for device files) */
    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode)) {
        /* Display major and minor device numbers in the size column */
        printf("%3u, %3u ", (unsigned)major(sb->st_rdev),
                            (unsigned)minor(sb->st_rdev));
    } else {
        format_size(sb->st_size, opts, sizebuf, sizeof(sizebuf));
        printf("%8s ", sizebuf);
    }

    /* Timestamp */
    time_t t = pick_time(sb, opts);
    struct tm *tm_info = localtime(&t);
    char timebuf[64];
    /* If the file is more than ~6 months old, show year instead of time */
    time_t now = time(NULL);
    double diff = difftime(now, t);
    if (diff > 15552000 || diff < 0)   /* 180 days */
        strftime(timebuf, sizeof(timebuf), "%b %e  %Y", tm_info);
    else
        strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", tm_info);
    printf("%s ", timebuf);

    /* File name */
    print_name(e->name, opts);

    /* -F indicator */
    if (opts->opt_F) {
        char ind = get_type_indicator(e);
        if (ind) putchar(ind);
    }

    /* Symbolic link target */
    if (S_ISLNK(sb->st_mode)) {
        char target[4096];
        ssize_t len = readlink(e->fullpath, target, sizeof(target) - 1);
        if (len >= 0) {
            target[len] = '\0';
            printf(" -> %s", target);
        }
    }

    putchar('\n');
}

void print_short_entry(const FileEntry *e, const Options *opts)
{
    char blkbuf[32];

    /* Optional: inode number */
    if (opts->opt_i)
        printf("%7lu ", (unsigned long)e->sb.st_ino);

    /* Optional: block usage */
    if (opts->opt_s) {
        format_blocks(e->sb.st_blocks, opts, blkbuf, sizeof(blkbuf));
        printf("%s ", blkbuf);
    }

    /* File name */
    print_name(e->name, opts);

    /* -F indicator */
    if (opts->opt_F) {
        char ind = get_type_indicator(e);
        if (ind) putchar(ind);
    }

    putchar('\n');
}
