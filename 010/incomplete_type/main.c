#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"

int main(void)
{
    int arr_elem = 0;

    /* 初始化一个动态数组 */
	dynamic_array_def *pArray = DA_Init();

    /* 设置数组长度为10 */
    DA_SetSize(pArray, 10);

    /* 给数组元素赋值 */
    for (int i = 0; i < 10; i++)
    {
        DA_SetValue(pArray, i, i);
    }

    /* 遍历数组元素并打印 */
    for (int i = 0; i < 10; i++)
    {
        DA_GetValue(pArray, i, &arr_elem);
        printf("%d ", arr_elem);
    }
    
    /* 数组清理 */
    DA_Clean(pArray);
    
    return 0;
}
