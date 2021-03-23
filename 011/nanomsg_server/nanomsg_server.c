#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <nanomsg/pair.h>
#include <nanomsg/bus.h>
#include <nanomsg/nn.h>

#define BUF_LEN  100

char *url = "tcp://127.0.0.1:2021";
 
int main(void)
{
	int server_sock = 0;
	char buf[BUF_LEN] = {0};
	
	if (server_sock = nn_socket (AF_SP, NN_PAIR) < 0)
	{
		printf("create server socket failed!\n");
		return -1;
	}
	
	if (nn_bind(server_sock, url) < 0) 
	{
		printf("bind server sock failed!\r\n");
		nn_close(server_sock);
		return -1;
	}
	printf("server init success!\n");
	while (1)
	{
		if (nn_recv(server_sock, buf, sizeof(buf), 0) < 0) 
		{
			printf("recv failed!\n");
			nn_close(server_sock);
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("recieve client msg: %s\r\n", buf);
			if (nn_send(server_sock, buf, sizeof(buf), 0) < 0)
			{
				printf("send failed!\r\n");
				nn_close(server_sock);
				exit(EXIT_FAILURE);
			}
		}
	}
	nn_close(server_sock);
	
	return 0;
}
