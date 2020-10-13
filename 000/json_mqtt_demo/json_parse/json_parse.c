/*
- 程序功能: 接收JSON数据并解析(MQTT订阅者客户端程序)
- 编译命令: gcc cJSON.c json_parse.c -L ../mosquitto/build/lib -lmosquitto -o json_parse
- 导出mosquitto动态库: export LD_LIBRARY_PATH=../mosquitto/build/lib:$LD_LIBRARY_PATH
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "../mosquitto/lib/mosquitto.h"

#define  STU_NAME_LEN  32

/* 学生结构体 */
typedef struct _Student
{
    char name[STU_NAME_LEN];  // 名字      
    int num;                  // 学号      
    int c_score;              // C语言分数
}StudentDef, *pStudentDef;

/* 内部函数声明 */
static void StudentsData_Parse(pStudentDef _Stu, const char *_JsonStudnetData);
static void PrintParseResult(const pStudentDef _Stu);
static void SaveParseResult(const pStudentDef _Stu);
static void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
static void my_connect_callback(struct mosquitto *mosq, void *userdata, int result);

/* 内部全局变量 */
static FILE *stu_fp = NULL;

/********************************************************************************************************
** 函数: main
**------------------------------------------------------------------------------------------------------
** 参数: 
** 说明: 
** 返回: 
********************************************************************************************************/
bool clean_session = true;
int main(void)
{         
    struct mosquitto *mosq = NULL;

    /* libmosquitto 库初始化 */
    mosquitto_lib_init();

    /* 创建mosquitto客户端 */
    mosq = mosquitto_new(NULL, clean_session, NULL);
    if(NULL == mosq)
    {
        printf("Create mqtt client failed...\n");
        mosquitto_lib_cleanup();
        return 1;
    }

    /* 绑定连接、消息接收回调函数 */
    mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);

    /* 连接服务器 */
    if(mosquitto_connect(mosq, "localhost", 1883, 60))
    {
        printf("Unable to connect...\n");
        return 1;
    }

    /* 循环处理网络消息 */
    mosquitto_loop_forever(mosq, -1, 1);

    /* 回收操作 */
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
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

/********************************************************************************************************
** 函数: my_message_callback, 消息接收回调函数
**------------------------------------------------------------------------------------------------------
** 参数: 
** 返回: 
********************************************************************************************************/
static void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    StudentDef stu = {0};

    if(message->payloadlen)
    {
        printf("%s %s\n", message->topic, (char*)message->payload);

        /* 解析JSON数据 */
        StudentsData_Parse(&stu, (const char*)message->payload);  

        /* 打印输出解析结果 */
        PrintParseResult(&stu);  

        /* 保存数据到文件 */
        SaveParseResult(&stu); 
    }
    else
    {
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

/********************************************************************************************************
** 函数: my_connect_callback, 连接回调函数
**------------------------------------------------------------------------------------------------------
** 参数: 
** 返回: 
********************************************************************************************************/
static void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if(!result)
    {
        /* 订阅test_topic主题的消息 */
        mosquitto_subscribe(mosq, NULL, "test_topic", 0);
    }
    else
    {
        fprintf(stderr, "Connect failed\n");
    }
}