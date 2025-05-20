#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <dirent.h>

static const char *host_dir = "/it24_host";
static const char *log_path = "/antink-system/antink-logs/it24.log";

void reverse_name(char *filename) {
    int len = strlen(filename);
    for (int i = 0; i < len/2; i++) {
        char temp = filename[i];
        filename[i] = filename[len-1-i];
        filename[len-1-i] = temp;
    }
}

void log_activity(const char *action, const char *filename) {
    time_t now;
    time(&now);
    char timestr[20];
    strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    FILE *log_file = fopen(log_path, "a");
    if (log_file) {
        if (strcmp(action, "ALERT") == 0) {
            fprintf(log_file, "[%s] [ALERT] Anomaly detected %s in file: %s\n", timestr, strstr(filename, "nafis") ? "nafis" : "kimcun", filename);
        } else if (strcmp(action, "REVERSE") == 0) {
            char reversed[256];
            strcpy(reversed, filename);
            reverse_name(reversed);
            fprintf(log_file, "[%s] [REVERSE] File %s has been reversed : %s\n", timestr, filename, reversed);
        } else if (strcmp(action, "ENCRYPT") == 0) {
            fprintf(log_file, "[%s] [ENCRYPT] File %s has been encrypted\n", timestr, filename);
        } else {
            fprintf(log_file, "[%s] %s: %s\n", timestr, action, filename);
        }
        fclose(log_file);
    }
}

int is_dangerous(const char *filename) {
    char lower[256];
    for (int i = 0; filename[i]; i++) {
        lower[i] = tolower(filename[i]);
    }
    return strstr(lower, "nafis") || strstr(lower, "kimcun");
}

void rot13(char *str) {
    for (; *str; str++) {
        if ((*str >= 'a' && *str <= 'm') || (*str >= 'A' && *str <= 'M')) {
            *str += 13;
        } else if ((*str >= 'n' && *str <= 'z') || (*str >= 'N' && *str <= 'Z')) {
            *str -= 13;
        }
    }
}

static int antink_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", host_dir, path);
    int res = lstat(full_path, stbuf);
    if (res == -1) return -errno;
    return 0;
}

static int antink_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", host_dir, path);
    DIR *dp = opendir(full_path);
    if (!dp) return -errno;
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char display_name[256];
        strcpy(display_name, de->d_name);
        if (is_dangerous(display_name)) {
            reverse_name(display_name);
            log_activity("REVERSE", de->d_name);
        }
        if (filler(buf, display_name, &st, 0, 0)) break;
    }
    closedir(dp);
    return 0;
}

static int antink_open(const char *path, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", host_dir, path);
    int res = open(full_path, fi->flags);
    if (res == -1) return -errno;
    close(res);
    log_activity("OPEN", path);
    return 0;
}

static int antink_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", host_dir, path);    
    int fd = open(full_path, O_RDONLY);
    if (fd == -1) return -errno;
    int res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    if (!is_dangerous(path)) {
        rot13(buf);
        log_activity("ENCRYPT", path);
        if (buf[res - 1] != '\n') {
        if (res < size) {
            buf[res] = '\n';
            res += 1;
        }
    }
    } else {
        log_activity("ALERT", path);
    }
    close(fd);
    return res;
}

static struct fuse_operations antink_oper = {
    .getattr = antink_getattr,
    .readdir = antink_readdir,
    .open = antink_open,
    .read = antink_read,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &antink_oper, NULL);
}
