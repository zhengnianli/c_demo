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
	int client_sock = 0;
	char buf[BUF_LEN] = {0};
	
	if (client_sock = nn_socket (AF_SP, NN_PAIR) < 0)
	{
		printf("create server socket failed!\n");
		return -1;
	}
	
	if (nn_connect(client_sock, url) < 0) 
	{
		printf("connect server sock failed!\r\n");
		nn_close(client_sock);
		return -1;
	}
	printf("client init success!\n");
	while (1)
	{
        scanf("%s", buf);
		if (nn_send(client_sock, buf, sizeof(buf), 0) < 0)
		{
			printf("send failed!\r\n");
			nn_close(client_sock);
		}
        memset(buf, 0, BUF_LEN);   

		if (nn_recv(client_sock, buf, sizeof(buf), 0) > 0) 
		{
			printf("recieve server msg: %s\r\n", buf);
		}
        memset(buf, 0, BUF_LEN);   
	}
	nn_close(client_sock);
	
	return 0;
}
