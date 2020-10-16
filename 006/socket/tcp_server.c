/*
- 程序功能: TCP服务端
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <winsock2.h>

#define BUF_LEN  100

int main(void)
{
	WSADATA wd;
	SOCKET ServerSock, ClientSock;
	char Buf[BUF_LEN] = {0};
	SOCKADDR ClientAddr;
	SOCKADDR_IN ServerSockAddr;
	int addr_size = 0, recv_len = 0;
	
	/* 初始化操作sock需要的DLL */
	WSAStartup(MAKEWORD(2,2),&wd);  
	
	/* 创建服务端socket */
	if (-1 == (ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	{
		printf("socket error!\n");
		exit(1);
	}
	
	/* 设置服务端信息 */
    memset(&ServerSockAddr, 0, sizeof(ServerSockAddr)); 	// 给结构体ServerSockAddr清零
    ServerSockAddr.sin_family = AF_INET;  					// 使用IPv4地址
    ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");// 本机IP地址
    ServerSockAddr.sin_port = htons(1314);  				// 端口
	
	/* 绑定套接字 */
    if (-1 == bind(ServerSock, (SOCKADDR*)&ServerSockAddr, sizeof(SOCKADDR)))
	{
		printf("bind error!\n");
		exit(1);
	}
		
	/* 进入监听状态 */
	if (-1 == listen(ServerSock, 10))
	{
		printf("listen error!\n");
		exit(1);
	}
	
	addr_size = sizeof(SOCKADDR);

	while (1)
	{
		/* 监听客户端请求，accept函数返回一个新的套接字，发送和接收都是用这个套接字 */
		if (-1 == (ClientSock = accept(ServerSock, (SOCKADDR*)&ClientAddr, &addr_size)))
		{
			printf("socket error!\n");
			exit(1);
		}

		/* 接受客户端的返回数据 */
		int recv_len = recv(ClientSock, Buf, BUF_LEN, 0);
		printf("客户端发送过来的数据为：%s\n", Buf);
		
		/* 发送数据到客户端 */
		send(ClientSock, Buf, recv_len, 0);
		
		/* 关闭客户端套接字 */
		closesocket(ClientSock);
		
		/* 清空缓冲区 */
		memset(Buf, 0, BUF_LEN);  
	}

	/*如果有退出循环的条件，这里还需要清除对socket库的使用*/
	/* 关闭服务端套接字 */
	//closesocket(ServerSock);
    /* WSACleanup();*/

	return 0;
}