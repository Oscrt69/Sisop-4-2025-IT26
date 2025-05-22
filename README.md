# Sisop-4-2025-IT26
| Nama                         | Nrp        |
| ---------------------------- | ---------- |
| Azaria Raissa Maulidinnisa   | 5027241043 |
| Oscaryavat Viryavan          | 5027241053 |
| Naufal Ardhana               | 5027241118 |

# Soal 1
Mendownload file zip, kemudian mengunzip dan menghapus file tersebut.
```
wget -O anomali.zip "https://drive.usercontent.google.com/u/0/uc?id=1hi_GDdP51Kn2JJMw02WmCOxuc3qrXzh5&export=download"
unzip anomali.zip
rm -rf anomali.zip
```
Mendownload file zip 
![Screenshot 2025-05-21 183316](https://github.com/user-attachments/assets/c225cde5-a64c-4e2d-b8ed-7e31ca751935)

Unzip file

![Screenshot 2025-05-21 183436](https://github.com/user-attachments/assets/1f03292a-ed8a-4769-a1af-163794bf5c03)

Menghapus file zip 

![Screenshot 2025-05-21 183506](https://github.com/user-attachments/assets/af5d4f48-01af-498a-b4f5-654a211da522)

```
void create_image_dir() {
    struct stat st = {0};
    if (stat(IMAGE_DIR, &st) == -1) {
        mkdir(IMAGE_DIR, 0755);
    }
}
```
Fungsi ```create_image_dir()``` mengecek apakah direktori ```IMAGE_DIR``` sudah ada. Jika belum ada, maka akan dibuat dengan permission ```0755``` (read-write-execute untuk owner, read-execute untuk group dan others).
```
void log_conversion(const char *input_file, const char *output_file, const char *datetime) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log == NULL) {
        perror("Failed to open log file");
        return;
    }

    fprintf(log, "[%s]: Successfully converted hexadecimal text %s to %s.\n", datetime, input_file, output_file);
    fclose(log);
}
```
Fungsi ```log_conversion()``` bertugas untuk membuka file log ```(LOG_FILE)``` dalam mode append ```("a")```. Menuliskan log bahwa file teks berhasil dikonversi ke file gambar. Format log: ```[tanggal dan waktu]: Successfully converted hexadecimal text input_file to output_file.```
```
int is_valid_hex(char c) {
    return isxdigit(c); // cek apakah karakter valid hexadecimal
}
```
Mengecek apakah karakter ```c``` termasuk dalam karakter heksadesimal ```(0-9, a-f, A-F)``` menggunakan fungsi bawaan ```isxdigit()```.
```
void convert_hex_to_image(const char *filename) {
    char full_path[MAX_FILENAME_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", INPUT_DIR, filename);
```
Fungsi utama ```convert_hex_to_image``` mengonversi file ```.txt``` berisi data heksadesimal menjadi file gambar ```.png```, lalu mencatat proses konversinya ke ```file log```. Bagian ini menyusun path lengkap ke file input dengan menggabungkan direktori input ```(INPUT_DIR)``` dengan nama file ```filename``` menjadi ```full_path```.
```
FILE *infile = fopen(full_path, "r");
if (!infile) {
    perror("Failed to open input file");
    return;
}
```
Bagian ini bertugas membuka file teks untuk dibaca. Jika gagal dibuka (file tidak ada atau permission error), tampilkan pesan kesalahan dan keluar dari fungsi.
```
fseek(infile, 0, SEEK_END);
long filesize = ftell(infile);
rewind(infile);

char *raw_data = malloc(filesize + 1);
fread(raw_data, 1, filesize, infile);
raw_data[filesize] = '\0';
fclose(infile);

```
Bagian ini berfungsi membaca isi file ke memori dengan mengukur ukuran file, lalu kembali ke awal file. Isi file dibaca ke dalam ```raw_data``` sebagai string. Tambahan ```\0``` untuk mengakhiri string.
```
char *hex_data = malloc(filesize + 1);
int j = 0;
for (int i = 0; i < filesize; i++) {
    if (is_valid_hex(raw_data[i])) {
        hex_data[j++] = raw_data[i];
    }
}
hex_data[j] = '\0';
free(raw_data);
```
Bagian ini bertugas menyaring hanya karakter heksadesimal dengan memfilter hanya karakter valid hex ```(0-9, a-f, A-F)``` dan menyimpannya di ```hex_data```. ```is_valid_hex()``` adalah fungsi yang menggunakan ```isxdigit()```.
```
if (data_len < 2 || data_len % 2 != 0) {
    fprintf(stderr, "%s: skipped, invalid hex length: %zu\n", filename, data_len);
    free(hex_data);
    return;
}
```
Bagian ini memvalidasi panjang data hex, dimana Hex harus genap (2 digit = 1 byte), dan minimal 1 byte (2 digit). Jika tidak memenuhi syarat, file dilewati.
```
unsigned char *buffer = malloc(data_len / 2);
for (size_t i = 0; i < data_len; i += 2) {
    sscanf(hex_data + i, "%2hhx", &buffer[i / 2]);
}
```
Bagian ini bertugas mengkonversi hex string ke array byte. Setiap dua karakter diubah menjadi 1 byte dan disimpan di ```buffer```.
```
time_t now = time(NULL);
struct tm *t = localtime(&now);
char datetime[64];
strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M:%S", t);

strncpy(base_name, filename, MAX_FILENAME_LEN);
char *dot = strstr(base_name, ".txt");
if (dot) *dot = '\0';

snprintf(output_filename, sizeof(output_filename), "%s_image_%s.png", base_name, datetime);
snprintf(output_path, sizeof(output_path), "%s/%s", IMAGE_DIR, output_filename);
```
Bagian ini membuat nama file output berdasarkan waktu dengan mendapatkan timestamp untuk digunakan sebagai bagian dari nama file. Kemudian menghapus ```.txt``` dari nama file untuk membuat nama dasar file output. Serta membuat nama file PNG dengan format: ```namafile_image_tanggal.png```, lalu digabungkan dengan direktori output.
```
FILE *outfile = fopen(output_path, "wb");
if (!outfile) {
    perror("Failed to open output image file");
    free(hex_data);
    free(buffer);
    return;
}
fwrite(buffer, 1, data_len / 2, outfile);
fclose(outfile);
```
Bagian ini berfungsi menulis file output dengan membuka file PNG untuk ditulis dalam mode biner (```wb```), kemudian menulis isi ```buffer``` ke file dan terakhir menutup file.
```
char log_datetime[32];
strftime(log_datetime, sizeof(log_datetime), "%Y-%m-%d][%H:%M:%S", t);
log_conversion(filename, output_filename, log_datetime);
```
Bagian ini akan menuliskan log konversi dengan Membuat format timestamp log seperti ```[2025-05-21][14:30:00]```. Memanggil fungsi ```log_conversion()``` untuk mencatat file yang dikonversi.
```
free(hex_data);
free(buffer);
```
Bagian ini akan membebaskan alokasi memori setelah selesai.

Menjalankan file hexed.c
![Screenshot 2025-05-21 183758](https://github.com/user-attachments/assets/e43ce5e1-171b-4318-8f2b-4a98bc4aa9aa)

Output conversion_log
![Screenshot 2025-05-21 183626](https://github.com/user-attachments/assets/0bca135b-7eda-4d23-9634-2a80dc4eb917)

Struktur direktori akhir 

![Screenshot 2025-05-21 183843](https://github.com/user-attachments/assets/6f2e40f4-89a3-414d-81dc-3204b767d533)

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
Fungsi ini bertugas mencatat aktivitas pengguna ke dalam sebuah file log bernama `activity.log` Setiap kali user melakukan aksi seperti membaca file, membuka direktori, atau mengakses atribut file, fungsi ini dipanggil untuk mencatat kejadian tersebut. Fungsi menerima parameter format seperti `printf`, sehingga fleksibel dalam mencatat berbagai bentuk informasi. Pertama-tama, waktu saat kejadian direkam menggunakan `time()` dan `localtime()`, lalu waktu tersebut diformat menjadi string waktu lokal. Setelah itu, file log dibuka dalam mode append (a), sehingga data baru tidak menimpa catatan sebelumnya. Jika file log berhasil dibuka, maka baris log baru akan ditulis menggunakan `vfprintf`, mencatat timestamp dan pesan sesuai format. File ditutup kembali setelah selesai menulis. Fungsi ini penting untuk keperluan audit, debugging, atau sekadar mencatat aktivitas filesystem buatan ini.

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
Fungsi ini menangani permintaan untuk mendapatkan atribut file atau direktori, mirip dengan perintah `stat` atau `ls -l` dalam sistem file biasa. FUSE akan memanggil fungsi ini ketika user mencoba mengetahui informasi seperti ukuran file, tipe (file atau direktori), dan permission. Jika path yang diminta adalah root `("/")`, maka `stbuf` diatur sebagai direktori biasa dengan mode 0755 dan jumlah link 2 (default). Namun, jika path adalah `"/Baymax.jpeg"`, maka fungsi akan membuka direktori `BASE_DIR` yang berisi potongan file JPEG. Fungsi lalu membaca seluruh isi direktori tersebut dan menjumlahkan ukuran semua file yang memiliki awalan `Baymax.jpeg`. dan diikuti oleh angka (misalnya `Baymax.jpeg.1`, `Baymax.jpeg.2`, dst). Ukuran gabungan tersebut akan dianggap sebagai ukuran total `Baymax.jpeg`. Fungsi ini membuat seolah-olah file utuh `Baymax.jpeg` benar-benar ada.

```
stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = total_size;
        return 0;
    }

    return -ENOENT;
}
```
merupakan bagian dari fungsi `baymax_getattr`, yang bertugas memberikan informasi atribut (metadata) dari sebuah file atau direktori dalam filesystem FUSE.

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
Fungsi ini dipanggil saat isi direktori diminta oleh user, seperti saat menggunakan perintah `ls`. Fungsi akan menentukan daftar file dan folder yang akan ditampilkan. Pada implementasi ini, jika path adalah root `("/")`, maka fungsi akan mengisi buf dengan tiga entri: `"."` (direktori saat ini), ".." (direktori induk), dan `"Baymax.jpeg"`. Hanya file virtual Baymax.jpeg yang ditampilkan, terlepas dari berapa banyak chunk yang ada di `BASE_DIR`. Artinya, user hanya melihat satu file utuh dalam sistem ini, meskipun di balik layar file tersebut terdiri dari banyak bagian. Fungsi ini tidak menampilkan file chunk secara langsung, sehingga menjaga abstraksi dan memberikan pengalaman pengguna seperti berinteraksi dengan filesystem biasa.

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
Fungsi ini menangani permintaan untuk membuka file. Dalam filesystem ini, hanya file `Baymax.jpeg` yang bisa diakses. Jika user mencoba membuka file selain `Baymax.jpeg`, maka fungsi akan mengembalikan error `-ENOENT` (file tidak ditemukan). Selain itu, fungsi juga memeriksa apakah file dibuka dalam mode selain read-only (`O_RDONLY`). Karena file ini hanya bisa dibaca, bukan ditulis atau dimodifikasi, maka setiap percobaan membuka file dengan mode lain akan ditolak dengan error `-EACCES` (akses ditolak). Jika semua validasi lolos, fungsi mencatat bahwa file berhasil dibuka ke file log. Dengan demikian, fungsi ini memastikan hanya file virtual yang valid yang bisa diakses, dan hanya dalam mode baca, demi menjaga konsistensi dan integritas data virtual.

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
Bagian awal fungsi `baymax_read` dimulai dengan memverifikasi apakah path yang diminta sama dengan `FILENAME` (yaitu `Baymax.jpeg`). Jika tidak cocok, maka akan langsung mengembalikan error `-ENOENT` (file tidak ditemukan). Selanjutnya, fungsi mencoba membuka direktori `BASE_DIR` yang menyimpan potongan file (`chunk`). Jika direktori gagal dibuka, program mengembalikan error dari `errno`. Kemudian, fungsi menggunakan scandir untuk membaca semua isi direktori dan menyimpannya dalam array `namelist`, yang juga diurutkan secara alfabet. Jika terjadi kegagalan saat memindai direktori, maka juga akan dikembalikan error dan direktori akan ditutup.

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
Bagian utama dari fungsi ini melakukan iterasi terhadap semua file dalam `namelist`, dan memfilter hanya file-file yang merupakan potongan dari `Baymax.jpeg` (misalnya `Baymax.jpeg.1`, `Baymax.jpeg.2`, dst.). Setiap file yang valid dibuka, dan program menghitung ukurannya. Kemudian, berdasarkan nilai `offset`, fungsi menentukan apakah potongan tersebut perlu dibaca. Jika ya, ia menghitung berapa banyak byte yang perlu dibaca dari posisi tersebut, dan membaca ke dalam buffer `buf`. Proses ini akan terus berjalan sampai buffer penuh atau semua potongan selesai dibaca. Setelah selesai, semua memori yang dialokasikan untuk daftar file akan dibebaskan, dan direktori ditutup. Aktivitas pembacaan juga dicatat ke log dengan informasi ukuran, offset, dan jumlah byte yang dibaca.

```
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &baymax_oper, NULL);
}
```
Fungsi `main` adalah titik awal eksekusi program. Fungsi ini memanggil `fuse_main`, yang menjalankan sistem file FUSE berdasarkan operasi yang sudah didefinisikan dalam `baymax_oper`. Sebelum itu, fungsi memanggil `umask(0)` untuk menonaktifkan mask permission default dari sistem, agar file yang dibuat atau dimodifikasi oleh filesystem ini tidak dibatasi oleh `umask`. `fuse_main` akan memanggil fungsi-fungsi seperti `getattr`, `read`, `open`, atau `readdir` sesuai dengan interaksi user terhadap mount point (misalnya `ls`, `cat`, `stat`, dll). Dengan kata lain, main menginisialisasi dan mengaktifkan filesystem buatan ini.
### Hasil Output activity log dari baymax
![image](https://github.com/user-attachments/assets/4ce5563a-ec7a-4b19-863d-c984f26fe62a)
### Hasil Output saat baymax jpeg digabung dan diplace di mount_dir
![image](https://github.com/user-attachments/assets/e11bb07a-6445-4222-8e98-ac74fafc77eb)

# Soal 3

File berbahaya akan di-reverse penamaannya saat di-ls:
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

### Melihat daftar container <br>

`docker ps -a` 

<img src = "https://github.com/user-attachments/assets/00d41302-73c2-499f-ab1f-4f08c4f97175" width = "500"> <br>


Nama-nama seperti:

> silly_bell

> sharp_villani

> zealous_mayer

dan lain-lain adalah nama default otomatis yang diberikan oleh Docker.

### Cara compile soal ini.<br>
Jalankan Container dengan `docker-compose up -d`, namun jika setelah mengubah apapun di Dockerfile atau antink.c, gunakan `docker-compose build --no-cache` terlebih dahulu untuk build ulang<br>

Sebelum menjalankan dockerfile, pastikan semua paket diinstal ulang:

<img src = "https://github.com/user-attachments/assets/b8e1af11-3d45-46ec-82ee-c5d1c4103955" width = "500" ><br>
<img src = "https://github.com/user-attachments/assets/91c1f25a-3b2e-4c01-9b51-cba6577021b0" width = "500"> <br>

Jalankan `docker exec -it antink-server ls /antink_mount` untuk melihat semua file baik file berbahaya yang di-reverse maupun file biasa <br>

Folder antink-system berisi:

> it24_host/ → folder sumber berisi file seperti kimcun.txt, nafis.txt, test.txt, dll

> antink_mount/ → folder mount hasil dari FUSE

> antink-logs/ → folder log (termasuk it24.log)

> Dockerfile, docker-compose.yml, antink.c → file program utama dan konfigurasi Docker

Perintah ls dilakukan pada antink_mount diluar Docker, akan tetapi hasilnya tidak muncul file apa-apa, meskipun pada container ada isi karena `antink_mount` adalah FUSE mount point, dan sistem FUSE sedang berjalan didalam container Docker, bukan di local system.

<img src = "https://github.com/user-attachments/assets/46074ea4-cca9-43b5-b557-d51f8aac10bc" width = "500"> <br>

`docker exec antink-server cat /antink_mount/[NAMA FILE]` dengan perintah ini, isi file teks normal akan dienkripsi menggunakan ROT13 saat dibaca, sedangkan file teks berbahaya isinya tidak dienkripsi. Semua aktivitas akan disimpan ke file `it24log`  <br>

<img src = "https://github.com/user-attachments/assets/bbb22a7c-9087-402d-b328-b1ea7e1297ee" width = "500"><br>
<img src = "https://github.com/user-attachments/assets/96fa1857-1115-455b-91eb-8ff144e4c513" width = "500"> <br>

Hapus container dengan `docker-compose down`, maka list container kembali ke default.<br>

<img src = "https://github.com/user-attachments/assets/f477cea6-0a3f-4480-9426-e82125190e26" width = "600"> <br>


# Soal 4
```
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
```
Fungsi log_action ini dibuat untuk mencatat aktivitas yang terjadi pada file, misalnya saat file dibuka, dibaca, atau ditulis. Setiap kali fungsi ini dipanggil, dia akan menulis ke file log (disebut LOGFILE) informasi berupa waktu kejadian, jenis aksi (seperti "READ" atau "WRITE"), dan path file yang sedang diakses. Waktu ditulis dalam format tanggal dan jam lengkap, agar mudah dilacak. Jadi, log ini berguna kalau suatu saat butuh melihat apa saja yang terjadi dalam sistem, terutama kalau ada masalah atau mau melakukan pengecekan aktivitas.

```
int read_starter(const char *fpath, char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    int r = pread(fd, buf, size, offset);
    close(fd);
    return r;
}
```
Fungsi read_starter digunakan untuk membaca isi file dari lokasi path tertentu (fpath). Ia akan membuka file tersebut hanya untuk dibaca (O_RDONLY). Kalau gagal dibuka, fungsi akan mengembalikan kode error. Kalau berhasil, fungsi akan membaca sejumlah data sebanyak size byte mulai dari posisi offset, dan hasilnya disimpan ke dalam buffer buf. Setelah selesai membaca, file langsung ditutup. Jadi fungsi ini praktis buat ambil sebagian isi file tanpa harus membuka semuanya dari awal.

```
int write_starter(const char *fpath, const char *buf, size_t size, off_t offset) {
    int fd = open(fpath, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) return -errno;
    int r = pwrite(fd, buf, size, offset);
    close(fd);
    return r;
}
```
Fungsi write_starter bertugas menulis data ke file pada path tertentu (fpath). File akan dibuka dalam mode tulis, dan kalau belum ada, akan dibuat otomatis dengan permission 0644. Jika file gagal dibuka, fungsi langsung mengembalikan error. Kalau berhasil, data dari buffer buf akan ditulis sebanyak size byte ke posisi offset dalam file. Setelah proses selesai, file ditutup dan fungsi mengembalikan hasil jumlah byte yang berhasil ditulis. Ini berguna saat ingin menyimpan sebagian data ke file tanpa menimpa keseluruhan isinya.

```
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
```
Fungsi-fungsi ini digunakan untuk membaca file dari Metro Area dengan mekanisme encoding khusus. Pertama, shift_bytes dan unshift_bytes melakukan manipulasi byte: setiap byte diubah dengan menambahkan atau mengurangi nilai berdasarkan posisinya agar data tidak disimpan secara polos.

Fungsi read_metro membuka file dan membaca datanya ke buffer sementara (tmp). Setelah berhasil dibaca, datanya di-encode menggunakan shift_bytes, lalu hasilnya disalin ke buf. Ini membuat file tetap terbaca dengan benar oleh sistem, tapi sebenarnya disimpan dalam format yang telah dienkode. Setelah selesai, buffer sementara dibebaskan dan file ditutup.

```
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
```
Fungsi pada Dragon Area menggunakan algoritma ROT13, yang diterapkan melalui fungsi rot13. Saat membaca file `read_dragon`, isi file dibaca terlebih dahulu ke dalam buffer sementara, kemudian diproses oleh rot13 sebelum dimasukkan ke buffer tujuan. Saat menulis `write_dragon`, data yang akan ditulis dienkripsi terlebih dahulu menggunakan `rot13`, lalu disimpan ke file.

Sementara itu, fungsi di Blackrose Area `read_blackrose` dan `write_blackrose` menggunakan operasi baca dan tulis standar `pread`, `pwrite` tanpa transformasi data.

```
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
```
Kode ini pakai library OpenSSL buat enkripsi dan dekripsi file pakai AES-256-CBC. Waktu nge-enkripsi, pertama bikin IV (initialization vector) acak 16 byte terus ditulis ke file dulu. Setelah itu, data dienkripsi dengan fungsi dari OpenSSL dan hasilnya ditulis setelah IV. Buat dekripsi, file dibuka terus IV-nya dibaca 16 byte pertama, sisanya data terenkripsi. Data itu didekripsi pakai fungsi OpenSSL dan disimpen ke buffer. Fungsi `read_heaven` sama `write_heaven` cuma manggil fungsi dekripsi dan enkripsi itu biar gampang dipakai.

```
int read_youth(const char *fpath, char *buf, size_t size, off_t offset) {
    gzFile gz = gzopen(fpath, "rb");
    if (!gz) return -errno;
    if (gzseek(gz, offset, SEEK_SET) == -1) {
        gzclose(gz);
        return -EIO;
    }
    int r = gzread(gz, buf, size);
    gzclose(gz);
    return r;
}
int write_youth(const char *fpath, const char *buf, size_t size) {
    size_t out_len = size + size / 10 + 12;  // buffer kompresi lebih besar dari input
    char *out_buf = malloc(out_len);
    if (!out_buf) return -ENOMEM;

    int ret = compress_content(buf, size, out_buf, &out_len);
    if (ret != 0) {
        free(out_buf);
        return ret;
    }

    FILE *fp = fopen(fpath, "wb");
    if (!fp) {
        free(out_buf);
        return -errno;
    }
    fwrite(out_buf, 1, out_len, fp);
    fclose(fp);
    free(out_buf);
    return size;
}
```
Kode read_youth dan `write_youth` ini berfungsi untuk membaca dan menulis file yang menggunakan kompresi `gzip`. Pada fungsi `read_youth`, file dibuka dengan `gzopen` untuk membaca dalam mode `gzip`, lalu posisi baca digeser sesuai offset yang diminta menggunakan `gzseek`. Data kemudian dibaca ke buffer menggunakan `gzread` dan file ditutup dengan gzclose.

```
int youth_compress(const char *src, size_t src_len, char *dest, size_t *dest_len) {
    z_stream zs;
    int status;

    // Initialize stream
    zs.zalloc = NULL;
    zs.zfree = NULL;
    zs.opaque = NULL;

    status = deflateInit2(&zs, 5, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);  // level 5, 31 = 15+16 gzip wrapper
    if (status != Z_OK) return -EIO;

    zs.next_in = (Bytef *)src;
    zs.avail_in = (uInt)src_len;
    zs.next_out = (Bytef *)dest;
    zs.avail_out = (uInt)*dest_len;

    while (zs.avail_in > 0) {
        status = deflate(&zs, Z_NO_FLUSH);
        if (status == Z_STREAM_ERROR) {
            deflateEnd(&zs);
            return -EIO;
        }
    }

    // Finalize compression
    for (;;) {
        status = deflate(&zs, Z_FINISH);
        if (status == Z_STREAM_END) break;
        if (status != Z_OK) {
            deflateEnd(&zs);
            return -EIO;
        }
    }

    *dest_len = zs.total_out;
    deflateEnd(&zs);

    return 0;
}
int youth_decompress(const char *src, size_t src_len, char *dest, size_t *dest_len) {
    z_stream zs;
    int status;

    zs.zalloc = NULL;
    zs.zfree = NULL;
    zs.opaque = NULL;

    status = inflateInit2(&zs, 47);  // 47 = 32 + 15, auto detect gzip/zlib with windowBits
    if (status != Z_OK) return -EIO;

    zs.next_in = (Bytef *)src;
    zs.avail_in = (uInt)src_len;
    zs.next_out = (Bytef *)dest;
    zs.avail_out = (uInt)*dest_len;

    while (1) {
        status = inflate(&zs, Z_NO_FLUSH);

        if (status == Z_STREAM_END) break;
        if (status != Z_OK) {
            inflateEnd(&zs);
            return -EIO;
        }
        if (zs.avail_out == 0) {
            // Output buffer penuh, stop to prevent overflow
            break;
        }
    }

    *dest_len = zs.total_out;
    inflateEnd(&zs);

    return 0;
}
```
Kode ini berisi dua fungsi utama untuk kompresi dan dekompresi data menggunakan library `zlib` dengan format `gzip`. Fungsi `youth_compress` menginisialisasi struktur `z_stream` lalu menjalankan proses kompresi data dari buffer sumber `src` ke buffer tujuan `dest`. Proses kompresi dilakukan dalam loop sampai seluruh data masuk telah diproses, kemudian diakhiri dengan `deflate` bertipe `Z_FINISH`. Ukuran hasil kompresi disimpan dalam `*dest_len`.

Fungsi youth_decompress juga menginisialisasi `z_stream`, tapi untuk melakukan dekompresi data dari buffer `src` ke buffer `dest`. Fungsi ini memakai `inflateInit2` dengan parameter 47 agar dapat mendeteksi format `gzip` atau `zlib` secara otomatis. Data didekompresi dalam loop sampai akhir stream atau buffer output penuh. 

Fungsi diatas merupakan bentuk revisi dari kode sebelumnya sehingga sekarang penambahan fungsi compress dan decompress berhasil. Hasil sebelumnya dimana bentuk file zip tidak bisa di unzip sekarang sudah bisa.

```
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
```
Kode ini terdiri dari dua bagian utama. Pertama, fungsi `is_valid_chiho` yang berfungsi sebagai pengecek validitas direktori utama `chiho` dengan membandingkan input terhadap daftar direktori yang diizinkan: "starter", "metro", "dragon", "blackrose", "heaven", "youth", dan "7sref". Fungsi ini mengembalikan nilai boolean (1 jika valid, 0 jika tidak).

Kedua, fungsi get_real_path yang bertugas mengonversi path virtual menjadi path fisik yang sesuai di sistem file. Fungsi ini memulai dengan mengekstrak nama chiho dari path menggunakan sscanf dan memvalidasinya melalui `is_valid_chiho`. Jika chiho tidak valid, fungsi langsung mengosongkan string hasil agar menandakan path tidak sah.
```
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
```
Kode ini terdiri dari dua bagian utama. Pertama, fungsi `is_valid_chiho` yang berfungsi sebagai pengecek validitas direktori utama (chiho) dengan membandingkan input terhadap daftar direktori yang diizinkan: "starter", "metro", "dragon", "blackrose", "heaven", "youth", dan "7sref". Fungsi ini mengembalikan nilai boolean (1 jika valid, 0 jika tidak).

Kedua, fungsi `get_real_path` yang bertugas mengonversi path virtual menjadi path fisik yang sesuai di sistem file. Fungsi ini memulai dengan mengekstrak nama chiho dari path menggunakan sscanf dan memvalidasinya melalui `is_valid_chiho`. Jika chiho tidak valid, fungsi langsung mengosongkan string hasil agar menandakan path tidak sah.

```
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
```
Kode fungsi dispatch_write berfungsi sebagai pengarah `dispatcher` yang menentukan fungsi penulisan `write` mana yang harus dipanggil berdasarkan path file yang diberikan.

Pertama, fungsi ini mengubah path virtual yang diterima menjadi path fisik dengan memanggil `get_real_path`. Hasil path fisik disimpan dalam variabel `fpath`.

Setelah itu, fungsi memeriksa bagian tertentu dari path asli `path` dengan menggunakan `strstr` untuk mencari substring yang mengindikasikan jenis direktori, seperti "/starter/", "/metro/", "/dragon/", dan seterusnya.

```
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
```
Fungsi pertama, `fs_getattr`, bertugas mengambil atribut dari sebuah file atau direktori yang diberikan lewat `path`. Di dalamnya, path virtual diubah menjadi path fisik dengan `get_real_path`, lalu fungsi mencatat aksi GETATTR menggunakan `log_action`. Setelah itu, fungsi memanggil `lstat` untuk mendapatkan informasi atribut file.

Fungsi kedua, `fs_readdir`, bertugas membaca isi sebuah direktori. Path virtual juga diubah ke path fisik dengan `get_real_path`. Kemudian direktori tersebut dibuka dengan `opendir`. Jika gagal, fungsi mengembalikan kode error yang sesuai. Jika berhasil, fungsi menambahkan dua entri khusus `.` direktori saat ini dan `..` direktori induk ke buffer hasil dengan `filler`. Selanjutnya, fungsi membaca semua isi direktori satu per satu menggunakan `readdir`. Nama-nama file atau direktori yang dimulai dengan titik `.` diabaikan, agar file tersembunyi tidak ditampilkan.

```
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
```
Fungsi pertama, `fs_open`, bertugas membuka file sesuai path yang diberikan. Pertama, path virtual diterjemahkan ke path fisik menggunakan `get_real_path`. Kemudian fungsi mencoba membuka file dengan flag akses yang diberikan oleh FUSE melalui `fi->flags`. Jika gagal membuka file, fungsi mengembalikan kode error yang sesuai.

Fungsi kedua, `fs_read`, menangani pembacaan isi file. Ia mencatat aksi READ di log, lalu meneruskan operasi baca ke fungsi `dispatch_read`, yang bertugas melakukan pembacaan sebenarnya sesuai tipe file atau direktori yang diakses. Fungsi ini menerima parameter buffer, ukuran data yang dibaca, dan offset posisi baca.

```
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
```
Fungsi pertama, `fs_create`, berfungsi membuat file baru. Pertama, path virtual diterjemahkan ke path fisik dengan `get_real_path`. Kemudian fungsi memanggil creat untuk membuat file dengan mode akses sesuai parameter. Jika gagal membuat file, fungsi mengembalikan kode error. Dan jika berhasil maka akan certatat dalam log sebagai activity create.

Fungsi kedua, `fs_unlink`, digunakan untuk menghapus file. Setelah path diubah menjadi path fisik, fungsi memanggil unlink untuk menghapus file tersebut. Activity `unlink` dicatat di log, kemudian fungsi mengembalikan hasil operasi `unlink` yang bernilai `0` jika sukses atau kode error jika gagal.

Fungsi ketiga, `fs_utimens`, berfungsi memperbarui waktu akses dan modifikasi file. Setelah path diterjemahkan dan dicek agar tidak kosong, fungsi mencatat activity `utimens` di log lalu memanggil utimensat untuk mengatur waktu file sesuai array `tv` yang berisi waktu akses dan modifikasi baru.

### Screenshot isi starter chiho
![image](https://github.com/user-attachments/assets/fdf6e29b-8804-42fe-bc70-1569c8b2d7d1)
### Screenshot isi dragon chiho
![image](https://github.com/user-attachments/assets/ae25abaa-bca0-4f79-af8d-c70840801714)
### Screenshot isi metro chiho
![image](https://github.com/user-attachments/assets/23e03840-9501-4834-bd3a-6bbe0f8f8a14)
### Screenshot isi blackrose chiho
![image](https://github.com/user-attachments/assets/cdc537ba-f7e7-424b-a46b-f798974e8e4d)
![image](https://github.com/user-attachments/assets/e75e161a-170c-4cca-b4ab-16582033b5d6)





