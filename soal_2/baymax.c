#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>

#define BASE_DIR "/home/ardhana_48/baymax_project/relics"
#define FILENAME "Baymax.jpeg"
#define LOG_PATH "/home/ardhana_48/baymax_project/activity.log"

static void log_activity(const char *format, ...) {
    FILE *logf = fopen(LOG_PATH, "a");
    if (!logf) {
        perror("fopen activity.log failed");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);

    fprintf(logf, "[%s] ", timestr);

    va_list args;
    va_start(args, format);
    vfprintf(logf, format, args);
    va_end(args);

    fprintf(logf, "\n");
    fclose(logf);
}

static int baymax_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    log_activity("GETATTR %s", path);

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    } else if (strcmp(path + 1, FILENAME) == 0) {
        DIR *dp = opendir(BASE_DIR);
        if (!dp) return -errno;

        size_t total_size = 0;
        struct dirent *de;

        while ((de = readdir(dp)) != NULL) {
            if (strncmp(de->d_name, FILENAME, strlen(FILENAME)) == 0) {
                const char *ext = strchr(de->d_name + strlen(FILENAME), '.');
                if (ext && isdigit((unsigned char)ext[1])) {
                    char chunk_path[512];
                    snprintf(chunk_path, sizeof(chunk_path), "%s/%s", BASE_DIR, de->d_name);

                    struct stat st;
                    if (stat(chunk_path, &st) == 0) {
                        total_size += st.st_size;
                    }
                }
            }
        }
        closedir(dp);

        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = total_size;
        return 0;
    }

    return -ENOENT;
}

static int baymax_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info *fi,
                          enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, FILENAME, NULL, 0, 0);

    return 0;
}

static int baymax_open(const char *path, struct fuse_file_info *fi) {
    log_activity("OPEN %s", path);

    if (strcmp(path + 1, FILENAME) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int baymax_read(const char *path, char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi) {
    (void) fi;

    if (strcmp(path + 1, FILENAME) != 0)
        return -ENOENT;

    DIR *dp = opendir(BASE_DIR);
    if (!dp)
        return -errno;

    struct dirent **namelist;
    int n = scandir(BASE_DIR, &namelist, NULL, alphasort);
    if (n < 0) {
        closedir(dp);
        return -errno;
    }

    size_t bytes_read = 0;
    off_t current_offset = 0;

    for (int i = 0; i < n; i++) {
        struct dirent *de = namelist[i];

        if (strncmp(de->d_name, FILENAME, strlen(FILENAME)) != 0) {
            
            continue;
        }

        const char *ext = strchr(de->d_name + strlen(FILENAME), '.');
        if (!ext || !isdigit((unsigned char)ext[1])) {
            
            continue;
        }

        char chunk_path[512];
        snprintf(chunk_path, sizeof(chunk_path), "%s/%s", BASE_DIR, de->d_name);

        FILE *f = fopen(chunk_path, "rb");
        if (!f) {
            free(de);
            continue;
        }

        fseek(f, 0, SEEK_END);
        size_t chunk_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (offset < current_offset + chunk_size) {
            size_t chunk_offset = offset > current_offset ? offset - current_offset : 0;
            size_t to_read = chunk_size - chunk_offset;
            if (to_read > size - bytes_read)
                to_read = size - bytes_read;

            fseek(f, chunk_offset, SEEK_SET);
            size_t r = fread(buf + bytes_read, 1, to_read, f);
            bytes_read += r;
            offset += r;

            fclose(f);
            free(de);

            if (bytes_read == size) break;
        } else {
            fclose(f);
            free(de);
        }

        current_offset += chunk_size;
    }

    for (int j = 0; j < n; j++) {
        free(namelist[j]);
    }
    free(namelist);
    closedir(dp);

    log_activity("READ %s offset=%ld size=%zu read=%zu", path, offset, size, bytes_read);
    return bytes_read;
}

static const struct fuse_operations baymax_oper = {
    .getattr = baymax_getattr,
    .readdir = baymax_readdir,
    .open = baymax_open,
    .read = baymax_read,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &baymax_oper, NULL);
}
