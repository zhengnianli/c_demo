/*
- 程序功能: JSON数据解析
- 编译命令: gcc cJSON.c json_parse.c -o json_parse.exe -lwsock32
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void StudentsData_Parse(pStudentDef _Stu, const char *_JsonStudnetData);
static void PrintParseResult(const pStudentDef _Stu);

/********************************************************************************************************
** 函数: main
**------------------------------------------------------------------------------------------------------
** 参数: 
** 说明: 
** 返回: 
********************************************************************************************************/
int main(void)
{
    StudentDef stu = {0};  // 保存解析后的数据
    int file_len = 0;      // 文件长度
    FILE *fp = NULL;       // 文件句柄
    char *data = NULL;     // 用于保存从文件读出的数据

    /* 文件操作 */
    if ((fp = fopen("student_data.txt", "r")) == NULL)
    {
        printf("Open file error!\n");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);    // 文件位置指针指向文件末尾
    file_len = ftell(fp);      // 获取文末相对于文首的偏移值
    fseek(fp, 0, SEEK_SET);    // 文件位置指针指向文首
    data = (char*)malloc(file_len+1); // 为data申请堆内存
    fread(data,file_len,1,fp); // 读取文件数据保存至data
    fclose(fp);                // 关闭文件

    /* 解析 */
    StudentsData_Parse(&stu, (const char*)data);  

    /* 打印输出解析结果 */
    PrintParseResult(&stu);  

    /* 释放内存 */ 
    free(data);   // 防止内存泄漏
    data = NULL;  // 防止出现野指针

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
    printf("name: %s, num: %d, c_score: %d\n", _Stu->name, _Stu->num, _Stu->c_score);
}