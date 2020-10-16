/*
- ������: TCP�ͻ���
- ���ߣ�ZhengN
- ���ںţ�Ƕ��ʽ���ӻ�
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
	
	/* ��ʼ������sock��Ҫ��DLL */
	WSAStartup(MAKEWORD(2,2),&wd);  
	
	/* ��������������� */
    memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));  
    ServerSockAddr.sin_family = AF_INET;
    ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerSockAddr.sin_port = htons(1314);
	
	while (1)
	{
		/* �����ͻ���socket */
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
		printf("������һ���ַ��������͸�����ˣ�");
		gets(Buf);
		/* �������ݵ������ */
		send(ClientSock, Buf, strlen(Buf), 0);
		
		/* ���ܷ���˵ķ������� */
		recv(ClientSock, Buf, BUF_LEN, 0);
		printf("����˷��͹���������Ϊ��%s\n", Buf);
		
		memset(Buf, 0, BUF_LEN);   // ���û�����
		closesocket(ClientSock);   // �ر��׽���
	}
	
	// WSACleanup();  /*������˳�ѭ�������������ﻹ��Ҫ�����socket���ʹ��*/
	return 0;
}