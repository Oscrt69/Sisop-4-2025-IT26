#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#define INPUT_DIR "anomali"
#define IMAGE_DIR "anomali/image"
#define LOG_FILE "anomali/conversion.log"
#define MAX_FILENAME_LEN 256

void create_image_dir() {
    struct stat st = {0};
    if (stat(IMAGE_DIR, &st) == -1) {
        mkdir(IMAGE_DIR, 0755);
    }
}

void log_conversion(const char *input_file, const char *output_file, const char *datetime) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log == NULL) {
        perror("Failed to open log file");
        return;
    }

    fprintf(log, "[%s]: Successfully converted hexadecimal text %s to %s.\n", datetime, input_file, output_file);
    fclose(log);
}

int is_valid_hex(char c) {
    return isxdigit(c); // cek apakah karakter valid hexadecimal
}

void convert_hex_to_image(const char *filename) {
    char full_path[MAX_FILENAME_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", INPUT_DIR, filename);

    FILE *infile = fopen(full_path, "r");
    if (!infile) {
        perror("Failed to open input file");
        return;
    }

    // Baca seluruh isi file dan filter hanya karakter hex
    fseek(infile, 0, SEEK_END);
    long filesize = ftell(infile);
    rewind(infile);

    char *raw_data = malloc(filesize + 1);
    fread(raw_data, 1, filesize, infile);
    raw_data[filesize] = '\0';
    fclose(infile);

    // Filter hanya karakter hex (0-9, a-f, A-F)
    char *hex_data = malloc(filesize + 1);
    int j = 0;
    for (int i = 0; i < filesize; i++) {
        if (is_valid_hex(raw_data[i])) {
            hex_data[j++] = raw_data[i];
        }
    }
    hex_data[j] = '\0';
    free(raw_data);

    size_t data_len = strlen(hex_data);
    if (data_len < 2 || data_len % 2 != 0) {
        fprintf(stderr, "%s: skipped, invalid hex length: %zu\n", filename, data_len);
        free(hex_data);
        return;
    }

    // Konversi hex ke byte array
    unsigned char *buffer = malloc(data_len / 2);
    for (size_t i = 0; i < data_len; i += 2) {
        sscanf(hex_data + i, "%2hhx", &buffer[i / 2]);
    }

    // Dapatkan waktu
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char datetime[64];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M:%S", t);

    // Nama file output (tanpa .txt)
    char base_name[MAX_FILENAME_LEN];
    strncpy(base_name, filename, MAX_FILENAME_LEN);
    base_name[MAX_FILENAME_LEN - 1] = '\0';
    char *dot = strstr(base_name, ".txt");
    if (dot) *dot = '\0';

    char output_filename[MAX_FILENAME_LEN];
    snprintf(output_filename, sizeof(output_filename), "%s_image_%s.png", base_name, datetime);

    char output_path[MAX_FILENAME_LEN * 2];
    snprintf(output_path, sizeof(output_path), "%s/%s", IMAGE_DIR, output_filename);

    // Tulis ke file gambar
    FILE *outfile = fopen(output_path, "wb");
    if (!outfile) {
        perror("Failed to open output image file");
        free(hex_data);
        free(buffer);
        return;
    }

    fwrite(buffer, 1, data_len / 2, outfile);
    fclose(outfile);

    // Log
    char log_datetime[32];
    strftime(log_datetime, sizeof(log_datetime), "%Y-%m-%d][%H:%M:%S", t);
    log_conversion(filename, output_filename, log_datetime);

    free(hex_data);
    free(buffer);
}

int main() {
    create_image_dir();

    DIR *dir = opendir(INPUT_DIR);
    if (!dir) {
        perror("Failed to open input directory");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt")) {
            convert_hex_to_image(entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}

//REVISI
#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#define SOURCE_DIR "anomali"
#define IMAGE_DIR "anomali/image"
#define LOG_FILE "anomali/conversion.log"

#define MAX_PATH 512

void ensure_image_dir() {
    struct stat st = {0};
    if (stat(IMAGE_DIR, &st) == -1) {
        mkdir(IMAGE_DIR, 0755);
    }
}

int is_valid_hex(char c) {
    return isxdigit(c);
}

void convert_hex_to_image_if_needed(const char *filename) {
    char src_path[MAX_PATH];
    snprintf(src_path, sizeof(src_path), "%s/%s", SOURCE_DIR, filename);

    char base_name[128];
    strncpy(base_name, filename, sizeof(base_name));
    char *dot = strstr(base_name, ".txt");
    if (dot) *dot = '\0';

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char datetime[64];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M:%S", t);

    char output_file[256];
    snprintf(output_file, sizeof(output_file), "%s_image_%s.png", base_name, datetime);

    char output_path[MAX_PATH];
    snprintf(output_path, sizeof(output_path), "%s/%s", IMAGE_DIR, output_file);

    if (access(output_path, F_OK) == 0) return; 

    FILE *infile = fopen(src_path, "r");
    if (!infile) return;

    fseek(infile, 0, SEEK_END);
    long size = ftell(infile);
    rewind(infile);

    char *raw = malloc(size + 1);
    fread(raw, 1, size, infile);
    raw[size] = '\0';
    fclose(infile);

    char *hex = malloc(size + 1);
    int j = 0;
    for (int i = 0; i < size; i++) {
        if (is_valid_hex(raw[i])) hex[j++] = raw[i];
    }
    hex[j] = '\0';
    free(raw);

    if (j < 2 || j % 2 != 0) {
        free(hex);
        return;
    }

    unsigned char *buffer = malloc(j / 2);
    for (int i = 0; i < j; i += 2) {
        sscanf(hex + i, "%2hhx", &buffer[i / 2]);
    }

    FILE *outfile = fopen(output_path, "wb");
    if (outfile) {
        fwrite(buffer, 1, j / 2, outfile);
        fclose(outfile);
    }

    char logtime[64];
    strftime(logtime, sizeof(logtime), "%Y-%m-%d][%H:%M:%S", t);

    FILE *logf = fopen(LOG_FILE, "a");
    if (logf) {
        fprintf(logf, "[%s]: Successfully converted hexadecimal text %s to %s.\n",
                logtime, filename, output_file);
        fclose(logf);
    }

    free(hex);
    free(buffer);
}

// ================= FUSE HANDLERS =====================

static int fs_getattr(const char *path, struct stat *st, struct fuse_file_info *fi) {
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s%s", SOURCE_DIR, path);

    if (lstat(full_path, st) == -1) return -errno;
    return 0;
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi,
                      enum fuse_readdir_flags flags) {
    char dir_path[MAX_PATH];
    snprintf(dir_path, sizeof(dir_path), "%s%s", SOURCE_DIR, path);

    DIR *dp = opendir(dir_path);
    if (!dp) return -errno;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        filler(buf, de->d_name, NULL, 0, 0);

        if (strstr(path, "/image") == NULL && strstr(de->d_name, ".txt")) {
            convert_hex_to_image_if_needed(de->d_name);
        }
    }

    closedir(dp);
    return 0;
}

static int fs_open(const char *path, struct fuse_file_info *fi) {
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s%s", SOURCE_DIR, path);

    FILE *file = fopen(full_path, "r");
    if (!file) return -errno;
    fclose(file);

    return 0;
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
                   struct fuse_file_info *fi) {
    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s%s", SOURCE_DIR, path);

    FILE *file = fopen(full_path, "r");
    if (!file) return -errno;

    fseek(file, offset, SEEK_SET);
    size_t res = fread(buf, 1, size, file);
    fclose(file);

    return res;
}

// =====================================================

static const struct fuse_operations fs_oper = {
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .open = fs_open,
    .read = fs_read,
};

int main(int argc, char *argv[]) {
    ensure_image_dir();
    return fuse_main(argc, argv, &fs_oper, NULL);
}
