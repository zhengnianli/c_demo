/*
- 程序功能: goto的使用
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <stdlib.h>

/* goto测试 */
void TestGoto(void)
{
	int i;
	
	while (1)
	{
		for (i = 0; i < 10; i++)
		{
			if (i > 6)
			{
				goto label;
			}
			printf("%s : i = %d\n", __FUNCTION__, i);
		}
	}
	label:
		printf("test goto end!");
}
 
int main(void)
{
    TestGoto();
}

/*------执行结果------
TestGoto : i = 0
TestGoto : i = 1
TestGoto : i = 2
TestGoto : i = 3
TestGoto : i = 4
TestGoto : i = 5
TestGoto : i = 6
test goto end!
*/