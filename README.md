# BÁO CÁO BÀI TẬP LỚN: CÀI ĐẶT LỆNH `ls` (UNIX/LINUX)
## HỌC PHẦN: LẬP TRÌNH HỆ THỐNG (SYSTEM PROGRAMMING)

---

## 📌 THÔNG TIN SINH VIÊN
- **Họ và tên:** Nguyễn Thị Thúy Hoan
- **Mã số sinh viên (MSSV):** 24IT074
- **Đề tài:** Cài đặt lại lệnh `ls` (List Directory Contents) trên hệ điều hành UNIX/Linux bằng ngôn ngữ C.

---

## 📖 GIỚI THIỆU TỔNG QUAN
Dự án nhằm mục đích hiện thực lại tiện ích dòng lệnh chuẩn `ls(1)` của hệ điều hành UNIX/Linux tuân theo chuẩn POSIX / Single UNIX Specification. Chương trình cho phép liệt kê thông tin về các file và thư mục trong hệ thống tệp, hỗ trợ các định dạng hiển thị, cơ chế lọc, sắp xếp đa dạng và truy vấn sâu thông tin siêu dữ liệu (metadata) của tệp tin thông qua các System Calls.

---

## 🚀 TÍNH NĂNG VÀ CÁC TÙY CHỌN (OPTIONS) HỖ TRỢ

Chương trình hỗ trợ đầy đủ 19 tùy chọn dòng lệnh phổ biến và khả năng kết hợp các cờ (clustering flags ví dụ: `ls -la`, `ls -lSh`, `ls -lR`):

| Cờ (Option) | Tên tính năng | Mô tả chi tiết |
| :---: | :--- | :--- |
| `-a` | All files | Hiển thị tất cả các tệp, bao gồm cả các tệp ẩn bắt đầu bằng dấu chấm (`.`). |
| `-A` | Almost all | Tương tự `-a`, nhưng loại bỏ hai mục đặc biệt là thư mục hiện tại (`.`) và thư mục cha (`..`). |
| `-l` | Long format | Liệt kê chi tiết ở định dạng dài: kiểu tệp & quyền hạn, số hard link, owner, group, kích thước, thời gian sửa đổi và tên tệp. |
| `-n` | Numeric UID/GID | Định dạng dài tương tự `-l`, nhưng hiển thị UID và GID dưới dạng số thay vì chuỗi tên người dùng/nhóm. |
| `-h` | Human-readable | Hiển thị kích thước tệp và số block bằng các đơn vị dễ đọc (B, K, M, G, T). |
| `-k` | Kilobytes | Hiển thị kích thước theo đơn vị Kilobytes (1024 bytes). |
| `-i` | Inode number | Hiển thị số chỉ mục inode của tệp/thư mục. |
| `-s` | Block count | Hiển thị số block đĩa được cấp phát cho mỗi tệp. |
| `-d` | Directories as files | Xem thư mục như tệp thông thường, không liệt kê nội dung bên trong thư mục. |
| `-R` | Recursive | Duyệt và liệt kê đệ quy toàn bộ cây thư mục con. |
| `-r` | Reverse sort | Đảo ngược thứ tự sắp xếp kết quả. |
| `-t` | Sort by time | Sắp xếp theo mốc thời gian gần nhất (mặc định là `mtime`). |
| `-S` | Sort by size | Sắp xếp theo kích thước tệp giảm dần (tệp lớn nhất đứng đầu). |
| `-c` | Status change time | Sử dụng thời gian thay đổi trạng thái inode (`ctime`) để sắp xếp (`-t`) hoặc hiển thị (`-l`). |
| `-u` | Access time | Sử dụng thời gian truy cập cuối (`atime`) để sắp xếp (`-t`) hoặc hiển thị (`-l`). |
| `-f` | Do not sort | Không thực hiện sắp xếp danh sách (tự động bật cờ `-a`). |
| `-F` | File indicator | Thêm ký hiệu nhận diện sau tên: `/` (thư mục), `*` (file thực thi), `@` (symlink), `=` (socket), `\|` (FIFO). |
| `-q` | Replace non-printable | Thay thế các ký tự không in được trong tên tệp bằng ký tự `?` (mặc định khi xuất ra terminal). |
| `-w` | Raw characters | Xuất nguyên gốc các ký tự không in được (mặc định khi pipe/chuyển hướng luồng). |
| `--` | End of options | Đánh dấu kết thúc danh sách tùy chọn; các đối số tiếp theo được coi là đường dẫn tệp/thư mục. |

---

## 📂 CẤU TRÚC MÃ NGUỒN (PROJECT ARCHITECTURE)

Dự án được tổ chức theo kiến trúc module hóa rõ ràng, dễ bảo trì và mở rộng:

```text
├── Makefile             # Kịch bản biên dịch tự động (gcc, cờ -Wall, -Wextra, -pedantic, -std=c11)
├── README.md            # Tài liệu báo cáo tóm tắt dự án
├── ls.h                 # File header: định nghĩa struct Options, FileEntry và prototype các hàm
├── main.c               # Điểm khởi đầu chương trình: kiểm tra tham số, phân tách file/thư mục
├── options.c            # Xử lý phân tích cú pháp các tùy chọn dòng lệnh (parse options)
├── list.c               # Xử lý logic duyệt thư mục, lọc tệp, đệ quy (-R)
├── sort.c               # Thuật toán sắp xếp đa tiêu chí (theo tên, thời gian, kích thước, đảo ngược)
├── format.c             # Định dạng hiển thị dữ liệu (long format, quyền hạn, dung lượng, symlink)
└── util.c               # Các hàm tiện ích bổ trợ (sao chép chuỗi an toàn, in ký tự kiểm soát)
```

### Chi tiết các module:
1. **`ls.h`**:
   - Khai báo cấu trúc dữ liệu trung tâm `Options` lưu trữ trạng thái các cờ.
   - Định nghĩa cấu trúc `FileEntry` lưu trữ thông tin tệp (`name`, `fullpath`, `struct stat`).
2. **`main.c`**:
   - Khởi tạo giá trị mặc định dựa theo môi trường chạy (`isatty`).
   - Xử lý quyền hạn siêu người dùng (`getuid() == 0` tự động kích hoạt `-A`).
   - Phân loại và sắp xếp riêng biệt các đối số là file và thư mục theo chuẩn UNIX.
3. **`options.c`**:
   - Phân tích chuỗi tham số dòng lệnh.
   - Xử lý các cặp tùy chọn ghi đè lẫn nhau (`-c` & `-u`, `-h` & `-k`, `-l` & `-n`, `-q` & `-w`).
4. **`list.c`**:
   - Sử dụng `opendir()`, `readdir()`, `closedir()` để quét thư mục.
   - Thu thập thông tin file thông qua System Call `lstat()`.
   - Tính toán và in dòng tổng số block (`total`) cho thư mục.
   - Thực thi đệ quy với cờ `-R` (tránh vòng lặp vô tận bằng cách bỏ qua `.` và `..`).
5. **`sort.c`**:
   - Sử dụng hàm chuẩn `qsort()` với các hàm so sánh tùy biến.
   - Sắp xếp theo tên (lexicographical), thời gian (`ctime`, `atime`, `mtime`), kích thước (`st_size`).
6. **`format.c`**:
   - Phân tích quyền hạn tập tin (file mode bits: `S_IRUSR`, `S_IWUSR`, `S_IXUSR`,...) cùng các bit đặc biệt (**SUID**, **SGID**, **Sticky bit**).
   - Truy vấn thông tin người dùng/nhóm qua `getpwuid()` và `getgrgid()`.
   - Định dạng kích thước tệp linh hoạt (`format_size`, `format_blocks`).
   - Đọc và hiển thị đích đến của liên kết mềm (symlink) qua `readlink()`.
7. **`util.c`**:
   - Các tiện ích kiểm tra ký tự in được (`isprint`), xử lý an toàn bộ nhớ đệm chuỗi.

---

## 🛠️ CÁC SYSTEM CALLS VÀ THƯ VIỆN ĐÃ SỬ DỤNG

Chương trình khai thác sâu các giao diện lập trình hệ thống cấp thấp của UNIX:
- **Quản lý thư mục & File:** `lstat()`, `stat()`, `opendir()`, `readdir()`, `closedir()`, `readlink()`.
- **Quản lý User & Group:** `getpwuid()`, `getgrgid()`, `getuid()`.
- **Quản lý Terminal:** `isatty()`.
- **Xử lý thời gian:** `time()`, `localtime()`, `strftime()`.

---

## ⚙️ HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY THỬ NGHIỆM

### 1. Yêu cầu môi trường
- Hệ điều hành: Linux (Ubuntu, Debian, Fedora,...) hoặc môi trường POSIX (WSL, macOS, MSYS2/MinGW).
- Trình biên dịch: `gcc` hỗ trợ chuẩn C11.
- Công cụ: `make`.

### 2. Biên dịch
Mở terminal tại thư mục dự án và thực hiện lệnh:
```bash
# Biên dịch chương trình
make

# Hoặc xóa các file đối tượng cũ rồi biên dịch mới
make clean && make
```

### 3. Ví dụ chạy chương trình
```bash
# 1. Liệt kê cơ bản
./ls

# 2. Liệt kê chi tiết định dạng dài bao gồm cả file ẩn
./ls -la

# 3. Hiển thị dung lượng dạng đọc được (Human-readable) và sắp xếp theo kích thước
./ls -lhS

# 4. Hiển thị số inode và số block đĩa
./ls -lis

# 5. Liệt kê đệ quy kèm chỉ báo loại file
./ls -RF

# 6. Hiển thị thông tin dạng dài với UID/GID dạng số
./ls -n

# 7. Sắp xếp theo thời gian truy cập gần nhất (ngược thứ tự)
./ls -lutr

# 8. Liệt kê thư mục hiện tại dưới dạng file (không vào trong thư mục)
./ls -ld .
```

---

## 📊 KẾT QUẢ ĐẠT ĐƯỢC
- Chương trình chạy ổn định, mô phỏng chính xác hành vi của lệnh `ls` tiêu chuẩn trên hệ thống UNIX/Linux.
- Quản lý bộ nhớ chặt chẽ, giải phóng tài nguyên sau khi sử dụng (`closedir`, `free`).
- Kiểm tra và xử lý lỗi kỹ lưỡng (`errno`, thông báo lỗi tương thích chuẩn hệ điều hành).
- Kiến trúc module hóa sạch sẽ, đạt chuẩn mã nguồn C hiện đại.
