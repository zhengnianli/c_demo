// 公众号：嵌入式大杂烩
// 作者：ZhengN
#include <stdio.h>

// 测试函数1
void test1(void)
{
	int i = -1;

	if (i = 0)
		printf("i = %d\n", i);
	else if (i = 1)
		printf("i = %d\n", i);
	else
		printf("i = %d\n", i);
}

// 测试函数2
void test2(void)
{
	int a[10] = {0,1,2,3,4,5,6,7,8,9};
	int *p = &a[1];
	int *p1 = (int*)(&a + 1);

	printf("p[6] = %d\n", p[6]);  
	printf("*(p1 - 1) = %d\n", *(p1 - 1));		
}

// 主函数
int main(int argc, char *argv[])
{
    test1();
    test2();
    
    return 0;
}