/*
- 程序功能: 交换a、b值的几种方法
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <stdlib.h>

// 函数选择
#define FUNC_CHIOCE 3  // 1:方法一  2:方法二  3:方法三

#if (FUNC_CHIOCE == 1)  // 方法一：普通方法
void change_ab(int *a, int *b)
{
    int temp = 0;

    temp = *a;
    *a = *b;
    *b = temp;
}
#elif(FUNC_CHIOCE == 2) // 方法二：采用算术的方法
void change_ab(int *a, int *b)
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}
#else                  // 方法三：采用异或的方法（较好的方法）
void change_ab(int *a, int *b)
{
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}
#endif

int main(void)
{
    int a = 1, b = 2;

    printf("Before change, a = %d, b = %d\n", a, b);
    change_ab(&a, &b);
    printf("After change, a = %d, b = %d\n", a, b);
    system("pause");

    return 0;
}