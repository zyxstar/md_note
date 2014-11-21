#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include "shape.h"



int main()
{
	char ch;


		tcgetattr(0,&old);
		tcgetattr(0,&new);
		new.c_lflag = new.c_lflag & ~(ICANON | ECHO);
		new.c_cc[VTIME] = 0;
		new.c_cc[VMIN] = 1;
		tcsetattr(0,TCSANOW,&new);
		wall_init();

		signal(SIGALRM,fun_alarm);
		alarm(1);
		while(1)
		{
			ch = getchar();
			turn_shape(ch);
			fflush(NULL);
		}

		return 0;
}
