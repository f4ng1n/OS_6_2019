# OS_6_2019
# SYNCHRONIZATION OF PROCESSES WITH THE HELP OF NAMED SEMAPHORES
## Mục đích bài lab:   
Làm quen với công cụ đồng bộ hóa các processes - semaphore có tên (named semaphore) và các lệnh hệ thống, đảm bảo việc create, close và destroy named-semaphore, cũng như capture và release named-semaphore.  
## Lý thuyết cơ bản  
 **Named-semaphore** cho phép tổ chức đồng bộ hóa processes (tiến trình) trong hệ điều hành. Do trong thực tế, khi tạo (create) và đóng (close) nó, chúng ta truyền tên cho nó - chuỗi các ký tự, 2 processes có cơ hội đưa một con trỏ đến cùng một semaphore. Có nghĩa là, khác với mutex và unnamed-semaphore, các semaphores định danh có thể phối hợp truy cập đến vùng tài nguyên quan trọng (Critical Resources) không chỉ ở mức độ của vài threads của một chương trình, mà ở mức độ của một vài tiến trình (processes), thực hiện chương trình.  
 
  Trong hệ thống, loại semaphore này thực hiện như một dạng tập tin đặc biệt, thời gian tồn tại (lifetime) của nó không bị giới hạn bởi lifetime của process tạo ra nó.  
  
  Các giao diện (interfaces) phổ biến để tạo (create) named-semaphore là:  
  * interface POSIX (Portable Operating System Interface) - giao diện hệ điều hành di động - bộ tiêu chuẩn mô tả các giao diện giữa hệ điều hành và chương trình ứng dụng (Giao diện lập trình ứng dụng API hệ thống) [https://ru.wikipedia.org/wiki/POSIX]);  
  * interface SVID (System V Interface Definition)  tiêu chuẩn, mô tả hành vi hệ điều hành UNIX. (https://ru.wikipedia.org/wiki/System_V_Interface_Definition)  
  Trong bộ tiêu chuẩn POSIX, named-semaphore được tạo ra bằng lệnh sau:
````
sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value),
````
trong đó:  
`name` – tên semaphore;   
`oflag` – cờ (flag), quản lý việc tạo một semaphore.  Khi tạo một semaphore, phải chỉ định một cờ O_CREAT.  
`mode` – quyền truy cập vào semaphore, có thể được thiết lập, ví dụ 0644.  
`value` – trạng thái bắt đầu của semaphore.  

Named-semaphore  bị đóng (close) bằng lệnh sau:  
`int sem_close(sem_t *sem)`  
Khi đi vào vùng quan trọng (CS) cần gọi hàm sau:  
`int sem_wait(sem_t *sem)`  
Khi đi ra khỏi vùng quan trọng (CS) cần gọi hàm sau:  
`int sem_post(sem_t *sem)`  
Semaphore định danh bị xóa (release) bởi lệnh sau:  
`int sem_unlink(const char *name)`  

Trong chuẩn SVID, semaphore định danh được tạo ra bởi lệnh:  
`int semget(key_t key, int nsems, int semflg);`  
trong đó:  
`key_t key` - chìa khóa (key) để tạo một đối tượng duy nhất;  
`int nsems` – số lượng những semaphores được tạo ra;  
`int semflg` -   các cờ (flags) để quản lý truy cập vào semaphore.  
Khóa (key) có thể dùng hàm sau:  
`key_t ftok(const char *pathname, int proj_id);`  
trong đó :  
`const char *pathname` – tên của file tồn tại;  
`int proj_id` – identifier định danh của project.  

Khi chỉ định vào 2 chương tình có cùng một tên file và chung id project, thì trả về 2 chương trình đó cùng một key.  

Capture và Release semaphore được thực hiện bằng 1 hàm có dạng sau:  
`int semop(int semid, struct sembuf *sops, unsigned nsops);`  
trong đó:  
`int semid` – id semaphore, trả bằng hàm semget();  
`struct sembuf *sops` – con trỏ vào cấu trúc, định nghĩa xác định các hành vi (operation) thực hiện với semaphore;  
`unsigned nsops` – số lượng các hành vi (operation).  

Cấu trúc `struct sembuf` có dạng sau:  
````
struct sembuf {
short sem_num;
short sem_op;
short sem_flg;
};
````
trong đó:  
`short sem_num` – số của semaphore, mà trên đó thực hiện hành vi;  
`short sem_op` – loại hành vi;  
`short sem_flg` – các cờ của hành vi (flags of operations).  

Có 3 loại hành vi có thể thực hiện trên semaphore:  
1. Nếu `sem_op=0` thì process sẽ đợi đến khi semaphore được thiết lập lại (reset);  
2. Nếu `sem_op>0` thì giá trị hiện tại của semaphore tăng lên vào giá trị của sem_op;  
3. Nếu `sem_op<0` thì process sẽ đợi đến khi giá trị của semaphore không được đặt hoặc là bằng giá trị tuyệt đối của `sem_op`. Sau đó, giá trị tuyệt đối `sem_op` được tính toán từ giá trị của semaphore.  

Đặt giá trị semaphore =0 nghĩa là, tài nguyên được giải phóng; còn giá trị 1 nghĩa là tài nguyên đang bận.  

* Tạo cấu trúc theo dạng:  
````
struct sembuf lock[2] = {
0,0,0, //ждать обнуления семафора
0,1,0 //увеличить значение семафора на 1
};
````
Sau đó hành động tiếp theo: capture tài nguyên (resouce):
`semop(semid,&lock[0],2);`  

* Nhập cấu trúc theo dạng:  
````
struct sembuf unlock[1] = {
0,-1,0, //обнулить семафор
};
````
Sau đó hành động tiếp theo: release tài nguyên.  
`semop(semid,&unlock[0],1);`  

* Cho đến khi kết thúc công việc với semaphore, cần phải destroy bằng hàm:  
`int semctl(int semid, int semnum, int cmd);`  
trong đó:  
`int semid` – id hủy nhóm semaphores;  
`int semnum` – số của semaphore, bị bỏ qua (is ignored) cho command IPC_RMID;  
`int cmd` – command, trong trường hợp xóa, sẽ lấy giá trị của IPC_RMID.  

## Yêu cầu công việc  
Trên cơ sở sử dụng named-semaphores để phối hợp tương tác các processes, cần thực hiện viết 2 chương trình.  
Cần chọn một tài nguyên chung - file dùng để ghi dữ liệu từ 2 chương trình.  
Cả 2 chương trình cần phải create (hoặc close, nếu created) một và chỉ một named-semaphore, cần phải create (hoặc close, nếu created) một và chỉ đúng file, và ghi vào file những ký tự khác nhau.  
Cần phải chắc chắn răng, khi semaphore vắng mặt, các tiến trình đưa ra các ký tự vào file theo thứ tự ngẫu nhiên, ví dụ:  
1212121212121212121212121212121212121212121212121212121212121.  
Trong trường hợp sử dụng named-semaphore, các tiến trình đưa ra màn hình các ký tự theo thứ tự nhất định, ví dụ:  
111111111122222222221111111111222222222211111111112222222222.  
Sử dụng hàm vào vùng quan trọng (CS) cùng khóa (block) và không khóa (unblock).  
Sử dụng chuẩn POSIX để phối hợp truy cập vào tài nguyên.  
Mẫu:  
````
declare a flag to end the thread;
declare the identifier of the named semaphore;
declare a file descriptor;
thread function ()
{
declare a character type variable and assign it a value of ‘1’;
yet (the thread termination flag is not set)
{
capture the named semaphore;
in a loop to perform several times
{
output a character to a file;
delay for a while;
}
release the named semaphore;
delay for a while;
}
}
main program ()
{
create (or open if exists) a named semaphore;
create (or open, if exists) a file;
create a stream from the thread function;
wait for key presses;
set the end of thread flag;
wait for the thread to complete;
close file;
close named semaphore;
delete the named semaphore;
}
````











