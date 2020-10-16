/*----------------------------------------------------------------------------------------
	
	Program Explain：推箱子游戏

----------------------------------------------------------------------------------------*/

/*
	程序要点：1、地图的布局（二维数组作为函数参数的问题！！）
			  2、特出字符长度是普通字符的两倍
			  3、怎么给字符作色（使用window API）
			  4、怎么控制小人移动
*/

#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include<conio.h>

#define MAP_LEN   10
#define MAP_WIDTH 10

#define SUCCESS   1
#define FAILURE   0

//小人的位置
int x = 0, y =0;

//当前地图
int map[MAP_LEN][MAP_WIDTH] = {0};

//保存上一步的界面（暂未用到）
int map_tmp[MAP_LEN][MAP_WIDTH] = {0};


//地图 1
int map_1[MAP_LEN][MAP_WIDTH] = {
	{0,0,1,1,1,0,0,0},
	{0,0,1,4,1,0,0,0},
	{0,0,1,0,1,1,1,1},
	{1,1,1,0,0,2,4,1},
	{1,4,2,2,0,1,1,1},
	{1,1,1,3,2,1,0,0},
	{0,0,0,1,4,1,0,0},
	{0,0,0,1,1,1,0,0},
};

//地图 2
int map_2[10][10] = {
	{ 0,0,0,1,1,1,1,1,1,1 },
	{ 0,0,1,1,0,0,1,0,3,1 },
	{ 0,0,1,0,0,0,1,0,0,1 },
	{ 0,0,1,2,0,2,0,2,0,1 },
	{ 0,0,1,0,2,1,1,0,0,1 },
	{ 1,1,1,0,2,0,1,0,1,1 },
	{ 1,4,4,4,4,4,0,0,1,0 },
	{ 1,1,1,1,1,1,1,1,1,0 },
};



//函数声明
void map_layout(int **array, int m, int n);		  //地图布局
void set_color(short int color);				  //设置颜色
void set_pattern(char *str, short int color);	  //绘画图案
void select_map(int n);							  //地图选择
void print_note(void);							  //打印游戏说明
void control_move(int x1, int y1, int x2, int y2);//控制小人移动
void find_man(void);							  //找到当前小人所在的位置
int finish(void);								  //判断游戏是否结束


/*********************************************************************************
* Function Name    ： main主函数
* Parameter		   ： NULL
* Return Value     ： 0 
* Function Explain ： 
**********************************************************************************/
int main(void)
{
	int *p_map[10] = {map[0], map[1], map[2], map[3], map[4], map[5], map[6], map[7], map[8], map[9]};
	char dir = 0;
	int map_num = 0;
	
	print_note();
	
	while(scanf("%d",&map_num))
	{
		if( map_num == 0 )
		{
			printf("game over!!\n");
			break;
		}
		system("cls");
		if( (map_num==1)||(map_num==2) )
		{
			select_map(map_num);
			map_layout(p_map, MAP_LEN, MAP_WIDTH);
			while((dir=getch()) != EOF)
			{
				system("cls");//移动之前，删除原有命令行中的地图
				find_man();
				memcpy(map_tmp, map, sizeof(map));
				switch(dir)
				{
					case 'w': control_move(x-1, y, x-2, y); break;
					case 's': control_move(x+1, y, x+2, y); break;
					case 'a': control_move(x, y-1, x, y-2); break;
					case 'd': control_move(x, y+1, x, y+2); break;
					case 'r': select_map(map_num); break;
					case 'z': 
							  memcpy(map, map_tmp, sizeof(map_tmp));
							  map_layout(p_map, MAP_LEN, MAP_WIDTH);
							  break;
					case 'q': return 0;
					default:  break;
				}
				map_layout(p_map, MAP_LEN, MAP_WIDTH);
				if(finish())
				{
					printf("游戏胜利,请重新选择关卡(1/2)(0:退出游戏):");
					break;
				}
			}
		}
		else 
		{
			printf("输入有误，请重新输入\n");
			printf("请选择关卡：（1/2）(0:退出游戏):");
		}
	}
	
	
	return 0;
}

/*********************************************************************************
* Function Name    ： map_layout地图布局函数
* Parameter		   ： array：二维数组  m：一维长度  n：二维长度
* Return Value     ： void 
* Function Explain ： 
**********************************************************************************/
void map_layout(int **array, int m, int n)
{
	int i = 0, j = 0;
	
	for ( i = 0; i < m; i++ )
	{
		for ( j = 0; j < n; j++ )
		{
			switch(array[i][j])
			{
				case 0: set_pattern("  ", 0xF); break;		//特殊字符是普通字符的两倍
				case 1: set_pattern("■", 8);    break;
				case 2: set_pattern("■", 4);    break;
				case 3: set_pattern("♀", 3);    break;
				case 4: set_pattern("★", 0xE); break;
				case 5: set_pattern("★", 6);   break;
				case 6: set_pattern("♀", 3);    break;
				default: 						break;
			}
		}
		printf("\n");
	}
}

/*********************************************************************************
* Function Name    ： set_color设置颜色
* Parameter		   ： color：颜色值
* Return Value     ： void 
* Function Explain ： 
**********************************************************************************/
void set_color(short int color)
{
	HANDLE hConsole  = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

/*********************************************************************************
* Function Name    ： set_pattern设置图案
* Parameter		   ： str：特殊字符  color：颜色值
* Return Value     ： void 
* Function Explain ： 
**********************************************************************************/
void set_pattern(char *str, short int color)
{
	set_color(color);
	printf("%s", str);
}

void select_map(int n)
{
	switch(n)
	{
		case 1: memcpy(map, map_1, sizeof(map_1)); break;
		case 2: memcpy(map, map_2, sizeof(map_2)); break;
		default: break;
	}
}

/*********************************************************************************
* Function Name    ： control_move控制小人移动
* Parameter		   ： x1,y1:人前面的图案的坐标  x2,y2:箱子前面的图案的坐标
* Return Value     ： void 
* Function Explain ： 
**********************************************************************************/
/*
	1、人位于空格处：
	（1）人前是箱子，箱子在空格上，箱子前面是空格
	（2）人前是箱子，箱子在空格上，箱子前面是目标位置
	（3）人前是箱子，箱子在目标位置上，目标位置前面是空格
	（4）人前是箱子，箱子在目标位置上，目标位置前面是目标位置
	（5）人前是空格
	（6）人前是目标位置
	2、人位于目标位置处：同上

*/
void control_move(int x1, int y1, int x2, int y2)
{
	//人位于空格处
	if( map[x][y] == 3 )
	{
		//人前是箱子，箱子在空格上
		if( map[x1][y1] == 2 )
		{
			//箱子前面是空格
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 0;
				map[x1][y1] = 3;
				map[x2][y2] = 2;
			}
			//箱子前面是目标位置
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 0;
				map[x1][y1] = 3;
				map[x2][y2] = 5;
			}
		}
		//人前是箱子，箱子在目标位置上
		if( map[x1][y1] == 5 )
		{
			//箱子前面是空格
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 0;
				map[x1][y1] = 6;
				map[x2][y2] = 2;
			}
			//箱子前面是目标位置
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 0;
				map[x1][y1] = 6;
				map[x2][y2] = 5;
			}
		}
		//人前是空格
		if( map[x1][y1] == 0 )
		{
			map[x][y] = 0;
			map[x1][y1] = 3;
		}
		//人前是目标位置
		if( map[x1][y1] == 4 )
		{
			map[x][y] = 0;
			map[x1][y1] = 6;
		}
	}
	
	//人位于目标位置
	if( map[x][y] == 6 )
	{
		//人前是箱子，箱子在空格上
		if( map[x1][y1] == 2 )
		{
			//箱子前面是空格
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 4;
				map[x1][y1] = 3;
				map[x2][y2] = 2;
			}
			//箱子前面是目标位置
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 4;
				map[x1][y1] = 3;
				map[x2][y2] = 5;
			}
		}
		//人前是箱子，箱子在目标位置上
		if( map[x1][y1] == 5 )
		{
			//箱子前面是空格
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 4;
				map[x1][y1] = 6;
				map[x2][y2] = 2;
			}
			//箱子前面是目标位置
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 4;
				map[x1][y1] = 6;
				map[x2][y2] = 5;
			}
		}
		//人前是空格
		if( map[x1][y1] == 0 )
		{
			map[x][y] = 4;
			map[x1][y1] = 3;
		}
		//人前是目标位置
		if( map[x1][y1] == 4 )
		{
			map[x][y] = 4;
			map[x1][y1] = 6;
		}
	}
}

/*********************************************************************************
* Function Name    ： find_man寻找人的位置
* Parameter		   ： void
* Return Value     ： void 
* Function Explain ： 
**********************************************************************************/
void find_man(void)
{
	for( x = 0; x < MAP_LEN; x++ )
	{
		for( y = 0; y < MAP_WIDTH; y++ )
		{
			if( (map[x][y]==3) || (map[x][y]==6) )
			{
				return;
			}
		}
	}
}

/*********************************************************************************
* Function Name    ： finish判断是否闯关成功
* Parameter		   ： void
* Return Value     ： SUCCESS：闯关成功   FAILURE：还未成功
* Function Explain ： 
**********************************************************************************/
int finish(void)
{
	for( x = 0; x < MAP_LEN; x++ )
	{
		for( y = 0; y < MAP_WIDTH; y++ )
		{
			if( map[x][y] == 2 )
			{
				return FAILURE;
			}
		}
	}
	
	return SUCCESS;
}

/*********************************************************************************
* Function Name    ： print_note打印输出游戏规则
* Parameter		   ： void
* Return Value     ： void
* Function Explain ： 
**********************************************************************************/
void print_note(void)
{
	printf("**************推箱子游戏欢迎您***************\n"
	"*                                           *\n"
	"*                                           *\n"
	"*             ♀:自己的位置                 *\n"
	"*             ☆:箱子要到的的位置           *\n"
	"*             ■:箱子的位置                 *\n"
	"*                                           *\n"
	"*         |       游戏规则       |          *\n"
	"*                                           *\n"
	"*          控制♀将■推到☆的位置           *\n"
	"*          按 wasd 控制♀的移动             *\n"
	"*          按   r  重新开始游戏             *\n"
	"*          按   q      退出游戏             *\n"
	"*********************************************\n");
	set_color(0xA);
	printf("请选择关卡：（1/2）(0:退出游戏):");
}