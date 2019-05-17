#include "tcp_client_optional_mode.h"

#if 0
//定时器设置函数，成功返回timerfd
int timer_init(int timer_val, int timer_interval)
{
	struct itimerspec new_value;
	struct timespec now_time;
	//获得相对时间
	clock_gettime(CLOCK_MONOTONIC, &now_time);

	new_value.it_value.tv_sec = timer_val;
	new_value.it_value.tv_nsec = 0;
	new_value.it_interval.tv_sec = timer_interval;
	new_value.it_interval.tv_nsec = 0;

	int timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
	if(timerfd == -1)
	{
		handle_error("timerfd_create");
	} 
	printf("timerfd create success!\n");
	timerfd_settime(timerfd, 0, &new_value, NULL);
	return timerfd;
}

//定时器关闭函数
void timer_destroy(int timer_fd)
{
	struct itimerspec new_time;
	new_time.it_value.tv_sec = 0;
	new_time.it_value.tv_nsec = 0;
	new_time.it_interval = new_time.it_value;
	
	timerfd_settime(timer_fd, 0, &new_time, NULL);
}
#endif

//发送数据的处理函数
struct send_status *send_handler(struct send_status *save_data)
{
	ssize_t read_ret;
	ssize_t sent_ret;
	char buf[1024];
	char file_name[1024];

	memset(buf, 0, sizeof(buf));
	read_ret = fread(buf, 1, sizeof(buf), save_data->fd);
	if(read_ret == 0)
	{
		fclose(save_data->fd);
		snprintf(file_name, sizeof(file_name), "%d-%d.json",
								save_data->thread_num,save_data->file_num);
		save_data->fd = fopen(file_name, "r");

		if(save_data->fd == NULL)
		{
			printf("open %s file failure:\n", file_name);
			perror("");
			save_data->send_state = 0;
			return save_data;
		}
		printf("open file %s\n",file_name);
		save_data->file_num ++;
		return save_data;
	}

	sent_ret = send_from_read(save_data->socket_fd, buf, read_ret);
	if(sent_ret < 0)
	{
		handle_error("send msg");
	}
	
	printf("send bytes %ld\n", sent_ret);
	
	return save_data;
}

#if 0
//SIG信号处理函数
void signal_handler(int signal_num)
{
	switch(signal_num){
		case SIGPROF:
			send_all_handler();			
			break;
	}
	
}
#endif

//打开要发送的文件
FILE * open_file(int num)
{
	char file_name[32];
	
	memset(file_name, 0, sizeof(file_name));
	snprintf(file_name, sizeof(file_name), "%d-1.json", num);

	FILE *fd = fopen(file_name,"r");
	if(fd == NULL)
	{
		printf("open file %s:\n", file_name);
		perror("");
		return NULL;
	}
	else
	{		
		return fd;
	}
}

//发送读取到的数据
ssize_t send_from_read(int socket_fd, char *buf, ssize_t size)
{
	ssize_t sent_bytes = 0;	
	if(size < 128)
	{
		char head[2] = {48, size};
		char send_msg[sizeof(head) + size];

		memset(send_msg, 0, sizeof(send_msg));
		memcpy(send_msg, head, 2);
		memcpy(send_msg+2, buf, size);
		
		sent_bytes = send(socket_fd, send_msg, sizeof(send_msg), 0);
		if(sent_bytes < 0)
		{
			perror("sent fail!\n");
			return -1;
		}	
		
		return sent_bytes;
	}
	
	else
	{
		char head[3] = {48, 128 + size%128, size/128};
		char send_msg[sizeof(head) + size];
		
		memset(send_msg, 0, sizeof(send_msg));		
		memcpy(send_msg, head, 3);
		memcpy(send_msg+3, buf, size);

		sent_bytes = send(socket_fd, send_msg, sizeof(send_msg), 0);
		if(sent_bytes < 0)
		{
			perror("sent fail!\n");
			return -1;
		}
		
		return sent_bytes;
	}
}
//循环发送.json中的数据
struct send_status *send_all(struct send_status *save_data)
{
	if(save_data->send_state == 0)
	{
		return save_data;
	}	

	else if(save_data->fd == NULL){
		save_data->fd = open_file(save_data->thread_num);
		if(save_data->fd == NULL)
		{
			save_data->send_state = 0;
			return save_data;
		}	
	}
	else if(save_data->fd == NULL)
	{
		save_data->send_state = 0;
		return save_data;
	}

	save_data = send_handler(save_data);

	return save_data;
}
#if 0
//接受数据
void *recv_msg(void *socket_fd)
{
	int sock_fd = (int)socket_fd;
	char buf[1024] = {'\0'};

	while(1)
	{	
		memset(buf, 0, sizeof(buf));
		read(sock_fd, buf, sizeof(buf));
		printf("%d recv msg:%s\n", sock_fd, buf);
//		while(1){}
	}
	printf("recv_over\n");
	return NULL;
}
#endif
//获取ipconfig.txt中的ip和port
struct ip_port read_ip_port(void)
{
	struct ip_port a;
	FILE *fd = fopen("./ipconfig.txt", "r");
	if(fd == NULL)
	{
		perror("open ipconfig.txt!\n");
		exit(1);
	}
//	long offset = num*sizeof(struct ip_port);
//	fseek(fd,offset,SEEK_SET);
	
	fscanf(fd, "%s%d", a.ip, &a.port);
	printf("%s\n%d\n", a.ip, a.port);
	fclose(fd);
	
	return a;
}


//连接到服务器

int connect_to_server(int socket_fd,int thread_num)
{
	struct ip_port p = read_ip_port();
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(p.port);
	server_addr.sin_addr.s_addr = inet_addr(p.ip);
	
	if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("connect error !\n");
		return -1;
	}
	
	printf("%d connect to server success!\n", thread_num);
	
	return 0;
}


//模式2连接服务器

int connect_to_server_mode2()
{
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct ip_port p = read_ip_port();
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(p.port);
	server_addr.sin_addr.s_addr = inet_addr(p.ip);
	
	if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("connect error !\n");
		return -1;
	}
	
	printf("connect to server success!\n");
	return socket_fd;

}



//线程执行函数
void *handle(void *thr_num)
{	
	int i = (int)thr_num;

	struct mg_mgr mgr;
	struct mg_connection *c;
	char tcp_ip_port[256] = {'\0'};

	struct ip_port p = read_ip_port();

	snprintf(tcp_ip_port, sizeof(tcp_ip_port), "tcp://%s:%d", p.ip, p.port);

	mg_mgr_init(&mgr, NULL);
	
	c = mg_connect(&mgr, tcp_ip_port, ev_handler);

	struct send_status save_data;
	save_data.socket_fd = c->sock;
	save_data.file_num = 2;
	save_data.send_state = 1;
	save_data.fd = NULL;
	save_data.thread_num = i;
	
	c->save_data = &save_data;
	c->mode = 1;
	if (c == NULL) {
		printf("Cannot start on port %s\n", tcp_ip_port);
		return NULL;
	}

	mg_set_timer(c, mg_time() + 1);

	printf("Starting on port %s, time: %.2lf\n", tcp_ip_port, mg_time());

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return NULL;
}

void mode2_handler(struct mg_connection *nc)
{
	char file_name[1024];
	char buf[1024];
	ssize_t read_ret;
	ssize_t sent_ret;
	FILE *fd;
	if(nc->save_data->fd == NULL){
		printf("请输入文件名:");
		memset(file_name, 0, 1024);
		scanf("%s", file_name);
			
		fd = fopen(file_name, "r");
		if(fd == NULL)
		{
			perror("open file!\n");
			return;
		}
	}
					
	memset(buf, 0, sizeof(buf));
	read_ret = fread(buf, 1, sizeof(buf), fd);
	if(read_ret != 0){
		sent_ret = send_from_read(nc->sock, buf, read_ret);
		if(sent_ret < 0)
		{
			fclose(fd);
			return;
		}
		nc->save_data->fd = fd;
		printf("send bytes %ld\n", sent_ret);
	}

	else if(read_ret == 0)
	{
		fclose(fd);
		nc->save_data->fd = NULL;
	}
	return;
}

//ev_handler
void ev_handler(struct mg_connection *c, int ev, void *ev_data){
	
	struct send_status *save_data;
	save_data = c->save_data;

	
	switch (ev) {
		case MG_EV_RECV:
			fprintf(stderr, "thread %d recv:%.*s\n", save_data->thread_num, (int) c->recv_mbuf.len, c->recv_mbuf.buf);
			c->recv_mbuf.len = 0;
			break;
    	case MG_EV_TIMER: {
			double now = *(double *) ev_data;
			double next = mg_set_timer(c, 0) + 1;
			printf("timer event, current time: %.2lf\n", now);

			if(c->mode == 1){
				save_data = send_all(save_data);
				mg_set_timer(c, mg_time() + next);  // Send us timer event again after 1 seconds
				break;
				}
			else if(c->mode == 2){
				mode2_handler(c);		
				mg_set_timer(c,mg_time() + next);
				break;
			}
		}
	}
}




