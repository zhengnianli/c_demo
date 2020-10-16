/*
- 程序功能: 接收JSON数据并解析
- 编译命令: gcc cJSON.c json_parse.c -o json_parse.exe -lwsock32
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "cJSON.h"

#define  STU_NAME_LEN  32

/* 学生结构体 */
typedef struct _Student
{
    char name[STU_NAME_LEN];  // 名字      
    int num;                  // 学号      
    int c_score;              // C语言分数
}StudentDef, *pStudentDef;

/* 内部函数声明 */
static char *StudentsData_Recv(void);
static void StudentsData_Parse(pStudentDef _Stu, const char *_JsonStudnetData);
static void PrintParseResult(const pStudentDef _Stu);
static void SaveParseResult(const pStudentDef _Stu);

/* 内部全局变量 */
static FILE *stu_fp = NULL;

/********************************************************************************************************
** 函数: main
**------------------------------------------------------------------------------------------------------
** 参数: 
** 说明: 
** 返回: 
********************************************************************************************************/
int main(void)
{
    StudentDef stu = {0};   
    char *recv_data;         

    while (1)
    {
        /* 接收数据 */
        recv_data = StudentsData_Recv();

        /* 解析 */
        StudentsData_Parse(&stu, (const char*)recv_data);  

        /* 打印输出解析结果 */
        PrintParseResult(&stu);  

        /* 保存数据到文件 */
        SaveParseResult(&stu);  
    }

    return 0;
}

/********************************************************************************************************
** 函数: StudentsData_Recv, 接收数据
**------------------------------------------------------------------------------------------------------
** 参数: 
** 说明: 
** 返回: 
********************************************************************************************************/
static char *StudentsData_Recv(void)
{
    WSADATA wd;
	SOCKADDR_IN ServerSockAddr;
    int recv_len = 0;
    char *recv_buf = (char*)malloc(512);
    static SOCKET ServerSock, ClientSock;
    static SOCKADDR ClientAddr;
    static int addr_size = 0;
    static int run_count = 0;

    /* 以下操作执行只一次就可以 */
    if (0 == run_count)
    {
       /* 初始化操作sock需要的DLL */
        WSAStartup(MAKEWORD(2,2),&wd);  
        
        /* 创建服务端socket */
        if (-1 == (ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
        {
            printf("server socket error!\n");
            exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }

        printf("bind ok!\n");	
        /* 进入监听状态 */
        if (-1 == listen(ServerSock, 10))
        {
            printf("listen error!\n");
            exit(EXIT_FAILURE);
        }
        printf("listen ok!\n");

        addr_size = sizeof(SOCKADDR);
    }
        
    run_count++;

    /* 监听客户端请求，accept函数返回一个新的套接字，发送和接收都是用这个套接字 */
    if (-1 == (ClientSock = accept(ServerSock, (SOCKADDR*)&ClientAddr, &addr_size)))
    {
        printf("client socket error!\n");
        exit(EXIT_FAILURE);
    }

    /* 接受客户端的返回数据 */
    memset(recv_buf, 0, 512);
    recv_len = recv(ClientSock, recv_buf, 512, 0);
    printf("%s\n", recv_buf);
    
    /* 关闭客户端套接字 */
    closesocket(ClientSock);

    /* 返回获取得到JSON数据 */
    return (char*)recv_buf;
}

/********************************************************************************************************
** 函数: StudentsData_Parse, JOSN格式学生期末数据解析
**------------------------------------------------------------------------------------------------------
** 参数: _JsonStudnetData：JSON数据   _Stu：保存解析出的有用数据
** 说明: 
** 返回: 
********************************************************************************************************/
static void StudentsData_Parse(pStudentDef _Stu, const char *_JsonStudnetData)
{
    cJSON *student_json = NULL;   // student_json操作对象，可代表 {} 扩起来的内容
    cJSON *name = NULL;             
    cJSON *num = NULL;
    cJSON *c_score = NULL;
    
    /* 开始解析 */
    student_json = cJSON_Parse(_JsonStudnetData);
    if (NULL == student_json)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        goto end;
    }

    /* 解析获取name得值 */
    name = cJSON_GetObjectItemCaseSensitive(student_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        memset(&_Stu->name, 0, STU_NAME_LEN*sizeof(char));
        memcpy(&_Stu->name, name->valuestring, strlen(name->valuestring));
    }

    /* 解析获取num的值 */
    num = cJSON_GetObjectItemCaseSensitive(student_json, "num");
    if (cJSON_IsNumber(num))
    {
        _Stu->num = num->valueint;
    }

    /* 解析获取c_score的值 */
    c_score = cJSON_GetObjectItemCaseSensitive(student_json, "c_score");
    if (cJSON_IsNumber(c_score))
    {
        _Stu->c_score = c_score->valueint;
    }

end:
    cJSON_Delete(student_json);
}

/********************************************************************************************************
** 函数: PrintParseResult, 打印输出解析结果
**------------------------------------------------------------------------------------------------------
** 参数: 
** 说明: 
** 返回: 
********************************************************************************************************/
static void PrintParseResult(const pStudentDef _Stu)
{
    printf("name: %s, num: %d, c_score: %d\n\n", _Stu->name, _Stu->num, _Stu->c_score);
}

/********************************************************************************************************
** 函数: SaveParseResult, 保存解析结果
**------------------------------------------------------------------------------------------------------
** 参数: _Stu：需要保存的数据
** 说明: 
** 返回: 
********************************************************************************************************/
static void SaveParseResult(const pStudentDef _Stu)
{
    char write_buf[512] = {0};
    static int stu_count = 0;

    /* 以可在文件末尾追加内容的方式打开文件 */
	if((stu_fp = fopen("ParseResult.txt", "a+")) == NULL)
	{
		printf("Open file error!\n");
		return exit(EXIT_FAILURE);
	} 

    /* 按指定格式写入文件 */
    snprintf(write_buf, 512, "name: %s, num: %d, c_score: %d\n", _Stu->name, _Stu->num, _Stu->c_score);
    size_t len = fwrite((char*)write_buf, 1, strlen(write_buf), stu_fp);

    /* 文件位置指针偏移 */
    fseek(stu_fp, len * stu_count, SEEK_SET);
    stu_count++;

    /* 关闭文件 */
    fclose(stu_fp);
}