#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

const char *src_path;
const char *log_path = "/var/log/it24.log";

void log_event(const char *type, const char *filename) {
    FILE *log = fopen(log_path, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, type, filename);
    fclose(log);
}

char *reverse_name(const char *name) {
    int len = strlen(name);
    char *rev = malloc(len+1);
    for (int i = 0; i < len; i++)
        rev[i] = name[len - i - 1];
    rev[len] = 0;
    return rev;
}

int is_dangerous(const char *filename) {
    return strstr(filename, "nafis") || strstr(filename, "kimcun");
}

static int x_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", src_path, path);

    DIR *dp = opendir(full_path);
    if (!dp) return -errno;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        char *name = strdup(de->d_name);
        if (is_dangerous(name)) {
            char *rev = reverse_name(name);
            filler(buf, rev, NULL, 0, 0);
            free(rev);
        } else {
            filler(buf, name, NULL, 0, 0);
        }
        free(name);
    }

    closedir(dp);
    return 0;
}

static int x_getattr(const char *path, struct stat *stbuf,
                     struct fuse_file_info *fi) {
    char real[1024];
    snprintf(real, sizeof(real), "%s%s", src_path, path);
    if (is_dangerous(path)) {
        char *rev = reverse_name(path);
        snprintf(real, sizeof(real), "%s/%s", src_path, rev);
        free(rev);
    }
    int res = lstat(real, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int x_open(const char *path, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", src_path, path);
    if (is_dangerous(path)) {
        log_event("ALERT", path);
        char *rev = reverse_name(path);
        snprintf(full_path, sizeof(full_path), "%s/%s", src_path, rev);
        free(rev);
    } else {
        log_event("READ", path);
    }

    int fd = open(full_path, fi->flags);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

static int x_read(const char *path, char *buf, size_t size, off_t offset,
                  struct fuse_file_info *fi) {
    int res = pread(fi->fh, buf, size, offset);

    if (!is_dangerous(path)) {
        for (size_t i = 0; i < res; ++i) {
            if (isalpha(buf[i])) {
                if ((buf[i] >= 'A' && buf[i] <= 'Z'))
                    buf[i] = ((buf[i] - 'A' + 13) % 26) + 'A';
                else if ((buf[i] >= 'a' && buf[i] <= 'z'))
                    buf[i] = ((buf[i] - 'a' + 13) % 26) + 'a';
            }
        }
    }

    return res;
}

static struct fuse_operations x_oper = {
    .readdir = x_readdir,
    .getattr = x_getattr,
    .open = x_open,
    .read = x_read,
};

int main(int argc, char *argv[]) {
    src_path = argv[1];
    char *mountpoint = argv[2];

    char *fuse_argv[] = { argv[0], mountpoint, "-f" };
    return fuse_main(3, fuse_argv, &x_oper, NULL);
}
