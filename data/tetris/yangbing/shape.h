#ifndef TEST_H_
#define TEST_H_

#define W 20
#define H 20
struct termios old,new;
char wall[W][H];

void wall_init();
void init_frame(void );
void print_shape(int shape[][16]);
void fun_alarm(int );
void save_shape();
void trans();
int can_trans(int );
void left_move();
void right_move();
void down_move();
int can_left();
int can_right();
int can_down();
int can_press_down();
void cut_level();
void can_cut(int );
void set_color();
void set_shape_num();
void if_over();
void next_shape();
void turn_shape(char );
void save_in_wall();
void system_pause();
void attack_down();
#endif /* TEST_H_ */
