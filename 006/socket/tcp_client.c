/*
- 程序功能: TCP客户端
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <winsock2.h>

#define BUF_LEN  100

int main(void)
{
	WSADATA wd;
	SOCKET ClientSock;
	char Buf[BUF_LEN] = {0};
	SOCKADDR_IN  ServerSockAddr;
	
	/* 初始化操作sock需要的DLL */
	WSAStartup(MAKEWORD(2,2),&wd);  
	
	/* 向服务器发起请求 */
    memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));  
    ServerSockAddr.sin_family = AF_INET;
    ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerSockAddr.sin_port = htons(1314);
	
	while (1)
	{
		/* 创建客户端socket */
		if (-1 == (ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
		{
			printf("socket error!\n");
			exit(1);
		}
		if (-1 == connect(ClientSock, (SOCKADDR*)&ServerSockAddr, sizeof(SOCKADDR)))
		{
			printf("connect error!\n");
			exit(1);
		}
		printf("请输入一个字符串，发送给服务端：");
		gets(Buf);
		/* 发送数据到服务端 */
		send(ClientSock, Buf, strlen(Buf), 0);
		
		/* 接受服务端的返回数据 */
		recv(ClientSock, Buf, BUF_LEN, 0);
		printf("服务端发送过来的数据为：%s\n", Buf);
		
		memset(Buf, 0, BUF_LEN);   // 重置缓冲区
		closesocket(ClientSock);   // 关闭套接字
	}
	
	// WSACleanup();  /*如果有退出循环的条件，这里还需要清除对socket库的使用*/
	return 0;
}