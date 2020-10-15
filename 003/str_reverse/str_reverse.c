/*
- 程序功能: 字符串反转
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 交换两数函数选择 */
#define FUNC_CHIOCE 1  // 1:方法一  2:方法二  3:方法三

/* 交换两数函数 */
#if (FUNC_CHIOCE == 1)  // 方法一：普通方法
void change_ab(char *a, char *b)
{
    int temp = 0;

    temp = *a;
    *a = *b;
    *b = temp;
}
#elif(FUNC_CHIOCE == 2) // 方法二：采用算术的方法
void change_ab(char *a, char *b)
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}
#else                  // 方法三：采用异或的方法（较好的方法）
void change_ab(char *a, char *b)
{
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}
#endif

/* 字符串反转函数 */
char *str_reverse(char* src_str)
{
    int str_len = strlen(src_str);
    char *dest_str = src_str;

    char *pHead = &src_str[0];          // 指向字符串的头
    char *pTail = &src_str[str_len -1]; // 指向字符串的尾

    while ((pHead != pTail) && (pHead < pTail))
    {
        change_ab(pHead, pTail); // 交换头尾字符
        pHead++; // 指向头部的指针往后移
        pTail--; // 指向尾部的指针往前移
    }

    return dest_str;
}

/* 主函数 */
int main(void)
{
    char src_str[] = "hello world";
    
    char *dest_str = (char*)malloc(strlen(src_str));
    printf("Before reverse: %s\n", src_str);
    dest_str = str_reverse(src_str);
    printf("After reverse: %s\n", dest_str);
    free(dest_str);
    dest_str = NULL;

    return 0;
}