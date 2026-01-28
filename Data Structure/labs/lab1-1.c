#/*
 * 实验内容一：线性表的链式实现
 *  - 完成线性表抽象数据类型 ADT 的链式存储的设计与实现
 *  - 利用函数指针调用不同的遍历访问函数
 */

#include<stdio.h>
#include<stdlib.h>

#define elemtype int
#define _DEBUG_ 1

typedef struct node
{
    elemtype data;
    struct node*next;
}linknode,*linklist;//

linknode *first;//这有个全局变量

//初始化
linknode *initlist()//*放在这个位置。。。
{
    linknode*p;
    p=(linknode*)malloc(sizeof(linknode*));
    if(!p)
    {
        printf("初始化头节点失败");
        exit(0);//立即终止程序执行
    }
    p->next=NULL;//
    return p;
}

#ifdef _DEBUG_

linknode head;

void *initlist1(linknode *p)
{
    p=(linknode*)malloc(sizeof(linknode*));
    if(!p)
    {
        printf("初始化头节点失败");
        exit(0);//立即终止程序执行,使用exit()前必须包含stdlib.h
    }
    p->next=NULL;//
}

void initlist2(linknode *head)
{
    first=&head;
    first->next=NULL;
}

#endif

/*int listlength0(linknode p)//linknode是 struct node的别名，​​而不是指针​​;
{
    int m=0;
    while(p)//调用函数时，系统需要将整个 struct node结构体的内容复制一份给形参 p。
    如果结构体很大（例如包含很多数据），这会产生不必要的内存拷贝开销
    {
        m++;
        p=p.next;//p是一个结构体变量，不是指针,对于结构体变量，访问成员应该用点运算符，即p.next;
        p.next的类型是 struct node *（一个指针）。你不能把一个指针赋值给一个结构体变量
    }
    return m;
}*/
//求链表长度
int listlength(linklist p)
{
    int count=0;
    while(p->next!=NULL)//有哨兵结点的话是p->next
    {
        count++;
        p=p->next;
    }
    return count;
}
//判断是否空表
int listempty(linklist p)
{
    if(p->next!=NULL)
        return 1;//非空表
    return 0;
}
//清空链表（只保留头节点）....头节点是第一个存储数据的结点，哨兵结点不存储数据
void clearlist(linklist p)
{
    linklist q;
    while(p->next!=NULL)
    {
        q=p->next;
        p->next=q->next;
        free(q);
    }
}
//销毁链表
void destroylist(linklist p)
{
    clearlist(p);
    free(p);
}
//获取特定元素
int getelem(linklist p,int i,elemtype *e)
{
    int k=0;
    while(p=p->next)//​​赋值表达式（=）的值就是被赋给左侧变量的那个值​​。p=p->next的值是 p->next的值，即下一个节点的内存地址
    {
        k++;
        if(k==i)
        {
            *e=p->data;
            return k;//如果找到了，返回值就非0哎
        }
    }
    return 0;
}
//查找特定元素
linknode *locatelem(linklist p,elemtype e)//一般linklist指代链表（头指针），但是linknode *指一个节点（本质一样）
{
    while(p=p->next)
        if(p->data==e)
            return p;//返回所找元素（所在结构体？）的地址哎
    return NULL;
}
//查找上一个节点
linknode *priorelem(linklist p,linknode *e)
{
    for(;p->next;p=p->next)
        if(p->next==e)
            return p;
    return NULL;
}
//查找下一个节点
linknode *nextelem(linklist p,linknode *e)
{
    return e->next;
}
//插入节点
linknode *listinsert(linklist p,int i,elemtype e)
{
    if(i<=0)
        return NULL;
    linknode *q;
    for(;p;p=p->next)
    {
        if(--i<1)
        {
            q=(linknode*)malloc(sizeof(linknode*));
            if(q==NULL)
            {
                printf("内存空间分配失败");
                exit(0);
            }       
            q->next=p->next;
            p->next=q;
            q->data=e;
            return q;
        }
    }
    return NULL;
}
//删除节点
int listdelete(linklist p,int i,elemtype *e)//e来保留一下删除的数据
{
    if(i<1)
        return 0;
    linknode *q;
    q=p;
    for(p=p->next;p;p=p->next)
    {
        if(--i<1)
        {
            q->next=p->next;
            *e=p->data;
            free(p);
            return 1;
        }
        q=p;
    }
    return 0;
}
//打印结点数据
void printlinknode(linknode *p)
{
    printf("%d\n",p->data);
}
//每个节点的数据+2
void add2(linknode *p)
{
    p->data+=2;//p->data=p->data+2
}
//调试链表结点
void debuglinknode(linknode *p)
{
    printf("结点-(*addr)=value :  ");//表示接下来信息的输出形式
    printf("(*%lx)=%d\n", p, p->data);//%lx表示以一个​​小写十六进制长整数​​的格式来打印指针 p的值
}
//遍历链表（并执行ptrfun这个函数？
void listraverse(linklist p,void (*ptrfunc)(linknode *ptr))
{
    printf("链表的长度是%d\n",listlength(p));
    while(p=p->next)
        (*ptrfunc)(p);
    printf("\n");
}
//主函数
int main()
{
    void (*ptrfunc)(linknode *p)=debuglinknode;//函数指针变量名ptrfunc

    first=initlist();

    listraverse(first,ptrfunc);
    listinsert(first,1,2);
    listinsert(first,1,3);
    listinsert(first,1,4);
    listinsert(first,1,5);
    listraverse(first,ptrfunc);
    listinsert(first,1,6);
    listinsert(first,1,7);
    listinsert(first,1,8);
    listinsert(first,1,9);
    listraverse(first,ptrfunc);
    listinsert(first,1,666);
    listinsert(first,1,777);
    listinsert(first,1,888);
    listinsert(first,2,999);
    listraverse(first,ptrfunc);

    elemtype ei;
    printf("读取数据前 %d\n",ei);
    getelem(first,10,&ei);
    printf("读取数据为 %d\n",ei);
    printf("\n");

    //查找特定结点
    linknode *q;
    q=locatelem(first,888);
    if(!q)
        printf("未查找到该结点\n");
    else
    {
        q=priorelem(first,q);
        printf("找到结点的前驱是%d\n",q->data);
        printf("找到的结点是%d\n",q->next->data);
        if(q->next->next)
            printf("找到的结点的后继是%d\n",q->next->next->data);
    }

    //删除结点
    printf("删除前的值是 %d\n",ei);
    if(listdelete(first,15,&ei)>0)
        printf("删除的值为 %d\n",ei);
    else
        printf("删除失败 %d\n",ei);

    printf("删除前的值是 %d\n",ei);
    if(listdelete(first,10,&ei)>0)
        printf("删除的值为 %d\n",ei);
    else
        printf("删除失败 %d\n",ei);

    printf("删除前的值是 %d\n",ei);
    if(listdelete(first,6,&ei)>0)
        printf("删除的值为 %d\n",ei);
    else
        printf("删除失败 %d\n",ei);

    //+2
    ptrfunc=add2;
    printf("每个元素准备+2\n");
    listraverse(first,ptrfunc);
    printf("完成+2后新的链表：");
    listraverse(first,printlinknode);
    
    destroylist(first);
    
    return 0;
}