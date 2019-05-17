#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

//定时器初始化函数
void timer_init()

//信号处理函数
//void signal_handler(int signal_num);

//发送所有数据的处理函数
void send_all_handler(int socket_fd, int thread_num);
/*打开文件
 *num 线程编号
 *成功返回文件描述符（fd），失败返回NULL
*/
FILE * open_file(int num);

/*发送读取到的数据
 *buf 需要发送的字符数组 size 字符个数
 *成功返回发送个数，失败返回-1
*/		
ssize_t send_from_read(int socket_fd, char *buf, ssize_t size);

/*循环发送.json中的数据
 *socket_fd 套接字，num 线程编号
 *成功返回0，失败返回-1
 */
int send_all(int socket_fd, int thread_num);

/*接受数据
 *socket_fd 套接字
 *无返回值
 */
void *recv_msg(void *socket_fd);

/*获取ipconfig.txt中的ip和port
 *无参数
 *返回之为struct ip_port
*/
struct ip_port read_ip_port(void);

/*连接到服务器
 *thread_num 线程编号
 *成功返回0， 失败返回-1
*/
int connect_to_server(int socket_fd,int thread_num);

/*模式2连接服务器
 *无参数
 *成功返回socket_fd,return failed:-1
*/
int connect_to_server_mode2();

/*线程执行函数
 *arg线程序号
 *无返回值
*/
void *handle(void *arg);






#endif
