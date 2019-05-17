#ifndef TCP_CLIENT_OPTIONAL_MODE_H
#define TCP_CLIENT_OPTIONAL_MODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include "mongoose/mongoose.h"
#define handle_error(msg)\
			do{ perror(msg); exit(EXIT_FAILURE); }while(0)



struct ip_port
{
	char ip[32];
	int port;
};


void ev_handler(struct mg_connection *c, int ev, void *ev_data);
//发送读取到的数据
ssize_t send_from_read(int socket_fd, char *buf, ssize_t size);

//发送数据处理函数
struct send_status *send_handler(struct send_status *save_data);

//打开第一个文件
FILE * open_file(int num);

//发送所有数据
struct send_status *send_all(struct send_status *save_data);

//接受数据
void *recv_msg(void *socket_fd);

//读取ipconfig中的ip配置
struct ip_port read_ip_port(void);

//连接服务器(已弃用)
int connect_to_server(int socket_fd,int thread_num);

//模式2连接服务器
int connect_to_server_mode2();

//线程执行函数
void *handle(void *thr_num);

void mode2_handler(struct mg_connection *nc);
#endif




































