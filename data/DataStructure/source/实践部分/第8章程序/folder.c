#include<stdio.h>
#include<malloc.h>
#include<string.h>

struct Folder;

typedef struct Node
{
	int tag;
	union 
	{
		char filename[16];
		struct Folder* folderp;
	};
 	struct Node* next;
} Fnode;

typedef struct Folder
{
	struct Folder* parents_ptr;
	char foldername[16];
	Fnode* head,*tail;
} Folder;

Fnode* F_makeNode(int t,char* filename,Folder *folderp)
{
	Fnode* new_ptr= (Fnode*)malloc(sizeof(Fnode));
	new_ptr->tag=t;
	if(t==0)
		strncpy(new_ptr->filename,filename,16);
	else
		new_ptr->folderp=folderp;
	new_ptr->next=0;
	return new_ptr;
}



void F_init(Folder * p_folder,char *foldername ,Folder* parents_ptr)
{
	p_folder->head=p_folder->tail=0;
	strncpy(p_folder->foldername,foldername,16);
	p_folder->parents_ptr=parents_ptr;
}
void F_clear(Folder * p_folder)
{
	Fnode* cur,*del;
	if(p_folder==0)
		return ;
	cur=p_folder->head;
	while(cur)
	   if(cur->tag==0)
		{
			del=cur;
			cur=cur->next;
			free(del);
		}
		else
		{
			F_clear(cur->folderp);
			free(cur->folderp);
			del=cur;
			cur=cur->next;
			free(del);
		}	
}

void F_insertAtTail(Folder * p_folder,int t,char* filename,Folder *folderp)
{
	Fnode* new_ptr=F_makeNode(t,filename,folderp);
	if(p_folder->tail)
	{
	   p_folder->tail->next=new_ptr;
	   p_folder->tail=new_ptr;
	}
    else
    	p_folder->tail=p_folder->head=new_ptr;
}

void F_makeFolder(Folder * p_folder,char *foldername)
{
	Folder * new_ptr=(Folder *)malloc(sizeof(Folder));
	F_init(new_ptr,foldername,p_folder);
	F_insertAtTail(p_folder,1,0,new_ptr);
}

void F_makeFile(Folder * p_folder,char *filename)
{
	F_insertAtTail(p_folder,0,filename,0);
}

void F_dir(Folder * p_folder)
{
	unsigned i=0;
	Fnode* cur=p_folder->head;
	while(cur)
	{   
		if(cur->tag)
		   printf("dir:%-16s",cur->folderp->foldername);
		else
			printf("%-20s",cur->filename);
		++i;
		if(i%3==0)
			printf("\n");
		cur=cur->next;
	}
	printf("\n");
}
void F_printPath(Folder * p_folder)
{
	Folder * root=p_folder;
	Folder * arr[100];
	unsigned len=0;
	while(p_folder)
	{
		arr[len++]=p_folder;
		p_folder=p_folder->parents_ptr;
	}
	while(len)
		printf("%s/",arr[--len]->foldername);
	
	printf("\n");
}
int F_cd(Folder ** cur_folder,char* foldername)
{
	Fnode* cur=(*cur_folder)->head;
	while(cur)
	{
		if(cur->tag&&strcmp(cur->folderp->foldername,foldername)==0)
		{
			*cur_folder=cur->folderp;
			return 1;
		}
		cur=cur->next;
	}
	return 0;
}
int F_return(Folder ** cur_folder)
{
	if((*cur_folder)->parents_ptr)
	{
		*cur_folder=(*cur_folder)->parents_ptr;
		return 1;
	}
	return 0;
}

void test()
{
	Folder root;
	Folder* cur;
	char order[16],para[16];
	F_init(&root,"root",0);
	cur=&root;
	
	while(1)
	{
		printf("%s>",cur->foldername);
		scanf("%s",order);
		if(strcmp(order,"md")==0)
		{
			scanf("%s",para);
			F_makeFolder(cur,para);
			continue;
		}
		if(strcmp(order,"mf")==0)
		{
			scanf("%s",para);
			F_makeFile(cur,para);
			continue;
		}
		if(strcmp(order,"dir")==0)
		{
			F_dir(cur);
			continue;
		}
		if(strcmp(order,"path")==0)
		{
			F_printPath(cur);
			continue;
		}
		if(strcmp(order,"cd")==0)
		{
			scanf("%s",para);
			if(!F_cd(&cur,para))
				printf("目录不存在\n");
			continue;
		}
		if(strcmp(order,"cd..")==0)
		{
			if(! F_return(&cur))
				printf("已经是根目录\n");
			continue;
		}
		
		if(strcmp(order,"exit")==0)
		    break;
		printf("命令错误\n");

	}
	F_clear(&root);
}
int main()
{
	test();
	return 0;
}