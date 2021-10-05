// 微信公众号：嵌入式大杂烩
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

char *get_ip_addr(void)
{
	static char ip_str[256] = {0};
	struct ifaddrs *ifaddrs_struct = NULL;
	void *addrptr = NULL;
	
	if (getifaddrs(&ifaddrs_struct) != 0)
	{
		printf("getifaddrs error!\n");
		return NULL;
	}
	
	struct ifaddrs *tmp_ifaddrs = ifaddrs_struct;
	while (tmp_ifaddrs != NULL)
	{
        // IPv4
		if (tmp_ifaddrs->ifa_addr->sa_family == AF_INET)
		{
            #define INET_ADDR_STR_LEN  256
			char addr_buf[INET_ADDR_STR_LEN];
			addrptr = &((struct sockaddr_in*)tmp_ifaddrs->ifa_addr)->sin_addr;
			if (inet_ntop(AF_INET, addrptr, addr_buf, INET_ADDR_STR_LEN) == NULL)
			{
				printf("inet_ntop error!\n");
				return NULL;
			}
            if (strlen(addr_buf) < sizeof(ip_str) - strlen(ip_str))
            {
                strncat(ip_str, addr_buf, strlen(addr_buf));
                strncat(ip_str, ";", strlen(";"));
            }
			
		}
        // IPv6
        else if (tmp_ifaddrs->ifa_addr->sa_family == AF_INET6)
        {
            
        }
		
		tmp_ifaddrs = tmp_ifaddrs->ifa_next;
	}
	
	freeifaddrs(ifaddrs_struct);

    return ip_str;
}

int main(int argc, char** argv)
{
    printf("hello world!\n");
    char *ip_addr = get_ip_addr();
    printf("ip addr : %s\n", ip_addr);

    return 0;
}
