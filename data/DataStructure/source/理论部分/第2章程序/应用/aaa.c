/* ==================Program Description================== */
/* 程序名称：joseph.c                                     */
/* 程序目的：用链表实现约瑟夫环问题                        */
/* Written By Yang yonghu                                  */
/* ========================================================*/  

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
typedef struct node
{
    int number; /* 人的序号 */
    int cipher; /* 密码 */
    struct node *next; /* 指向下一个节点的指针 */
}jnode;

jnode *CreatList(int num) /* 建立循环链表 */
{
    int i;
    jnode *ptr1,*head;

    if((ptr1=(jnode *)malloc(sizeof(jnode)))==NULL)
    {
        perror("malloc");
        return ptr1;
    }
    head=ptr1;
    ptr1->next=head;
    for(i=1;i<num;i++)
    {
        if((ptr1->next=(struct node *)malloc(sizeof(struct node)))==NULL)
        {
            perror("malloc");
            ptr1->next=head;
            return head;
        }
        ptr1=ptr1->next;
        ptr1->next=head;
    }
    return head;
}
void print(jnode *h)
{
 jnode *p;
 p=h;
 while(p->next!=h)
 {printf("number:%d\t",p->number);
  printf("cipher:%d\n",p->cipher);
  p=p->next;
 }
}
main()
{
    int i,n=20,m; /* 人数n为20个 */
    jnode *head,*ptr;
    srand(time(NULL));
    head=CreatList(n);

    for(i=1;i<=n;i++)
    {
        head->number=i;
        head->cipher=rand()%20+1;
        head=head->next;
    }
    m=rand()%20+1; /* m取随机数 */
    i=1;
      printf("the first m:%d\n",m);
    ptr=head;
    while(head->next!=ptr)
    {
    head=head->next;
    }
    while(head->next!=head) /* 当剩下最后一个人时，退出循环 */
    {
        if(i==m)
        {
            ptr=head->next; /* ptr记录数到m的那个人的位置 */
            printf("number:%d\t",ptr->number);
            printf("cipher:%d\n",ptr->cipher);
            m=ptr->cipher; /* 让m等于数到m的人的密码 */
            head->next=ptr->next; /* 让ptr从链表中脱节，将前后两个节点连接起来 */

            free(ptr); /* 释放ptr指向的内存 */
            i=1; /* 将i重新置为1，从1再开始数 */
        }
        else
        {
            head=head->next;
            i++;
        }
    }
    printf("number:%d\t",head->number);
    printf("cipher:%d\n",head->cipher);
    free(head); /* 让最后一个人也出列 */
  
}

