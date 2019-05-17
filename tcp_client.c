#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

struct ip_port
{
	char ip[32];
	int port;
};


/*循环发送.js中的数据
 *socket_fd 套接字，num 线程编号
 *成功返回0，失败返回-1
 */
int send_msg(int socket_fd, int num)
{
	int i = 1;
	ssize_t ret = 0;
	char file_name[32] = {'\0'};
	char buf[1024] = {'\0'};
	while(1)
	{
		memset(file_name, 0, sizeof(file_name));
		snprintf(file_name, sizeof(file_name), "./%d-%d.json", num, i++);

		FILE *fd = fopen(file_name,"r");
		if(fd == NULL)
		{
			printf("open file %s:\n", file_name);
			perror("");
			break;
		}
		memset(buf, 0, sizeof(buf));
		while(ret = fread(buf, 1, sizeof(buf), fd))
		{
			if(ret < 128)
			{
				unsigned char head[2] = {48,ret};
				unsigned char send_msg[sizeof(head) + ret];

				//snprintf(send_msg, sizeof(send_msg), "%s%s", head, buf);
				memcpy(send_msg, head, 2);
				memcpy(send_msg+2, buf, ret);
				send(socket_fd, send_msg, sizeof(send_msg), 0);
				memset(buf, 0, sizeof(buf));
			
				printf("send bytes %ld\n", sizeof(send_msg));
				printf("%d,%d\n", send_msg[0], send_msg[1]);
			}
			else
			{
				ssize_t j = ret/128;
				unsigned char k = (unsigned char) j;
				ssize_t i = (128 + ret%128);
				unsigned char m = (unsigned char) i;
				unsigned char head[3] = {48, m, k};
		
				char send_msg[sizeof(head) + ret];
				//snprintf(send_msg, sizeof(send_msg), "%s%s",head,buf);
				memcpy(send_msg, head, 3);
				memcpy(send_msg+3, buf, ret);

				send(socket_fd, send_msg, sizeof(send_msg), 0);
				memset(buf, 0, sizeof(buf));
				printf("send bytes %ld\n", sizeof(send_msg));
				printf("%d,%d,%d\n", send_msg[0], send_msg[1], send_msg[2]);
			}
		}
		fclose(fd);
	}
	return 0;
}

/*接受数据
 *socket_fd 套接字
 *无返回值
 */
void recv_msg(int socket_fd)
{
	char buf[1024] = {'\0'};
	read(socket_fd, buf, sizeof(buf));
	printf("%d recv msg:%s\n", socket_fd,buf);
	return;
}

/*获取ipconfig.txt中的ip和port
 *num 第几个ip和port
 *返回之为struct ip_port
*/
struct ip_port read_ip_port(int num)
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

/*线程执行函数
 *arg线程序号
 *无返回值
*/
void *handle(void *arg)
{	
	int i = (int)arg;

//	char *file_name = (char *)arg;

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		perror("socket error!\n");
		exit(1);
	}

	struct ip_port p = read_ip_port(i);
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(p.port);
	server_addr.sin_addr.s_addr = inet_addr(p.ip);
	
	if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("connect error !\n");
		exit(1);
	}
	
	printf("%d connect to server success!\n", i+1);
	send_msg(socket_fd, i+1);
	while(1)
	{
		recv_msg(socket_fd);
	}

	exit(1);
}

int main(void)
{
	int i, model;
	char *file_name;

	printf("选择模式1,请输入1;选择模式2，请输入2:");
	scanf("%d", &model);
	if(model == 1)
	{
		printf("线程数:");
		scanf("%d", &i);
		for(int j=0; j < i; j++){
			pthread_t *pid;
			//创建线程
			if(pthread_create(pid, NULL, handle, (void *)j) != 0)
			{
				perror("create thread error!\n");
				return -1;
			}
		}
	
		while(1){}
		return 0;
	}

/*	else if(model == 2)
	{
		printf("请输入文件名:");
		scanf("%s", file_name);

	}

*/
	return 0;
}
