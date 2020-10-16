/*----------------------------------------------------------------------------------------
	
	Program Explain����������Ϸ

----------------------------------------------------------------------------------------*/

/*
	����Ҫ�㣺1����ͼ�Ĳ��֣���ά������Ϊ�������������⣡����
			  2���س��ַ���������ͨ�ַ�������
			  3����ô���ַ���ɫ��ʹ��window API��
			  4����ô����С���ƶ�
*/

#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include<conio.h>

#define MAP_LEN   10
#define MAP_WIDTH 10

#define SUCCESS   1
#define FAILURE   0

//С�˵�λ��
int x = 0, y =0;

//��ǰ��ͼ
int map[MAP_LEN][MAP_WIDTH] = {0};

//������һ���Ľ��棨��δ�õ���
int map_tmp[MAP_LEN][MAP_WIDTH] = {0};


//��ͼ 1
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

//��ͼ 2
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



//��������
void map_layout(int **array, int m, int n);		  //��ͼ����
void set_color(short int color);				  //������ɫ
void set_pattern(char *str, short int color);	  //�滭ͼ��
void select_map(int n);							  //��ͼѡ��
void print_note(void);							  //��ӡ��Ϸ˵��
void control_move(int x1, int y1, int x2, int y2);//����С���ƶ�
void find_man(void);							  //�ҵ���ǰС�����ڵ�λ��
int finish(void);								  //�ж���Ϸ�Ƿ����


/*********************************************************************************
* Function Name    �� main������
* Parameter		   �� NULL
* Return Value     �� 0 
* Function Explain �� 
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
				system("cls");//�ƶ�֮ǰ��ɾ��ԭ���������еĵ�ͼ
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
					printf("��Ϸʤ��,������ѡ��ؿ�(1/2)(0:�˳���Ϸ):");
					break;
				}
			}
		}
		else 
		{
			printf("������������������\n");
			printf("��ѡ��ؿ�����1/2��(0:�˳���Ϸ):");
		}
	}
	
	
	return 0;
}

/*********************************************************************************
* Function Name    �� map_layout��ͼ���ֺ���
* Parameter		   �� array����ά����  m��һά����  n����ά����
* Return Value     �� void 
* Function Explain �� 
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
				case 0: set_pattern("  ", 0xF); break;		//�����ַ�����ͨ�ַ�������
				case 1: set_pattern("��", 8);    break;
				case 2: set_pattern("��", 4);    break;
				case 3: set_pattern("��", 3);    break;
				case 4: set_pattern("��", 0xE); break;
				case 5: set_pattern("��", 6);   break;
				case 6: set_pattern("��", 3);    break;
				default: 						break;
			}
		}
		printf("\n");
	}
}

/*********************************************************************************
* Function Name    �� set_color������ɫ
* Parameter		   �� color����ɫֵ
* Return Value     �� void 
* Function Explain �� 
**********************************************************************************/
void set_color(short int color)
{
	HANDLE hConsole  = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

/*********************************************************************************
* Function Name    �� set_pattern����ͼ��
* Parameter		   �� str�������ַ�  color����ɫֵ
* Return Value     �� void 
* Function Explain �� 
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
* Function Name    �� control_move����С���ƶ�
* Parameter		   �� x1,y1:��ǰ���ͼ��������  x2,y2:����ǰ���ͼ��������
* Return Value     �� void 
* Function Explain �� 
**********************************************************************************/
/*
	1����λ�ڿո񴦣�
	��1����ǰ�����ӣ������ڿո��ϣ�����ǰ���ǿո�
	��2����ǰ�����ӣ������ڿո��ϣ�����ǰ����Ŀ��λ��
	��3����ǰ�����ӣ�������Ŀ��λ���ϣ�Ŀ��λ��ǰ���ǿո�
	��4����ǰ�����ӣ�������Ŀ��λ���ϣ�Ŀ��λ��ǰ����Ŀ��λ��
	��5����ǰ�ǿո�
	��6����ǰ��Ŀ��λ��
	2����λ��Ŀ��λ�ô���ͬ��

*/
void control_move(int x1, int y1, int x2, int y2)
{
	//��λ�ڿո�
	if( map[x][y] == 3 )
	{
		//��ǰ�����ӣ������ڿո���
		if( map[x1][y1] == 2 )
		{
			//����ǰ���ǿո�
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 0;
				map[x1][y1] = 3;
				map[x2][y2] = 2;
			}
			//����ǰ����Ŀ��λ��
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 0;
				map[x1][y1] = 3;
				map[x2][y2] = 5;
			}
		}
		//��ǰ�����ӣ�������Ŀ��λ����
		if( map[x1][y1] == 5 )
		{
			//����ǰ���ǿո�
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 0;
				map[x1][y1] = 6;
				map[x2][y2] = 2;
			}
			//����ǰ����Ŀ��λ��
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 0;
				map[x1][y1] = 6;
				map[x2][y2] = 5;
			}
		}
		//��ǰ�ǿո�
		if( map[x1][y1] == 0 )
		{
			map[x][y] = 0;
			map[x1][y1] = 3;
		}
		//��ǰ��Ŀ��λ��
		if( map[x1][y1] == 4 )
		{
			map[x][y] = 0;
			map[x1][y1] = 6;
		}
	}
	
	//��λ��Ŀ��λ��
	if( map[x][y] == 6 )
	{
		//��ǰ�����ӣ������ڿո���
		if( map[x1][y1] == 2 )
		{
			//����ǰ���ǿո�
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 4;
				map[x1][y1] = 3;
				map[x2][y2] = 2;
			}
			//����ǰ����Ŀ��λ��
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 4;
				map[x1][y1] = 3;
				map[x2][y2] = 5;
			}
		}
		//��ǰ�����ӣ�������Ŀ��λ����
		if( map[x1][y1] == 5 )
		{
			//����ǰ���ǿո�
			if( map[x2][y2] == 0 )
			{
				map[x][y] = 4;
				map[x1][y1] = 6;
				map[x2][y2] = 2;
			}
			//����ǰ����Ŀ��λ��
			if( map[x2][y2] == 4 )
			{
				map[x][y] = 4;
				map[x1][y1] = 6;
				map[x2][y2] = 5;
			}
		}
		//��ǰ�ǿո�
		if( map[x1][y1] == 0 )
		{
			map[x][y] = 4;
			map[x1][y1] = 3;
		}
		//��ǰ��Ŀ��λ��
		if( map[x1][y1] == 4 )
		{
			map[x][y] = 4;
			map[x1][y1] = 6;
		}
	}
}

/*********************************************************************************
* Function Name    �� find_manѰ���˵�λ��
* Parameter		   �� void
* Return Value     �� void 
* Function Explain �� 
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
* Function Name    �� finish�ж��Ƿ񴳹سɹ�
* Parameter		   �� void
* Return Value     �� SUCCESS�����سɹ�   FAILURE����δ�ɹ�
* Function Explain �� 
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
* Function Name    �� print_note��ӡ�����Ϸ����
* Parameter		   �� void
* Return Value     �� void
* Function Explain �� 
**********************************************************************************/
void print_note(void)
{
	printf("**************��������Ϸ��ӭ��***************\n"
	"*                                           *\n"
	"*                                           *\n"
	"*             ��:�Լ���λ��                 *\n"
	"*             ��:����Ҫ���ĵ�λ��           *\n"
	"*             ��:���ӵ�λ��                 *\n"
	"*                                           *\n"
	"*         |       ��Ϸ����       |          *\n"
	"*                                           *\n"
	"*          ���ơ⽫���Ƶ����λ��           *\n"
	"*          �� wasd ���ơ���ƶ�             *\n"
	"*          ��   r  ���¿�ʼ��Ϸ             *\n"
	"*          ��   q      �˳���Ϸ             *\n"
	"*********************************************\n");
	set_color(0xA);
	printf("��ѡ��ؿ�����1/2��(0:�˳���Ϸ):");
}