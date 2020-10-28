#include "dynamic_array.h"
#include <stdlib.h>

/* 创建一个动态数组结构体模板 */
struct dynamic_array
{
    int *array;
    unsigned len;
};

/* 初始化dynamic_array */
dynamic_array_def *DA_Init(void)
{
    dynamic_array_def *pArray = (dynamic_array_def *)malloc(sizeof(dynamic_array_def));

    pArray->array = NULL;
    pArray->len = 0;
}

/* 销毁dynamic_array */
void DA_Clean(dynamic_array_def *pThis)
{
    free(pThis->array);
    pThis->len = 0;
    free(pThis);
}

/* 设置dynamic_array长度 */
void DA_SetSize(dynamic_array_def *pThis, unsigned len)
{
    pThis->len = len;
    pThis->array = (int*)realloc(pThis->array, pThis->len*sizeof(int));
}

/* 获取dynamic_array长度 */
unsigned DA_GetSize(dynamic_array_def *pThis)
{
    return pThis->len;
}

/* 设置dynamic_array某元素的值 */
int DA_SetValue(dynamic_array_def *pThis, unsigned index, int value)
{
    if (index > pThis->len)
    {
        return -1;
    }

    pThis->array[index] = value;
    return 0;
}

/* 获取dynamic_array某元素的值 */
int DA_GetValue(dynamic_array_def *pThis, unsigned index, int *pValue)
{
    if (index > pThis->len)
    {
        return -1;
    }

    *pValue = pThis->array[index];
    return 0;
}