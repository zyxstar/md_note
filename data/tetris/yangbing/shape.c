#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include "shape.h"

int shape_num = 0;
int down_one = 0;
int move_left =0;
int move_right = 0;
int move_down =0;
int score_num = 0;
int down_init = 0;
int next_shape_num = 5;
int color_num = 44;
int press_pause = 1;

int shape[19][16] =
{
		{0, 8, 0, 9, 0, 10, 0, 11,           	    //  方块           // 8 10 12 14
		   1, 8, 1, 9, 1, 10, 1, 11},

		   {0,6,0,7,0,8,0,9,					 	 //横 一
		      0,10,0,11,0,12,0,13
		   },

		   { 0,8,0,9,1,8,1,9,		   				//竖 一
			2,8,2,9,3,8,3,9
		   },

		   {0,8,0,9,1,8,1,9,1,10,1,11,
		     2,10,2,11              					// 反竖Z

		   },

		 {	0, 10, 0,11, 0,12, 0, 13,        		 //反横Z
	     	1, 8, 1, 9, 1, 10, 1, 11},

	     { 0,8,0,9,0,10,0,11,                      //横Z
	     	1,10,1,11,1,12,1,13
	     },

		{0,10,0,11,                                     //竖Z   6
				1,8,1,9,1,10,1,11,
				2,8,2,9
		},

		{0,10,0,11,                                    //上山     7
				1,8,1,9,1,10,1,11,1,12,1,13
		},

		{0,10,0,11,1,8,1,9,1,10,1,11,           //左山     8
					2,10,2,11
			},

		{0,8,0,9,0,10,0,11,0,12,0,13,        //下山
				1,10,1,11
		},

		{0,8,0,9,1,8,1,9,1,10,1,11,             //右山
				2,8,2,9
		},

		{0,8,0,9,1,8,1,9,                           //正L右
				2,8,2,9,2,10,2,11
		},

		{0,12,0,13,							       //正L上
				1,8,1,9,1,10,1,11,1,12,1,13
		},

		{0,8,0,9,0,10,0,11,                       //正L左
				1,10,1,11,
				2,10,2,11
		},

		{0,8,0,9,0,10,0,11,0,12,0,13,         //正L下
				1,8,1,9
		},

		{0,10,0,11,                                     //_|
				1,10,1,11,
				2,8,2,9,2,10,2,11
		},

		{0,8,0,9,0,10,0,11,0,12,0,13,          //—.
				1,12,1,13

		},

		{0,8,0,9,0,10,0,11,                         //|-
				1,8,1,9,
				2,8,2,9

		},

		{0,8,0,9,                                      //|___
				1,8,1,9,1,10,1,11,1,12,1,13
		}
};
																		//初始化 数组
void wall_init()
{
	memset(wall, 0, sizeof(wall));
}

void init_frame(void )
{
	int i, j;
	int k, l;

	printf("\33[2J\033[1;1H");							//每次都清屏

	for (i = 0; i < H + 5; i++)						//每次都打印边框
		{
			for (j = 0; j < 10 + W; j++)
				{
				if (((j > 10 && j < W + 10) && i == 3)
				|| ((j > 10 && j < W + 10) && i == H + 5 - 1)
				|| (j == 10 + 1 && (i > 3 && i < H + 5 - 1))
				|| (j == W + 10 - 1 && (i > 3 && i < H + 5 - 1))
				|| (j == 22 && (i > 3 && i < H + 5 - 1)))
				{
					printf("\033[37;41m  \033[0m");
				}
					else
				{
						printf("  ");
				}
		}
			printf("\n");
}
																										//边框 分数 信息
		printf("\033[33m\033[%d;%dH @iceyang\n\033[0m",3,48);
		printf("\033[33;4m\033[%d;%dH   NEXT  \n\033[0m",5,48);
		printf("\033[33;4m\033[%d;%dH  score  \n\033[0m",11,48);
		printf("\033[31m\033[%d;%dH%d\n\033[0m",13,52,score_num);

		printf("\033[34m\033[%d;%dH 发炮: f  \n\033[0m",15,48);
		printf("\033[31m\033[%d;%dH 变形: w  \n\033[0m",16,48);
		printf("\033[31m\033[%d;%dH 向下: s  \n\033[0m",17,48);
		printf("\033[31m\033[%d;%dH 向左: a  \n\033[0m",18,48);
		printf("\033[31m\033[%d;%dH 向右: d  \n\033[0m",19,48);
		printf("\033[31m\033[%d;%dH 暂停: p  \n\033[0m",20,48);
		printf("\033[31m\033[%d;%dH 退出: q  \n\033[0m",21,48);

		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num + 1,shape[next_shape_num][0] + 3 + 4, shape[next_shape_num][1] + 25 + 17 );
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][2] + 3 + 4, shape[next_shape_num][3] + 25+ 17);
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][4] + 3+ 4, shape[next_shape_num][5] + 25+ 17);
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][6] + 3+ 4 , shape[next_shape_num][7] + 25+ 17);
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][8] + 3 + 4, shape[next_shape_num][9] + 25+ 17);
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][10] + 3+ 4 , shape[next_shape_num][11] + 25+ 17);
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][12] + 3+ 4, shape[next_shape_num][13] + 25+ 17);
		printf("\033[%dm\033[%d;%dH \n\033[0m",color_num+ 1,shape[next_shape_num][14] + 3 + 4, shape[next_shape_num][15] + 25+ 17);

																							// 打印已经存入wall的，位置信息，并且随着 init 打印出来
	for(k = 0; k < 20; k++)
	{
		for(l = 0; l < 20; l++)
		{
			if(wall[k][l] == '#')
			printf("\033[43m\033[%d;%dH#\n\033[0m", k + 3, l + 25 );
		}
	}
}
																		//触发每秒执行一次
void fun_alarm(int signo)
{

	 print_shape(shape);  //判断随机落下的形状
	 alarm(press_pause);
}

																		//在框架里打印 并且 下降shape
void  print_shape(int shape[][16])
{
	  init_frame();//每次打印先打印边框和里面已经落下的形状

	  if(press_pause == 0)
	  		{
	  			printf("\033[33;4m\033[%d;%dH      P A U S E     \n\033[0m",12,25);
	  		}

		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][0] + 3 + down_one + move_down, shape[shape_num][1] + 25 - move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][2] + 3 + down_one + move_down, shape[shape_num][3] + 25- move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][4] + 3 + down_one + move_down, shape[shape_num][5] + 25- move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][6] + 3 + down_one + move_down, shape[shape_num][7] + 25- move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][8] + 3 + down_one + move_down, shape[shape_num][9] + 25- move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][10] + 3 + down_one + move_down, shape[shape_num][11] + 25- move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][12] + 3 + down_one + move_down, shape[shape_num][13] + 25- move_left + move_right);
		printf("\033[?25l\033[%dm\033[%d;%dH \n\033[0m",color_num,shape[shape_num][14] + 3 + down_one + move_down, shape[shape_num][15] + 25- move_left + move_right);

   if(can_down() && (press_pause != 0))   //停止条件 ,不能继续下降条件
   	   {
	   down_one ++;
   	   }
	 else if((!can_down()) && (press_pause != 0))              // 存入wall中,并且将全局变量初始化
   	   {
		 	 save_in_wall();
   	   }

}
void save_in_wall()   //存入wall并且初始化
{
					wall[shape[shape_num][0] + down_one + move_down][shape[shape_num][1] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][2] + down_one + move_down][shape[shape_num][3] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][4] + down_one + move_down][shape[shape_num][5] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][6] + down_one + move_down][shape[shape_num][7] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][8] + down_one + move_down][shape[shape_num][9] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][10] + down_one + move_down][shape[shape_num][11] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][12] + down_one + move_down][shape[shape_num][13] -move_left + move_right] = '#';
			 	 wall[shape[shape_num][14] + down_one + move_down][shape[shape_num][15] -move_left + move_right] = '#';

			 	  cut_level(); 				//消行
				  if_over();						//是否OVER

			 	 	shape_num = 3;
			   down_one = 0;
			   move_left = 0;
			   move_right = 0;
			   move_down = 0;
			   shape_num = next_shape_num;
			   next_shape();
			   down_init = 1;
			   set_color();
}

void cut_level()                //消层
{
	int i, j,  num = 0;
	for(i = 0; i < 20; i++)
	{
		for(j = 0; j< 20; j++)
		{
				if(wall[i][j] == '#')
				{
					 num =  num + 1;
				}
		}
		if(num == 20 )
		{
			can_cut(i);
			score_num = score_num + 1;

		}
		num = 0;
	}
}

void can_cut(int l)     //消层 条件
{
	int i, j, k;
	for(i = l ; i >= 0; i -- )
	{
		if(i == 0)
		{
			for(k = 0; k < 20; k ++)
			{
				wall[0][k] = '0';
			}
		}
		else
		{
		for(j = 0; j < 20; j++)
		{
			 wall[i][j] = wall[i-1][j];
		}
		}
	}
}

void turn_shape(char ch)    //变形
{
	switch(ch){
			case 'w':
				trans();break;
			case 'a':
				left_move();break;
			case 'd':
				right_move();break;
			case 's':
				down_move();break;
			case 'p':
				system_pause();break;
  			case 'q':
  				printf("\033[33;4m\033[%d;%dH       Q U I T!     \n\033[0m",12,25);
  									sleep(2);
  									printf("\33[2J\033[1;1H");
  									tcsetattr(0,TCSANOW,&old);
  									exit(1);break;
			case 'f': 
				attack_down();break;
			}
}
void trans()              //变形实现
{
	if(shape_num == 0)
	{
		return;
		down_one--;
	}

	if(shape_num == 1 || shape_num == 2 ||  shape_num == 3 ||  shape_num == 4 ||  shape_num == 5 ||  shape_num == 6)
	{
		if(shape_num == 1 || shape_num ==  3 || shape_num ==  5)
		{
			if(can_trans(+1))
									{
									shape_num = shape_num +1;
									down_one--;
									}
						else
									return;
		}
		else
		{
			if(can_trans(-1))
			{
			shape_num = shape_num -1;
			down_one--;
			}
			else
			return;
		}
	}
	if(shape_num >= 7 && shape_num   <= 10)
	{
		if(shape_num == 7 ||shape_num == 8 || shape_num == 9)
		{
			if(can_trans(+1))
						{
						shape_num = shape_num +1;
						down_one--;
						}
			else
						return;
		}
	  else
		{
		  if(can_trans(-3))
		  			{
		  			shape_num = shape_num -3;
		  			down_one--;
		  			}
		  else
		  			return;

		}

	}

	if(shape_num >= 11 && shape_num   <= 14)
		{
			if(shape_num == 11 ||shape_num == 12 || shape_num == 13)
			{
				if(can_trans(+1))
										{
										shape_num = shape_num +1;
										down_one--;
										}
							else
										return;
			}
		  else
			{
			  if(can_trans(-3))
			  						{
			  						shape_num = shape_num -3;
			  						down_one--;
			  						}
			  			else
			  						return;
			}

		}
	if(shape_num >= 15 && shape_num   <= 18)
			{
				if(shape_num == 15 ||shape_num == 16 || shape_num == 17)
				{
					if(can_trans(+1))
											{
											shape_num = shape_num +1;
											down_one--;
											}
								else
											return;
				}
			  else
				{
				  if(can_trans(-3))
				  						{
				  						shape_num = shape_num -3;
				  						down_one--;//移动过程中立即调用print_shape 会down_one++ ,所以这里首先要 --
				  						}
				  			else
				  						return;
				}

			}

	print_shape(shape);

}

int can_trans(int i)          //变形条件
{
	if((shape[shape_num + i ][1] + 25 - move_left + move_right  >= 24) &&
			  (shape[shape_num+ i][3] + 25 - move_left + move_right  >= 24) &&
			  (shape[shape_num+ i][5] + 25 - move_left + move_right  >= 24) &&
			  (shape[shape_num+ i][7] + 25 - move_left + move_right  >= 24) &&
			  (shape[shape_num+ i][9] + 25 - move_left + move_right  >= 24) &&
			  (shape[shape_num+ i][11] + 25 - move_left + move_right  >= 24)&&
			  (shape[shape_num+ i][13] + 25 - move_left + move_right  >= 24)&&
			  (shape[shape_num+ i][15] + 25 - move_left + move_right  >= 24)&&
			  (wall[shape[shape_num+ i][0] + down_one + move_down ][shape[shape_num+ i][1] -move_left + move_right  ] != '#')&&
			  (wall[shape[shape_num+ i][2] + down_one + move_down ][shape[shape_num+ i][3] -move_left + move_right  ] != '#')&&
			  (wall[shape[shape_num+ i][4] + down_one + move_down ][shape[shape_num+ i][5] -move_left + move_right ] != '#')&&
			  (wall[shape[shape_num+ i][6] + down_one + move_down ][shape[shape_num+ i][7] -move_left + move_right  ] != '#')&&
			  (wall[shape[shape_num+ i][8] + down_one + move_down ][shape[shape_num+ i][9] -move_left + move_right ]  != '#')&&
			  (wall[shape[shape_num+ i][10] + down_one + move_down ][shape[shape_num+ i][11] -move_left + move_right ]  != '#')&&
			  (wall[shape[shape_num+ i][12] + down_one + move_down ][shape[shape_num+ i][13] -move_left + move_right ]  != '#')&&
			  (wall[shape[shape_num+ i][14] + down_one + move_down ][shape[shape_num+ i][15] -move_left + move_right ]  != '#')&&
			  (shape[shape_num+ i][1] + 25 - move_left + move_right  <= 45) &&
			  (shape[shape_num+ i][3] + 25 - move_left + move_right  <= 45) &&
			  (shape[shape_num+ i][5] + 25 - move_left + move_right  <= 45) &&
			  (shape[shape_num+ i][7] + 25 - move_left + move_right  <= 45) &&
			  (shape[shape_num+ i][9] + 25 - move_left + move_right  <= 45) &&
			  (shape[shape_num+ i][11] + 25 - move_left + move_right <= 45)&&
			  (shape[shape_num+ i][13] + 25 - move_left + move_right  <= 45)&&
			  (shape[shape_num+ i][15] + 25 - move_left + move_right  <= 45) && (press_pause == 1)

	)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void left_move()              //左移
{
	if(can_left())
	{
		move_left = move_left + 2;
	print_shape(shape);
	down_one--;//移动过程中立即调用print_shape 会down_one++ ,所以这里首先要 --
	}
	return;
}
void right_move()
{
	if(can_right())
	{
	move_right += 2;
	print_shape(shape);
	down_one--;//移动过程中立即调用print_shape 会down_one++ ,所以这里首先要 --
	}
	return;

}

void down_move()
{
	if(can_down())// && (move_down < 16))
	{
		move_down ++;
	}
	print_shape(shape);
	return;
}

int can_left()
{
  if((shape[shape_num][1] + 25 - move_left + move_right - 2 >= 24) &&
		  (shape[shape_num][3] + 25 - move_left + move_right - 2 >= 24) &&
		  (shape[shape_num][5] + 25 - move_left + move_right - 2 >= 24) &&
		  (shape[shape_num][7] + 25 - move_left + move_right - 2 >= 24) &&
		  (shape[shape_num][9] + 25 - move_left + move_right - 2 >= 24) &&
		  (shape[shape_num][11] + 25 - move_left + move_right - 2 >= 24)&&
		  (shape[shape_num][13] + 25 - move_left + move_right - 2 >= 24)&&
		  (shape[shape_num][15] + 25 - move_left + move_right - 2 >= 24)&&
		  (wall[shape[shape_num][0] + down_one + move_down ][shape[shape_num][1] -move_left + move_right - 1] != '#')&&
		  (wall[shape[shape_num][2] + down_one + move_down ][shape[shape_num][3] -move_left + move_right - 1] != '#')&&
		  (wall[shape[shape_num][4] + down_one + move_down ][shape[shape_num][5] -move_left + move_right - 1] != '#')&&
		  (wall[shape[shape_num][6] + down_one + move_down ][shape[shape_num][7] -move_left + move_right - 1] != '#')&&
		  (wall[shape[shape_num][8] + down_one + move_down ][shape[shape_num][9] -move_left + move_right - 1]  != '#')&&
		  (wall[shape[shape_num][10] + down_one + move_down ][shape[shape_num][11] -move_left + move_right - 1]  != '#')&&
		  (wall[shape[shape_num][12] + down_one + move_down ][shape[shape_num][13] -move_left + move_right - 1]  != '#')&&
		  (wall[shape[shape_num][14] + down_one + move_down ][shape[shape_num][15] -move_left + move_right - 1]  != '#')&& (press_pause == 1)


  )
		  {
	  	 return 1;
		  }
  else
	  return 0;

}

int can_right()
{
	if((shape[shape_num][1] + 25 - move_left + move_right + 2 <= 45) &&
			  (shape[shape_num][3] + 25 - move_left + move_right + 2 <= 45) &&
			  (shape[shape_num][5] + 25 - move_left + move_right + 2 <= 45) &&
			  (shape[shape_num][7] + 25 - move_left + move_right + 2 <= 45) &&
			  (shape[shape_num][9] + 25 - move_left + move_right + 2 <= 45) &&
			  (shape[shape_num][11] + 25 - move_left + move_right+ 2 <= 45)&&
			  (shape[shape_num][13] + 25 - move_left + move_right + 2 <= 45)&&
			  (shape[shape_num][15] + 25 - move_left + move_right + 2 <= 45)&&
			  (wall[shape[shape_num][0] + down_one + move_down ][shape[shape_num][1] -move_left + move_right + 1] != '#')&&
			  (wall[shape[shape_num][2] + down_one + move_down ][shape[shape_num][3] -move_left + move_right + 1] != '#')&&
			  (wall[shape[shape_num][4] + down_one + move_down ][shape[shape_num][5] -move_left + move_right + 1] != '#')&&
			  (wall[shape[shape_num][6] + down_one + move_down ][shape[shape_num][7] -move_left + move_right + 1] != '#')&&
			  (wall[shape[shape_num][8] + down_one + move_down ][shape[shape_num][9] -move_left + move_right + 1]  != '#')&&
			  (wall[shape[shape_num][10] + down_one + move_down ][shape[shape_num][11] -move_left + move_right + 1]  != '#')&&
			  (wall[shape[shape_num][12] + down_one + move_down ][shape[shape_num][13] -move_left + move_right + 1]  != '#')&&
			  (wall[shape[shape_num][14] + down_one + move_down ][shape[shape_num][15] -move_left + move_right + 1]  != '#')&& (press_pause == 1)



	)
			  {
		  	 return 1;
			  }
	  else
		  return 0;
}

int can_down()
{
	if((shape[shape_num][0] + 3 + down_one + move_down +1 <= 22) &&
			(shape[shape_num][2] + 3 + down_one + move_down +1 <= 22) &&
			(shape[shape_num][4] + 3 + down_one + move_down  +1<= 22) &&
			(shape[shape_num][6] + 3 + down_one + move_down  +1<= 22) &&
			(shape[shape_num][8] + 3 + down_one + move_down  +1<= 22) &&
			(shape[shape_num][10] + 3 + down_one + move_down +1 <= 22) &&
			(shape[shape_num][12] + 3 + down_one + move_down +1 <= 22)&&
			(shape[shape_num][14] + 3 + down_one + move_down +1 <= 22)&&
		 	 (wall[shape[shape_num][0] + down_one + move_down + 1][shape[shape_num][1] -move_left + move_right] != '#')&&
		 	 (wall[shape[shape_num][2] + down_one + move_down+ 1][shape[shape_num][3] -move_left + move_right] != '#')&&
		 	 (wall[shape[shape_num][4] + down_one + move_down+ 1][shape[shape_num][5] -move_left + move_right] != '#')&&
		 	 (wall[shape[shape_num][6] + down_one + move_down+ 1][shape[shape_num][7] -move_left + move_right] != '#')&&
		 	 (wall[shape[shape_num][8] + down_one + move_down+ 1][shape[shape_num][9] -move_left + move_right]  != '#')&&
		 	 (wall[shape[shape_num][10] + down_one + move_down+ 1][shape[shape_num][11] -move_left + move_right]  != '#')&&
		 	 (wall[shape[shape_num][12] + down_one + move_down+ 1][shape[shape_num][13] -move_left + move_right]  != '#')&&
		 	 (wall[shape[shape_num][14] + down_one + move_down+ 1][shape[shape_num][15] -move_left + move_right]  != '#') &&(press_pause == 1)
	)
	{
		return 1;
	}
	else
			  return 0;
}

void set_shape_num()
{
	srand(time(0));
	shape_num = rand()%18;
}
void if_over()
{
	if(wall[0][10] == '#')
	{
		printf("\033[33;4m\033[%d;%dH     GAME OVER!     \n\033[0m",12,25);
		sleep(1);
		tcsetattr(0,TCSANOW,&old);
		printf("\33[2J\033[1;1H");
		exit(1);
	}
	else
	{
		return;
	}

}

void next_shape()
{
	next_shape_num = rand()%19;
}

void set_color()    //随机设置
{
	srand(time(0));
  color_num = rand()%6 +41;
}

void system_pause()
{
	if(press_pause == 1)
	{
		printf("\033[33;4m\033[%d;%dH      P A U S E     \n\033[0m",12,25);
		press_pause = 0;

	}
	else
	{
		press_pause = 1;
		signal(SIGALRM,fun_alarm);
		alarm(1);
		printf("\033[33;4m\033[%d;%dH      S T A R T     \n\033[0m",12,25);
	}
}
void attack_down()
{
	int i, line_num; 
	for(i = 0; i < 20; i++)
	{
		if(wall[i][shape[0][1] -move_left + move_right] == '#')
		break;
				 
	}
	printf("attack  %d", i);
	wall[i][shape[shape_num][1] -move_left + move_right] =  '0';
	wall[i][shape[shape_num][3] -move_left + move_right] =  '0';
	down_one --; 
	print_shape(shape);

	
}
