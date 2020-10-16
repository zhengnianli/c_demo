#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_LEN  100

int main(void)
{
	WSADATA wd;
	char Buf[BUF_LEN] = {0};
	
	/* ��ʼ������sock��Ҫ��DLL */
	WSAStartup(MAKEWORD(2,2),&wd);  
	
	struct hostent *host = gethostbyname("www.baidu.com");
	
	if (!host)
	{
		printf("��ȡIPʧ�ܣ�\n");
		exit(0);
	}
	
	//����
    for(int i=0; host->h_aliases[i]; i++)
    {
        printf("Aliases %d: %s\n", i+1, host->h_aliases[i]);
    }

    //��ַ����
    printf("Address type: %s\n", (host->h_addrtype==AF_INET) ? "AF_INET": "AF_INET6");

    //IP��ַ
    for(int i=0; host->h_addr_list[i]; i++)
    {
        printf("IP addr %d:%s\n",i+1,inet_ntoa(*(struct in_addr*)host->h_addr_list[i] ));
    }

    system("pause");
    return 0;
}