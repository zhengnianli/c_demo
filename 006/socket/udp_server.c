/*
- ������: UDP�����
- ���ߣ�ZhengN
- ���ںţ�Ƕ��ʽ���ӻ�
*/
#include <stdio.h>
#include <winsock2.h>

#define BUF_LEN  100

int main(void)
{
	WSADATA wd;
	SOCKET ServerSock;
	char Buf[BUF_LEN] = {0};
	SOCKADDR ClientAddr;
	SOCKADDR_IN ServerSockAddr;
	int addr_size = 0;
	
	
	/* ��ʼ������sock��Ҫ��DLL */
	WSAStartup(MAKEWORD(2,2),&wd);  
	
	/* ���������socket */
	if(-1 == (ServerSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
	{
		printf("socket error!\n");
		exit(1);
	}
	
	/* ���÷������Ϣ */
    memset(&ServerSockAddr, 0, sizeof(ServerSockAddr)); 	// ���ṹ��ServerSockAddr����
    ServerSockAddr.sin_family = AF_INET;  					// ʹ��IPv4��ַ
    ServerSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); 	// �Զ���ȡIP��ַ
    ServerSockAddr.sin_port = htons(1314);  				// �˿�
	
	/* ���׽��� */
	
    if (-1 == (bind(ServerSock, (SOCKADDR*)&ServerSockAddr, sizeof(SOCKADDR))))
	{
		printf("bind error!\n");
		exit(1);
	}
		
	addr_size = sizeof(SOCKADDR);

	while (1)
	{
		/* ���ܿͻ��˵ķ������� */
		int str_len = recvfrom(ServerSock, Buf, BUF_LEN, 0, &ClientAddr, &addr_size);
		
		printf("�ͻ��˷��͹���������Ϊ��%s\n", Buf);
		
		/* �������ݵ��ͻ��� */
		sendto(ServerSock, Buf, str_len, 0, &ClientAddr, addr_size);
		
		/* ��ջ����� */
		memset(Buf, 0, BUF_LEN);  
	}

	/*������˳�ѭ�������������ﻹ��Ҫ�����socket���ʹ��*/
	/* �رշ�����׽��� */
	//closesocket(ServerSock);
    /* WSACleanup();*/

	return 0;
}