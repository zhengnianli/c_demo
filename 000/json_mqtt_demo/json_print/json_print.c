/*
- 程序功能: 组JSON格式数据包并发送(MQTT发布者客户端程序)
- 编译命令: gcc cJSON.c json_print.c -L ../mosquitto/build/lib -lmosquitto -o json_print
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
static StudentDef StudentData_Prepare(void);
static char *StudentsData_Packet(pStudentDef _Stu);
static void StudentData_Send(const char *_data);
static void MqttClientInit(void);
static void MqttClientClean(void);

bool clean_session = true;
struct mosquitto *mosq = NULL;

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
    char *stu_data = NULL;
    int stu_count = 0;
    int i = 0;

    /* MQTT客户端初始化 */
    MqttClientInit();

    /* 需要登记的学生总人数 */
    printf("Please input number of student: ");
    scanf("%d", &stu_count);

    while (i++ < stu_count)
    {
        /* 准备数据 */
        stu = StudentData_Prepare();

        /* JSON格式数据组包 */
        stu_data = StudentsData_Packet(&stu);

        /* 发送数据 */
        StudentData_Send(stu_data);
    }

    /* 回收操作 */
    MqttClientClean();

    return 0;
}

/********************************************************************************************************
** 函数: StudentData_Prepare, 准备组包需要的数据
**------------------------------------------------------------------------------------------------------
** 参数: 
** 说明: 
** 返回: 获取得到的数据
********************************************************************************************************/
static StudentDef StudentData_Prepare(void)
{
    char name[STU_NAME_LEN] = {0};
    int num = 0;
    int c_score = 0;
    StudentDef stu;

    /* 名字 */
    printf("Please input name: ");
    scanf("%s", name);
    if (strlen(name) < STU_NAME_LEN)
    {
        strncpy((char*)&stu.name, name, strlen(name)+1);
    }
    else
    {
        printf("The name is too long\n");
    }
    
    /* 学号 */
    printf("Please input num (0~100): ");
    scanf("%d", &num);
    stu.num = num;

    /* C语言分数 */
    printf("Please input c_score (0~100): ");
    scanf("%d", &c_score);
    stu.c_score = c_score;

    return stu;
}

/********************************************************************************************************
** 函数: StudentsData_Packet, JSON格式数据组包
**------------------------------------------------------------------------------------------------------
** 参数: _Stu：组student json数据包需要的数据
** 说明: 
** 返回: JSON格式的字符串	
********************************************************************************************************/
static char *StudentsData_Packet(pStudentDef _Stu)
{
    char *res_string = NULL;    // 返回值
    cJSON *name = NULL;         // 名字
    cJSON *num = NULL;          // 学号
    cJSON *c_score = NULL;      // C语言分数

    /* 创建一个JSON对象，｛｝扩起来 */
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL)
    {
        goto end;
    }

    /* 创建 "name": "xxx" 键值对 */
    name = cJSON_CreateString(_Stu->name);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(obj, "name", name);

    /* 创建 "num": 207 键值对 */
    num = cJSON_CreateNumber(_Stu->num);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(obj, "num", num);
    
    /* 创建 "c_score": 95 键值对 */
    c_score = cJSON_CreateNumber(_Stu->c_score);
    if (name == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(obj, "c_score", c_score); 

    res_string = cJSON_Print(obj);          // 呈现为JSON格式 
    // res_string = cJSON_PrintUnformatted(obj);   // 呈现为无格式

    if (res_string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

/* 异常情况统一Delete（free） */
end:
    cJSON_Delete(obj);
    return res_string;
}

/********************************************************************************************************
** 函数: StudentData_Send, JSON格式字符串数据组包发送
**------------------------------------------------------------------------------------------------------
** 参数: _data：要发送的数据
** 说明: 
** 返回: JSON格式的字符串	
********************************************************************************************************/
static void StudentData_Send(const char *_data)
{
    printf("%s: %s\n\n", __FUNCTION__, _data);

    /* 发布消息 */
    mosquitto_publish(mosq, NULL, "test_topic", strlen(_data)+1, (const char*)_data, 0, 0);
}

/********************************************************************************************************
** 函数: MqttClientInit, MQTT客户端初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 
** 返回: 
********************************************************************************************************/
static void MqttClientInit(void)
{
    /* libmosquitto 库初始化 */
    mosquitto_lib_init();

    /* 创建mosquitto客户端 */
    mosq = mosquitto_new(NULL, clean_session, NULL);
    if(NULL == mosq)
    {
        printf("Create mqtt client failed...\n");
        mosquitto_lib_cleanup();
        return;
    }

    /* 连接服务器 */
    if(mosquitto_connect(mosq, "localhost", 1883, 60))
    {
        printf("Unable to connect...\n");
        return;
    }

    /* 网络消息处理线程 */
    int loop = mosquitto_loop_start(mosq);
    if(loop != MOSQ_ERR_SUCCESS)
    {
        printf("mosquitto loop error\n");
        return;
    }
}

/********************************************************************************************************
** 函数: MqttClientClean, MQTT客户端清理操作
**------------------------------------------------------------------------------------------------------
** 参数: void
** 说明: 
** 返回: 
********************************************************************************************************/
static void MqttClientClean(void)
{
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}