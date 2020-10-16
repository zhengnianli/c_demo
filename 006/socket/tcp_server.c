/*
- ������: TCP�����
- ���ߣ�ZhengN
- ���ںţ�Ƕ��ʽ���ӻ�
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
	
	/* ��ʼ������sock��Ҫ��DLL */
	WSAStartup(MAKEWORD(2,2),&wd);  
	
	/* ���������socket */
	if (-1 == (ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	{
		printf("socket error!\n");
		exit(1);
	}
	
	/* ���÷������Ϣ */
    memset(&ServerSockAddr, 0, sizeof(ServerSockAddr)); 	// ���ṹ��ServerSockAddr����
    ServerSockAddr.sin_family = AF_INET;  					// ʹ��IPv4��ַ
    ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");// ����IP��ַ
    ServerSockAddr.sin_port = htons(1314);  				// �˿�
	
	/* ���׽��� */
    if (-1 == bind(ServerSock, (SOCKADDR*)&ServerSockAddr, sizeof(SOCKADDR)))
	{
		printf("bind error!\n");
		exit(1);
	}
		
	/* �������״̬ */
	if (-1 == listen(ServerSock, 10))
	{
		printf("listen error!\n");
		exit(1);
	}
	
	addr_size = sizeof(SOCKADDR);

	while (1)
	{
		/* �����ͻ�������accept��������һ���µ��׽��֣����ͺͽ��ն���������׽��� */
		if (-1 == (ClientSock = accept(ServerSock, (SOCKADDR*)&ClientAddr, &addr_size)))
		{
			printf("socket error!\n");
			exit(1);
		}

		/* ���ܿͻ��˵ķ������� */
		int recv_len = recv(ClientSock, Buf, BUF_LEN, 0);
		printf("�ͻ��˷��͹���������Ϊ��%s\n", Buf);
		
		/* �������ݵ��ͻ��� */
		send(ClientSock, Buf, recv_len, 0);
		
		/* �رտͻ����׽��� */
		closesocket(ClientSock);
		
		/* ��ջ����� */
		memset(Buf, 0, BUF_LEN);  
	}

	/*������˳�ѭ�������������ﻹ��Ҫ�����socket���ʹ��*/
	/* �رշ�����׽��� */
	//closesocket(ServerSock);
    /* WSACleanup();*/

	return 0;
}