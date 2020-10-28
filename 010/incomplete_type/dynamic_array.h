#ifndef __DYNAMIC_ARRAY_H
#define __DYNAMIC_ARRAY_H

/* 结构体“重命名” */
typedef struct dynamic_array dynamic_array_def;

/* 初始化dynamic_array */
dynamic_array_def *DA_Init(void);

/* 销毁dynamic_array */
void DA_Clean(dynamic_array_def *pThis);

/* 设置dynamic_array长度 */
void DA_SetSize(dynamic_array_def *pThis, unsigned len);

/* 获取dynamic_array长度 */
unsigned DA_GetSize(dynamic_array_def *pThis);

/* 设置dynamic_array某元素的值 */
int DA_SetValue(dynamic_array_def *pThis, unsigned index, int value);

/* 获取dynamic_array某元素的值 */
int DA_GetValue(dynamic_array_def *pThis, unsigned index, int *pValue);

#endif
