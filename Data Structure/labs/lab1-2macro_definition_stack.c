#/*
 * 实验内容二：实现顺序栈
 *  - 完成顺序栈抽象数据类型
 *  - ADT 的链式存储的设计与实现
 *  - 使用宏定义实现可以同时定义字符栈、整数栈和结构体栈，并可用同一个宏分别对它们进行操作
 */

#include<stdio.h>
#include<stdlib.h>
//宏定义
#define stackinitsize 100
#define stackincrease 10

// 顺序整数栈的静态结构（常包括栈顶指针、固定容量、固定大小的数组）
unsigned int intsize;
int *inttop;
int *intbase;

// 字符栈
unsigned int charsize;
char *charbase;
char *chartop;

// 结构体栈 (每个格子都是一个完整的 tnode 结构体)
typedef struct node
{
    int data[10];
    float x,y;
}tnode;
unsigned int tnodesize;
tnode *tnodebase;
tnode *tnodetop;

//初始化 (stack*就相当于int*或char*这样的)
#define initstack(stack) stack##base=(stack*)malloc(stackinitsize*sizeof(stack));\
                         if(stack##base)\
                         {\
                            stack##top=stack##base;\
                            stack##size=stackinitsize;\
                         }\
                         else exit(0)
//判断是否为空
#define ifempty(stack) stack##base==stack##top?1:0
//获取栈顶元素
#define gettop(stack,e) stack##top==stack##base?0:(*e=*(stack##top-1),1)//整个逗号表达式的​​最终结果​​是最后一个表达式的值(返回1)
//清空栈
#define clearstack(stack) stack##top=stack##base//"逻辑上"清空，原有数据还在内存中，但无法通过正常栈操作访问
//销毁栈
#define destroystack(stack) free(stack##base)//stack##base的空间长度是stackinitsize哎
//进栈
#define pushstack(stack,e) \
    if(stack##top-stack##base==stack##size) { \
        stack##base=(stack*)realloc(stack##base,(stack##size+stackincrease)*sizeof(stack)); \
        if(!stack##base) exit(0); \
        stack##top=stack##base+stack##size; \
        stack##size=stack##size+stackincrease; \
    } \
    *(stack##top++)=e//拆分扩容和入栈操作(要不不扩容就进不了栈了。。。)
//出栈
#define popstack(stack,e) stack##top==stack##base?0:(*e=*(--stack##top),1)
//求栈长
#define stacklength(stack) stack##top-stack##base

//（番外）测试tnode
// 辅助函数：打印tnode内容
void print_tnode(const tnode *t, const char* name) {
    printf("%s: data=[", name);
    for(int i = 0; i < 10; i++) {
        printf("%d", t->data[i]);
        if(i < 9) printf(",");
    }
    printf("], x=%.1f, y=%.1f\n", t->x, t->y);
}

// tnode栈测试函数
void test_tnode_stack() {
    printf("=== 开始测试tnode栈 ===\n\n");
    
    // 1. 初始化栈
    printf("1. 初始化tnode栈...\n");
    initstack(tnode);
    printf("   栈长度: %d\n", stacklength(tnode));
    printf("   栈是否空: %s\n\n", ifempty(tnode) ? "是" : "否");
    
    // 2. 创建测试数据
    tnode node1, node2, node3;
    
    // 初始化node1
    for(int i = 0; i < 10; i++) node1.data[i] = i * 10;
    node1.x = 1.5f;
    node1.y = 2.5f;
    
    // 初始化node2
    for(int i = 0; i < 10; i++) node2.data[i] = i * 20;
    node2.x = 3.0f;
    node2.y = 4.0f;
    
    // 初始化node3
    for(int i = 0; i < 10; i++) node3.data[i] = i * 30;
    node3.x = 5.5f;
    node3.y = 6.5f;
    
    printf("2. 创建测试数据:\n");
    print_tnode(&node1, "node1");
    print_tnode(&node2, "node2");
    print_tnode(&node3, "node3");
    printf("\n");
    
    // 3. 入栈测试
    printf("3. 入栈操作测试:\n");
    pushstack(tnode, node1);
    printf("   入栈node1后栈长度: %d\n", stacklength(tnode));
    
    pushstack(tnode, node2);
    printf("   入栈node2后栈长度: %d\n", stacklength(tnode));
    
    pushstack(tnode, node3);
    printf("   入栈node3后栈长度: %d\n\n", stacklength(tnode));
    
    // 4. 获取栈顶元素测试
    printf("4. 栈顶元素测试:\n");
    tnode top_node;
    if(gettop(tnode, &top_node)) {
        printf("   当前栈顶元素:\n");
        print_tnode(&top_node, "   top_node");
    } else {
        printf("   栈为空，无法获取栈顶元素\n");
    }
    printf("\n");
    
    // 5. 出栈测试（LIFO特性验证）
    printf("5. 出栈测试（验证LIFO后进先出特性）:\n");
    tnode popped_node;
    
    if(popstack(tnode, &popped_node)) {
        printf("   第一次出栈元素:\n");
        print_tnode(&popped_node, "   popped");
    }
    
    if(popstack(tnode, &popped_node)) {
        printf("   第二次出栈元素:\n");
        print_tnode(&popped_node, "   popped");
    }
    
    printf("   出栈两次后栈长度: %d\n\n", stacklength(tnode));
    
    // 6. 清空栈测试
    printf("6. 清空栈测试:\n");
    clearstack(tnode);
    printf("   清空后栈长度: %d\n", stacklength(tnode));
    printf("   栈是否空: %s\n\n", ifempty(tnode) ? "是" : "否");
    
    // 7. 空栈操作测试
    printf("7. 空栈操作测试:\n");
    if(popstack(tnode, &popped_node)) {
        printf("   出栈成功\n");
    } else {
        printf("   栈空，出栈失败（这是正常行为）\n");
    }
    
    if(gettop(tnode, &top_node)) {
        printf("   获取栈顶元素成功\n");
    } else {
        printf("   栈空，无法获取栈顶元素（这是正常行为）\n");
    }
    printf("\n");
    
    // 8. 销毁栈
    printf("8. 销毁栈...\n");
    destroystack(tnode);
    printf("   tnode栈测试完成！\n");
}


//主函数
int main()
{
    initstack(int);
    initstack(char);
    initstack(tnode);

    int x;
    if(popstack(int,&x))
        printf("出栈成功 %d\n",x);
    else
        printf("栈空，不能出栈\n");//if和else写全的话代码看起来舒服哎
    printf("栈中有%d个元素\n",stacklength(int));

    if(ifempty(int))
        printf("栈空\n");
    else
        if(gettop(int,&x))
            printf("栈顶元素 %d\n", x);

    pushstack(int,3);
    printf("栈中有%d个元素\n",stacklength(int));

    pushstack(int,4);
    pushstack(int,5);
    printf("栈中有%d个元素\n",stacklength(int));

    if(ifempty(int))
        printf("栈空\n");
    else
        if(gettop(int,&x))
            printf("栈顶元素%d\n",x);
    
    clearstack(int);
    // 销毁并释放所有栈资源
    destroystack(int);
    destroystack(char);
    destroystack(tnode);

    return 0;
}

/*文件名可以有下划线不能有空格的样子。。。*/