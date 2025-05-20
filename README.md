# Sisop-4-2025-IT26
| Nama                         | Nrp        |
| ---------------------------- | ---------- |
| Azaria Raissa Maulidinnisa   | 5027241043 |
| Oscaryavat Viryavan          | 5027241053 |
| Naufal Ardhana               | 5027241118 |

# Soal 1
# Soal 2
```
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
```
Fungsi ini bertugas mencatat aktivitas pengguna ke dalam sebuah file log bernama activity.log. Setiap kali user melakukan aksi seperti membaca file, membuka direktori, atau mengakses atribut file, fungsi ini dipanggil untuk mencatat kejadian tersebut. Fungsi menerima parameter format seperti printf, sehingga fleksibel dalam mencatat berbagai bentuk informasi. Pertama-tama, waktu saat kejadian direkam menggunakan time() dan localtime(), lalu waktu tersebut diformat menjadi string waktu lokal. Setelah itu, file log dibuka dalam mode append (a), sehingga data baru tidak menimpa catatan sebelumnya. Jika file log berhasil dibuka, maka baris log baru akan ditulis menggunakan vfprintf, mencatat timestamp dan pesan sesuai format. File ditutup kembali setelah selesai menulis. Fungsi ini penting untuk keperluan audit, debugging, atau sekadar mencatat aktivitas filesystem buatan ini.

```
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
```
Fungsi ini menangani permintaan untuk mendapatkan atribut file atau direktori, mirip dengan perintah stat atau ls -l dalam sistem file biasa. FUSE akan memanggil fungsi ini ketika user mencoba mengetahui informasi seperti ukuran file, tipe (file atau direktori), dan permission. Jika path yang diminta adalah root ("/"), maka stbuf diatur sebagai direktori biasa dengan mode 0755 dan jumlah link 2 (default). Namun, jika path adalah "/Baymax.jpeg", maka fungsi akan membuka direktori BASE_DIR yang berisi potongan file JPEG. Fungsi lalu membaca seluruh isi direktori tersebut dan menjumlahkan ukuran semua file yang memiliki awalan Baymax.jpeg. dan diikuti oleh angka (misalnya Baymax.jpeg.1, Baymax.jpeg.2, dst). Ukuran gabungan tersebut akan dianggap sebagai ukuran total Baymax.jpeg. Fungsi ini membuat seolah-olah file utuh Baymax.jpeg benar-benar ada, meskipun sebenarnya hanya kumpulan potongan.

```
stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = total_size;
        return 0;
    }

    return -ENOENT;
}
```
merupakan bagian dari fungsi baymax_getattr, yang bertugas memberikan informasi atribut (metadata) dari sebuah file atau direktori dalam filesystem FUSE.

```
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
```
Fungsi ini dipanggil saat isi direktori diminta oleh user, seperti saat menggunakan perintah ls. Fungsi akan menentukan daftar file dan folder yang akan ditampilkan. Pada implementasi ini, jika path adalah root ("/"), maka fungsi akan mengisi buf dengan tiga entri: "." (direktori saat ini), ".." (direktori induk), dan "Baymax.jpeg". Hanya file virtual Baymax.jpeg yang ditampilkan, terlepas dari berapa banyak chunk yang ada di BASE_DIR. Artinya, user hanya melihat satu file utuh dalam sistem ini, meskipun di balik layar file tersebut terdiri dari banyak bagian. Fungsi ini tidak menampilkan file chunk secara langsung, sehingga menjaga abstraksi dan memberikan pengalaman pengguna seperti berinteraksi dengan filesystem biasa.

```
static int baymax_open(const char *path, struct fuse_file_info *fi) {
    log_activity("OPEN %s", path);

    if (strcmp(path + 1, FILENAME) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}
```
Fungsi ini menangani permintaan untuk membuka file. Dalam filesystem ini, hanya file Baymax.jpeg yang bisa diakses. Jika user mencoba membuka file selain Baymax.jpeg, maka fungsi akan mengembalikan error -ENOENT (file tidak ditemukan). Selain itu, fungsi juga memeriksa apakah file dibuka dalam mode selain read-only (O_RDONLY). Karena file ini hanya bisa dibaca, bukan ditulis atau dimodifikasi, maka setiap percobaan membuka file dengan mode lain akan ditolak dengan error -EACCES (akses ditolak). Jika semua validasi lolos, fungsi mencatat bahwa file berhasil dibuka ke file log. Dengan demikian, fungsi ini memastikan hanya file virtual yang valid yang bisa diakses, dan hanya dalam mode baca, demi menjaga konsistensi dan integritas data virtual.

```
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
```
Bagian awal fungsi baymax_read dimulai dengan memverifikasi apakah path yang diminta sama dengan FILENAME (yaitu Baymax.jpeg). Jika tidak cocok, maka akan langsung mengembalikan error -ENOENT (file tidak ditemukan). Selanjutnya, fungsi mencoba membuka direktori BASE_DIR yang menyimpan potongan file (chunk). Jika direktori gagal dibuka, program mengembalikan error dari errno. Kemudian, fungsi menggunakan scandir untuk membaca semua isi direktori dan menyimpannya dalam array namelist, yang juga diurutkan secara alfabet. Jika terjadi kegagalan saat memindai direktori, maka juga akan dikembalikan error dan direktori akan ditutup.

```
size_t bytes_read = 0;
off_t current_offset = 0;

for (int i = 0; i < n; i++) {
    struct dirent *de = namelist[i];

    if (strncmp(de->d_name, FILENAME, strlen(FILENAME)) != 0)
        continue;

    const char *ext = strchr(de->d_name + strlen(FILENAME), '.');
    if (!ext || !isdigit((unsigned char)ext[1]))
        continue;

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
```
Bagian utama dari fungsi ini melakukan iterasi terhadap semua file dalam namelist, dan memfilter hanya file-file yang merupakan potongan dari Baymax.jpeg (misalnya Baymax.jpeg.1, Baymax.jpeg.2, dst.). Setiap file yang valid dibuka, dan program menghitung ukurannya. Kemudian, berdasarkan nilai offset, fungsi menentukan apakah potongan tersebut perlu dibaca. Jika ya, ia menghitung berapa banyak byte yang perlu dibaca dari posisi tersebut, dan membaca ke dalam buffer buf. Proses ini akan terus berjalan sampai buffer penuh atau semua potongan selesai dibaca. Setelah selesai, semua memori yang dialokasikan untuk daftar file akan dibebaskan, dan direktori ditutup. Aktivitas pembacaan juga dicatat ke log dengan informasi ukuran, offset, dan jumlah byte yang dibaca.

```
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &baymax_oper, NULL);
}
```
Fungsi main adalah titik awal eksekusi program. Fungsi ini memanggil fuse_main, yang menjalankan sistem file FUSE berdasarkan operasi yang sudah didefinisikan dalam baymax_oper. Sebelum itu, fungsi memanggil umask(0) untuk menonaktifkan mask permission default dari sistem, agar file yang dibuat atau dimodifikasi oleh filesystem ini tidak dibatasi oleh umask. fuse_main akan memanggil fungsi-fungsi seperti getattr, read, open, atau readdir sesuai dengan interaksi user terhadap mount point (misalnya ls, cat, stat, dll). Dengan kata lain, main menginisialisasi dan mengaktifkan filesystem buatan ini.


# Soal 3

File berbahaya akan di-reverse penamaannya saat di ls:
```
void reverse_name(char *filename) {
    int len = strlen(filename);
    for (int i = 0; i < len/2; i++) {
        char temp = filename[i];
        filename[i] = filename[len-1-i];
        filename[len-1-i] = temp;
    }
}
```

Sedangkan file aman dapat di-encrypt dengan enkripsi rot13 saat cat:
```
void rot13(char *str) {
    for (; *str; str++) {
        if ((*str >= 'a' && *str <= 'm') || (*str >= 'A' && *str <= 'M')) {
            *str += 13;
        } else if ((*str >= 'n' && *str <= 'z') || (*str >= 'N' && *str <= 'Z')) {
            *str -= 13;
        }
    }
}
```

Cara melihat daftar container `docker ps -a` <br>

### Cara compile soal ini.<br>
Jalankan Docker Compose dengan `docker-compose build --no-cache` lalu `docker-compose up`<br>
Buka terminal baru lalu jalankan `docker exec -it antink-server ls /antink_mount` untuk melihat semua file baik file berbahaya yang di-reverse maupun file biasa <br>
`docker exec antink-server cat /antink_mount/[NAMA FILE]` dengan command ini, file teks normal akan di enkripsi menggunakan ROT13 saat dibaca, sedangkan file teks berbahaya tidak di enkripsi.  <br>
Hapus container dengan `docker-compose down` <br>

# Soal 4
