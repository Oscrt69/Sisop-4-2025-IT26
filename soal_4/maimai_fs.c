#define FUSE_USE_VERSION 35
#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <zlib.h>
#include <openssl/evp.h>

#define PATH_MAX 4096
#define BASEDIR "/home/ardhana_48/maimai_project/data/chiho"
#define KEY "0123456789abcdef0123456789abcdef"
#define LOGFILE "/home/ardhana_48/maimai_project/maimai.log"

// Logging util
void log_action(const char *action, const char *path) {
    FILE *log = fopen(LOGFILE, "a");
    if (!log) return;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(log, "%04d-%02d-%02d %02d:%02d:%02d [%s] %s\n",
            tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, action, path);
    fclose(log);
}

// ======================== STARTER =========================
int read_starter(const char *fpath, char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    int r = pread(fd, buf, size, offset);
    close(fd);
    return r;
}
int write_starter(const char *fpath, const char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) return -errno;
    int r = pwrite(fd, buf, size, offset);
    close(fd);
    return r;
}

// ======================== METRO =========================
void shift_bytes(const char *src, char *dst, size_t len) {
    for (size_t i = 0; i < len; i++) dst[i] = src[i] + ((i + 1) % 256);
}
void unshift_bytes(const char *src, char *dst, size_t len) {
    for (size_t i = 0; i < len; i++) dst[i] = src[i] - ((i + 1) % 256);
}
int read_metro(const char *fpath, char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    char *tmp = malloc(size);
    int r = pread(fd, tmp, size, offset);
    shift_bytes(tmp, buf, r);
    free(tmp); close(fd);
    return r;
}
int write_metro(const char *fpath, const char *buf, size_t size, off_t offset) {
    char *tmp = malloc(size);
    unshift_bytes(buf, tmp, size);
    int fd = open(fpath, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) { free(tmp); return -errno; }
    int r = pwrite(fd, tmp, size, offset);
    free(tmp); close(fd);
    return r;
}

// ======================== DRAGON =========================
void rot13(const char *src, char *dst, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = src[i];
        if ('a' <= c && c <= 'z') dst[i] = 'a' + (c - 'a' + 13) % 26;
        else if ('A' <= c && c <= 'Z') dst[i] = 'A' + (c - 'A' + 13) % 26;
        else dst[i] = c;
    }
}
int read_dragon(const char *fpath, char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    char *tmp = malloc(size);
    int r = pread(fd, tmp, size, offset);
    rot13(tmp, buf, r);
    free(tmp); close(fd);
    return r;
}
int write_dragon(const char *fpath, const char *buf, size_t size, off_t offset) {
    char *tmp = malloc(size);
    rot13(buf, tmp, size);
    int fd = open(fpath, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) { free(tmp); return -errno; }
    int r = pwrite(fd, tmp, size, offset);
    free(tmp); close(fd);
    return r;
}

// ======================== BLACKROSE =========================
int read_blackrose(const char *fpath, char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    int r = pread(fd, buf, size, offset);
    close(fd);
    return r;
}
int write_blackrose(const char *fpath, const char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) return -errno;
    int r = pwrite(fd, buf, size, offset);
    close(fd);
    return r;
}

// ======================== HEAVEN (AES-256-CBC) =========================
int aes_decrypt_file(const char *path, char *buf, size_t maxlen) {
    FILE *f = fopen(path, "rb"); if (!f) return -errno;
    unsigned char iv[16]; fread(iv, 1, 16, f);
    fseek(f, 0, SEEK_END); long clen = ftell(f) - 16;
    fseek(f, 16, SEEK_SET);
    unsigned char *cipher = malloc(clen);
    fread(cipher, 1, clen, f); fclose(f);
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); int len, flen;
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)KEY, iv);
    EVP_DecryptUpdate(ctx, (unsigned char*)buf, &len, cipher, clen);
    EVP_DecryptFinal_ex(ctx, (unsigned char*)buf + len, &flen);
    EVP_CIPHER_CTX_free(ctx); free(cipher);
    return len + flen;
}
int aes_encrypt_file(const char *path, const char *buf, size_t len) {
    FILE *f = fopen(path, "wb"); if (!f) return -errno;
    unsigned char iv[16]; for (int i = 0; i < 16; i++) iv[i] = rand() % 256;
    fwrite(iv, 1, 16, f);
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char *cipher = malloc(len + 32);
    int outlen, tmplen;
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)KEY, iv);
    EVP_EncryptUpdate(ctx, cipher, &outlen, (const unsigned char*)buf, len);
    EVP_EncryptFinal_ex(ctx, cipher + outlen, &tmplen);
    fwrite(cipher, 1, outlen + tmplen, f);
    fclose(f); EVP_CIPHER_CTX_free(ctx); free(cipher);
    return outlen + tmplen;
}
int read_heaven(const char *fpath, char *buf, size_t size) {
    return aes_decrypt_file(fpath, buf, size);
}
int write_heaven(const char *fpath, const char *buf, size_t size) {
    return aes_encrypt_file(fpath, buf, size);
}

// ======================== YOUTH (GZIP) =========================
int read_youth(const char *fpath, char *buf, size_t size, off_t offset) {
    gzFile gz = gzopen(fpath, "rb");
    if (!gz) return -errno;
    gzseek(gz, offset, SEEK_SET);
    int r = gzread(gz, buf, size);
    gzclose(gz);
    return r;
}
int write_youth(const char *fpath, const char *buf, size_t size) {
    gzFile gz = gzopen(fpath, "wb");
    if (!gz) return -errno;
    int w = gzwrite(gz, buf, size);
    gzclose(gz);
    return w;
}
// lanjutan maimai_fs_v2.c — Dispatcher dan FUSE Handler

#include <sys/time.h>

// ======================== HELPER: Get Real Path =========================
// ======================== HELPER: Get Real Path (dengan validasi) =========================
int is_valid_chiho(const char *chiho) {
    const char *valid[] = {"starter", "metro", "dragon", "blackrose", "heaven", "youth", "7sref"};
    int n = sizeof(valid) / sizeof(valid[0]);
    for (int i = 0; i < n; i++) {
        if (strcmp(chiho, valid[i]) == 0) return 1;
    }
    return 0;
}

void get_real_path(const char *path, char *resolved) {
    char chiho[32];
    if (sscanf(path, "/%31[^/]/", chiho) == 1 && !is_valid_chiho(chiho)) {
        strcpy(resolved, "");  // invalid chiho, kosongkan path agar error
        return;
    }

    if (strncmp(path, "/starter/", 9) == 0)
        snprintf(resolved, PATH_MAX, "%s/starter%s.mai", BASEDIR, path + 8);
    else if (strncmp(path, "/metro/", 7) == 0)
        snprintf(resolved, PATH_MAX, "%s/metro%s", BASEDIR, path + 6);
    else if (strncmp(path, "/dragon/", 8) == 0)
        snprintf(resolved, PATH_MAX, "%s/dragon%s", BASEDIR, path + 7);
    else if (strncmp(path, "/blackrose/", 11) == 0)
        snprintf(resolved, PATH_MAX, "%s/blackrose%s", BASEDIR, path + 10);
    else if (strncmp(path, "/heaven/", 8) == 0)
        snprintf(resolved, PATH_MAX, "%s/heaven%s.enc", BASEDIR, path + 7);
    else if (strncmp(path, "/youth/", 7) == 0)
        snprintf(resolved, PATH_MAX, "%s/youth%s.gz", BASEDIR, path + 6);
    else if (strncmp(path, "/7sref/", 7) == 0) {
        char area[32], file[256];
        sscanf(path + 7, "%31[^_]_%255s", area, file);
        if (!is_valid_chiho(area)) {
            strcpy(resolved, "");  // invalid chiho area
            return;
        }
        snprintf(resolved, PATH_MAX, "%s/%s/%s", BASEDIR, area, file);
    } else {
        snprintf(resolved, PATH_MAX, "%s%s", BASEDIR, path);
    }
}

// ======================== Dispatcher =========================
int dispatch_read(const char *path, char *buf, size_t size, off_t offset) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    if (strstr(path, "/starter/")) return read_starter(fpath, buf, size, offset);
    if (strstr(path, "/metro/")) return read_metro(fpath, buf, size, offset);
    if (strstr(path, "/dragon/")) return read_dragon(fpath, buf, size, offset);
    if (strstr(path, "/blackrose/")) return read_blackrose(fpath, buf, size, offset);
    if (strstr(path, "/heaven/")) return read_heaven(fpath, buf, size);
    if (strstr(path, "/youth/")) return read_youth(fpath, buf, size, offset);
    if (strstr(path, "/7sref/")) return dispatch_read(fpath, buf, size, offset);
    return -ENOENT;
}

int dispatch_write(const char *path, const char *buf, size_t size, off_t offset) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    if (strstr(path, "/starter/")) return write_starter(fpath, buf, size, offset);
    if (strstr(path, "/metro/")) return write_metro(fpath, buf, size, offset);
    if (strstr(path, "/dragon/")) return write_dragon(fpath, buf, size, offset);
    if (strstr(path, "/blackrose/")) return write_blackrose(fpath, buf, size, offset);
    if (strstr(path, "/heaven/")) return write_heaven(fpath, buf, size);
    if (strstr(path, "/youth/")) return write_youth(fpath, buf, size);
    if (strstr(path, "/7sref/")) return dispatch_write(fpath, buf, size, offset);
    return -ENOENT;
}

// ======================== FUSE Handlers =========================
static int fs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    log_action("GETATTR", path);
    return lstat(fpath, stbuf);
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    DIR *dp = opendir(fpath);
    if (!dp) return -errno;
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (de->d_name[0] == '.') continue;
        filler(buf, de->d_name, NULL, 0, 0);
    }
    closedir(dp);
    log_action("READDIR", path);
    return 0;
}

static int fs_open(const char *path, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    int fd = open(fpath, fi->flags);
    if (fd == -1) return -errno;
    close(fd);
    log_action("OPEN", path);
    return 0;
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    log_action("READ", path);
    return dispatch_read(path, buf, size, offset);
}

static int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    log_action("WRITE", path);
    return dispatch_write(path, buf, size, offset);
}

static int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    int fd = creat(fpath, mode);
    if (fd == -1) return -errno;
    close(fd);
    log_action("CREATE", path);
    return 0;
}

static int fs_unlink(const char *path) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    int r = unlink(fpath);
    log_action("UNLINK", path);
    return r;
}

static int fs_utimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    get_real_path(path, fpath);
    if (strlen(fpath) == 0) return -ENOENT;
    log_action("UTIMENS", path);
    return utimensat(0, fpath, tv, 0);
}

// ======================== Mount & Oper =========================
static struct fuse_operations maimai_oper = {
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .open = fs_open,
    .read = fs_read,
    .write = fs_write,
    .create = fs_create,
    .unlink = fs_unlink,
    .utimens = fs_utimens,
};

int main(int argc, char *argv[]) {
    umask(0);
    srand(time(NULL));

    // Buat mountpoint jika belum ada
    if (argc >= 2) {
        struct stat st;
        if (stat(argv[1], &st) == -1) {
            if (mkdir(argv[1], 0755) == -1) {
                perror("Failed to create mountpoint");
                return 1;
            } else {
                log_action("MKDIR", argv[1]);
            }
        }
    }

    // Buat BASEDIR jika belum ada
    struct stat st;
    if (stat(BASEDIR, &st) == -1) {
        mkdir(BASEDIR, 0755);
        log_action("MKDIR", BASEDIR);
    }

    // Buat subdirektori untuk tiap chiho (kecuali 7sref)
    const char *chiho[] = {"starter", "metro", "dragon", "blackrose", "heaven", "youth"};
    int chiho_count = sizeof(chiho) / sizeof(chiho[0]);
    for (int i = 0; i < chiho_count; i++) {
        char subdir[PATH_MAX];
        snprintf(subdir, PATH_MAX, "%s/%s", BASEDIR, chiho[i]);
        if (stat(subdir, &st) == -1) {
            mkdir(subdir, 0755);
            log_action("MKDIR", subdir);
        }
    }

    return fuse_main(argc, argv, &maimai_oper, NULL);
}

