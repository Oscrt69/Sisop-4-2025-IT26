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
