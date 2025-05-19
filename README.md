# Sisop-4-2025-IT26
| Nama                         | Nrp        |
| ---------------------------- | ---------- |
| Azaria Raissa Maulidinnisa   | 5027241043 |
| Oscaryavat Viryavan          | 5027241053 |
| Naufal Ardhana               | 5027241118 |

# Soal 1
# Soal 2
# Soal 3

Cara melihat daftar container `docker ps -a` <br>

### Cara compile soal ini.<br>
Jalankan Docker Compose dengan `docker-compose build --no-cache` lalu `docker-compose up`<br>
Buka terminal baru lalu jalankan `docker exec -it antink-server ls /antink_mount` untuk melihat semua file baik file berbahaya yang di-reverse maupun file biasa <br>
`docker exec antink-server cat /antink_mount/[NAMA FILE]` dengan command ini, file teks normal akan di enkripsi menggunakan ROT13 saat dibaca, sedangkan file teks berbahaya tidak di enkripsi.  <br>
Hapus container dengan `docker-compose down` <br>

# Soal 4
