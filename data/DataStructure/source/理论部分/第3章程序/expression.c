 /*==================================================================*/
 /*=== 程序名称： Stack_03.c                    =====*/
 /*=== 程序目的： 计算中序表达式（堆栈应用）            =====*/
 /*==================================================================*/
#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#define MAXSIZE 10


typedef  struct
  {char  data[MAXSIZE];
   int  top;
  }SeqStack;


SeqStack  *Init_SeqStack()  /*置空栈*/
{
  SeqStack  *s;
  s=malloc(sizeof(SeqStack));
  s->top=0;  return s;
}

 /*------------------------------------------------------------------*/
 /* 存入堆栈数据                             */
 /*------------------------------------------------------------------*/
int Push_SeqStack (SeqStack *s, char  x)    /* 入栈*/
{
  if (s->top==MAXSIZE-1)  return 0; /*栈满不能入栈*/
  s->data[s->top]=x;
  s->top++;

  return 1;

}


 /*------------------------------------------------------------------*/
 /* 从堆栈中取出数据                         */
 /*------------------------------------------------------------------*/
int  Pop_SeqStack(SeqStack *s, char *x)
{  if  (Empty_SeqStack ( s ) )  return 0; /*栈空不能出栈 */
   else  {
          s->top--;
          *x=s->data[s->top];

          return 1; }        /*栈顶元素存入*x，返回*/
}

 /*------------------------------------------------------------------*/
 /* 检查堆栈是否为空                         */
 /*------------------------------------------------------------------*/
int Empty_SeqStack(SeqStack *s) /*判空栈*/
{
   if (s->top==0)  return 1;
   else  return 0;
}
 /*------------------------------------------------------------------*/
 /* 判断是否为运算符                         */
 /*------------------------------------------------------------------*/
int is_operator(char operator)
{
    switch(operator)
    {
        case '+': case '-': case '*': case '/':return 1; /*是操作符*/
        default: return 0; /*不是操作符*/
    }
}

 /*------------------------------------------------------------------*/
 /* 判断运算符的优先权                       */
 /*------------------------------------------------------------------*/
int priority(char o)
{
    switch(o)
    {
        case '+': case '-': return 1; /*"＋"与"－"的运算符优先权值为1*/
        case '*': case '/': return 2; /*"*"与"/"的运算符优先权值为2*/
        default: return 0;
    }
}

 /*------------------------------------------------------------------*/
 /* 计算任两个操作数的值                         */
 /*------------------------------------------------------------------*/
int two_result(int operator, int operand1, int operand2)
{
    switch(operator)
    {
        case '+': return(operand2 + operand1);
        case '-': return(operand2 - operand1);
        case '*': return(operand2 * operand1);
        case '/': return(operand2 / operand1);
    }
}

 /*------------------------------------------------------------------*/
 /* 主程序：可输入输出堆栈数据，并输出堆栈内容           */
 /*------------------------------------------------------------------*/
void main()
{
    SeqStack *s_operand,*s_operator;
    char expression[50]; /*声明表达式字符串数组*/
    int position = 0; /*表达式中元素位置*/
    int op = 0; /*运算符*/
    int operand1 = 0; /*前操作数*/
    int operand2 = 0; /*后操作数*/
    int evaluate = 0; /*运算结果*/

    s_operand=Init_SeqStack();
    s_operator=Init_SeqStack();

    printf("\nplease input the inorder expression:");
    gets(expression);

    while(expression[position]!='\0'&& expression[position]!='\n')
    {
        if(is_operator(expression[position]))
        {
            if(!Empty_SeqStack(s_operator))
        	{
                while(!Empty_SeqStack(s_operator)&&priority(expression[position]) <= priority(s_operator->data[s_operator->top]))
            	{
                    /*从堆栈中取出两个操作数和一个运算符*/
                    Pop_SeqStack(s_operand, &operand1);
                    Pop_SeqStack(s_operand, &operand2);
                    Pop_SeqStack(s_operator, &op);
                    Push_SeqStack (s_operand, two_result(op,operand1,operand2));
            	}
        	}
            /*当前元素存入运算符堆栈*/
            Push_SeqStack (s_operator,expression[position]);


        }
        else/*存入操作数堆栈－－须做Ascii码转换*/
            Push_SeqStack (s_operand,expression[position]-48);


        position++;/*表达式中下一个元素*/
    }

    /*取出运算符堆栈的内容*/
    while(!Empty_SeqStack(s_operator))
    {
        Pop_SeqStack(s_operator, &op);
        Pop_SeqStack(s_operand, &operand1);
        Pop_SeqStack(s_operand, &operand2);
        /*计算后将结果存入堆栈*/
        Push_SeqStack (s_operand,two_result(op,operand1,operand2));

    }
    /*取出表达式最终结果*/
    Pop_SeqStack(s_operand, &evaluate);
    printf("The expression [%s] result is '%d'",expression,evaluate);
    getch();
}

