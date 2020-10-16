/*
- 程序功能: 简单的通讯录。可保存联系人的名字、电话号码、城市、公司至磁盘文件中。
- 作者：ZhengN
- 公众号：嵌入式大杂烩
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxCnt  100
#define CntFull -1

// 菜单命令宏定义
#define CMD_Add    	1
#define CMD_Save    2
#define CMD_List    3
#define CMD_Delete  4
#define CMD_Clear 	5
#define CMD_Exit	6

// 定义联系人基本信息结构体
typedef struct Message
{
	char name[32];		//姓名
	char PhoneNum[32];  //电话号码
	char city[32];  	//城市
	char company[32];	//公司
}Message;

Message LinkmanMes[MaxCnt];		//可保存MaxCnt个联系人

// 函数声明
void Init(void);
void AddLinkman(void);
int FindCapacity(void);
void SaveToFile(void);
void PrintfList(void);
int MenuSelect(void);
void Delete(void);

/*********************************************************************************
* Function Name    ： main主函数
* Parameter		   ： NULL
* Return Value     ： 0 
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
int main(void)
{
	int cmd;
	int num;
	
	Init();
	while(1)
	{
		cmd = MenuSelect();
		switch(cmd)
		{
			case CMD_Add: 
				AddLinkman();
				break;
			case CMD_Save:
				SaveToFile();
				break;
			case CMD_List:
				PrintfList();
				break;	
			case CMD_Delete:
				Delete();
				break;	
			case CMD_Clear:
				system("cls");
				break;
			case CMD_Exit:
				exit(0);
				break;	
		}
	}
	
	return 0;
}

/*********************************************************************************
* Function Name    ： 通讯录初始化，初始化所有联系人信息为空
* Parameter		   ： NULL
* Return Value     ： 0 
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
void Init(void)
{
	for(int i; i<MaxCnt; i++)
	{
		LinkmanMes[i].name[0] = '\0';
	}
}

/*********************************************************************************
* Function Name    ： 添加联系人
* Parameter		   ： NULL
* Return Value     ： 0 
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
void AddLinkman(void)
{
	int slot;
	
	slot = FindCapacity();
	printf("Linkman num=%d\n",slot+1);
	if(slot==CntFull)
	{
		printf("\nList full\n");
		return;
	}
	printf("Input name:");
	gets(LinkmanMes[slot].name);
	printf("Input PhoneNum:");
	gets(LinkmanMes[slot].PhoneNum);
	printf("Input city:");
	gets(LinkmanMes[slot].city);
	printf("Input company:");
	gets(LinkmanMes[slot].company);
	printf("\n\n");
}

/*********************************************************************************
* Function Name    ： FindCapacity，查找可保存联系人的容量，最大容量为MaxCnt
* Parameter		   ： NULL
* Return Value     ： CntFull：已经没有余量了 slot：第slot+1个联系人  
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
int FindCapacity(void)
{
	int slot;
	
	for(slot=0; LinkmanMes[slot].name[0]&&slot<MaxCnt; slot++);
	if(slot==MaxCnt)
	{
		return CntFull;	//已经达到了最大容量，没有余量了
	}
	
	return slot;
}

/*********************************************************************************
* Function Name    ： SaveToFile，保存所有联系人信息至本地文件Contact.txt
* Parameter		   ： NULL
* Return Value     ： 0
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
void SaveToFile(void)
{
	FILE *fp;
	char num[100];
	
	if((fp=fopen("Contact.txt","wb"))==NULL)
	{
		printf("Cannot open the file.\n");
		exit(0);  /* 正常跳出程序 */
	}
	for(int i=0; i<MaxCnt; i++)
	{
		if(*LinkmanMes[i].name)
		{
			sprintf(num,"Num.%.2d",i+1);
			fwrite(num, 6, 1, fp);
			fwrite("\r\n", 2, 1, fp); 
			fwrite(LinkmanMes[i].name, strlen(LinkmanMes[i].name), 1, fp);
			fwrite("\r\n", 2, 1, fp); 
			fwrite(LinkmanMes[i].PhoneNum, strlen(LinkmanMes[i].PhoneNum), 1, fp);
			fwrite("\r\n", 2, 1, fp); 
			fwrite(LinkmanMes[i].city, strlen(LinkmanMes[i].city), 1, fp);
			fwrite("\r\n", 2, 1, fp); 
			fwrite(LinkmanMes[i].company, strlen(LinkmanMes[i].company), 1, fp);
			fwrite("\r\n", 2, 1, fp); 
		}
	}
	
	fclose(fp);
}

/*********************************************************************************
* Function Name    ： PrintfList，打印联系人列表
* Parameter		   ： NULL
* Return Value     ： 0
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
void PrintfList(void)
{
	int i;
	
	for(i=0; i<MaxCnt; i++)
	{
		if(LinkmanMes[i].name[0])
		{
			printf("Num.%d\n",i+1);
			printf("%s\n",LinkmanMes[i].name);
			printf("%s\n",LinkmanMes[i].PhoneNum);
			printf("%s\n",LinkmanMes[i].city);
			printf("%s\n",LinkmanMes[i].company);
			printf("\n");
		}
	}
}

/*********************************************************************************
* Function Name    ： Delete，删除联系人
* Parameter		   ： NULL
* Return Value     ： 0
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
void Delete(void)
{
	int i;
	char str[80];
	int num;
	
	printf("Input the linkman num(1-100)to delete a linkman or input 0 to delete all linkman:");
	gets(str);
	num = atoi(str);
	/* 删除第num个联系人 */
	if( (num>0)&&(num<MaxCnt) )
	{
		LinkmanMes[num-1].name[0] = '\0';
	}
	/* 删除所有联系人 */
	else if(num==0)
	{
		for(i=0; i<MaxCnt; i++)
		{
			LinkmanMes[i].name[0] = '\0';
		}
	}
	printf("\n\n");
}

/*********************************************************************************
* Function Name    ： MenuSelect，菜单
* Parameter		   ： NULL
* Return Value     ： c:菜单命令
* Function Explain ： 
* Create Date      ： 2016.2.4 by ZhengN
**********************************************************************************/
int MenuSelect(void)
{
	char str[80];
	int c;

	printf("1. Add linkman\n");
	printf("2. Save linkman to file\n");
	printf("3. Printf all linkman\n");
	printf("4. Delete linkman\n");
	printf("5. Clear screen\n");
	printf("6. Exit\n");
	do{
		printf("\nEnter your choice: ");
		gets(str);
		c = atoi(str);
	} while(c<0 || c>6);
	
	return c;
}


