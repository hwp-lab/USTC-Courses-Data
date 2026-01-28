/*
---实验内容​​
串的ADT的实现
​​---实验说明​​
采用两种不同的存储方式（定长顺序结构存储、堆分配存储和块链存储三选二）实现串的ADT
实现串的基本操作，包括但不限于：
初始化：初始化串
销毁：销毁串，释放空间
清空：清为空串
求长度：返回串中的元素个数，称为串的长度
模式匹配：定位子串的位置，要求使用KMP算法实现
求子串：返回某个起始位置的某长度的子串
替换：Replace(S,T,V), S是主串，用V替换主串S中出现的所有与T相等的不重叠的子串
拼接：拼接两个串
遍历：依次输出串中所有字符
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define Maxsize 200

typedef struct{//定长顺序存储
    char str[Maxsize];
    int len;
}Stringtype;
//初始化：初始化串
Stringtype *fixed_initial(){
    Stringtype *str=(Stringtype*)malloc(sizeof(Stringtype));
    if(str!=NULL){
        str->len=0;
        str->str[0]='\0';
    }
    else
        printf("Error: Memory allocation failed\n");
        
    return str;
}
//销毁：销毁串，释放空间
void destroy(Stringtype *str){
    free(str);
}
//清空：清为空串
void clear(Stringtype *str){
    str->len=0;
    str->str[0]='\0';
}
//求长度：返回串中的元素个数，称为串的长度
int length(Stringtype *str){
    return str->len;
}
//模式匹配：定位子串的位置，要求使用KMP算法实现
//next[j]
void computeNext(Stringtype *str, int next[]) {//next[]不要用全局变量了，否则多个串的话就不好了
    int j = 0, k = -1;

    if (str->len== 0) return;
    next[0] = -1;

    while (j <str->len- 1) {
        if (k == -1 || str->str[j] == str->str[k]) {
            j++; 
            k++;
            next[j] = k;
        } else {
            k = next[k];
        }
    }
}
// KMP 求子串第一次出现位置（若未找到返回 -1）
int location(Stringtype *s, Stringtype *t) {
    int i = 0; // s index
    int j = 0; // t index

    if (t->len == 0) return 0; // 空模式视为匹配在位置 0

    int next[Maxsize];
    computeNext(t, next);

    while (i < s->len && j < t->len) {
        if (j == -1 || s->str[i]==t->str[j]) {
            i++; j++;
        } else {
            j = next[j];
        }
    }
    if (j >= t->len)
        return i - t->len;
    else
        return -1;
}
//求子串：返回某个起始位置的某长度的子串
Stringtype *getsubstring(Stringtype *s,int i,int len){//起始位置i
    if(i<0||i>=s->len){//初始检查哎
        printf("Error: Start position %d out of bounds (string length: %d)\n", i, s->len);
        return NULL;
    }
    if (len < 0) {
        printf("Error: Negative length %d not allowed\n", len);
        return NULL;
    }

    Stringtype *t;
    t=(Stringtype*)malloc(sizeof(Stringtype));
    if (t == NULL) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }
    
    if (i + len > s->len) {
        printf("Overflow\n");
        len = s->len - i;
    }
    t->len=len;

    for(int j=0;j<t->len;j++,i++)
        t->str[j]=s->str[i];

    t->str[len]='\0';

    return t;
}
//替换：Replace(S,T,V), S是主串，用V替换主串S中出现的所有与T相等的不重叠的子串
int StrReplace(Stringtype *S, Stringtype *T,Stringtype *V) {

    if (T->len == 0) return 0; // 不处理空模式

    int replaceCount = 0;

    // 临时缓冲区用于构建新字符串（左到右扫描，非重叠替换）
    char temp[Maxsize];
    int src_i = 0; // 源字符串读指针
    int dst_p = 0; // 标记temp

    while (src_i < S->len) {
        // 如果从 src_i 开始有足够长度并且匹配 T，则进行替换
        if (src_i + T->len <= S->len && strncmp(&S->str[src_i], T->str, T->len) == 0) {//没说不能用strncmp嘞
            // 检查替换后长度是否会超出缓冲区
            if (dst_p + V->len + (S->len - (src_i + T->len)) >= Maxsize) {
                printf("Error: Result string would exceed maximum size\n");
                // 将已构建部分复制回 S 并返回已替换计数
                temp[dst_p] = '\0';
                strncpy(S->str, temp, dst_p + 1);
                S->len = dst_p;
                return replaceCount;
            }

            // 复制 V 到目标
            for (int k = 0; k < V->len; k++) {
                temp[dst_p++] = V->str[k];
            }
            replaceCount++;
            // 跳过被替换的 T
            src_i += T->len;
        } else {
            // 直接复制当前字符
            temp[dst_p++] = S->str[src_i++];
        }
    }

    // 终止并写回 S
    temp[dst_p] = '\0';
    if (dst_p >= Maxsize) {
        // 防御性检查，理论上不会到这里
        printf("Error: Unexpected overflow during replace\n");
        return replaceCount;
    }
    strncpy(S->str, temp, dst_p + 1);
    S->len = dst_p;

    return replaceCount;
}
//拼接：拼接两个串
int strconcat(Stringtype *s,Stringtype *t){
    if(s->len+t->len>=Maxsize)//>=
        return -1;

    int i,j;
    for(i=0,j=s->len;i<t->len,j<s->len+t->len;i++,j++)
        s->str[j]=t->str[i];

    s->len=s->len+t->len;
    s->str[s->len] = '\0';//别忘了哎
    return 0;
}
//遍历：依次输出串中所有字符
void traverse(Stringtype *str){
    for(int i=0;i<str->len;i++)
        printf("%c",str->str[i]);
}
//主函数
int main() {
    printf("=== 串ADT测试程序 ===\n\n");
    
    // 测试1: 初始化操作
    printf("1. 测试初始化操作:\n");
    Stringtype *str1 = fixed_initial();
    Stringtype *str2 = fixed_initial();
    
    // 给字符串赋值
    char test1[] = "Hello World";
    char test2[] = "Data Structure";
    
    for(int i = 0; test1[i] != '\0'; i++) {
        str1->str[i] = test1[i];
    }
    str1->len = strlen(test1);
    str1->str[str1->len] = '\0';
    
    for(int i = 0; test2[i] != '\0'; i++) {
        str2->str[i] = test2[i];
    }
    str2->len = strlen(test2);
    str2->str[str2->len] = '\0';
    
    printf("str1: %s, 长度: %d\n", str1->str, length(str1));
    printf("str2: %s, 长度: %d\n\n", str2->str, length(str2));
    
    // 测试2: 求子串操作
    printf("2. 测试求子串操作:\n");
    Stringtype *sub1 = getsubstring(str1, 6, 5);
    if(sub1 != NULL) {
        printf("从str1位置6开始长度为5的子串: %s\n", sub1->str);
        free(sub1);
    }
    
    // 测试边界情况
    Stringtype *sub2 = getsubstring(str1, 8, 10); // 长度超出
    if(sub2 != NULL) {
        printf("从str1位置8开始长度为10的子串(自动调整): %s\n", sub2->str);
        free(sub2);
    }
    printf("\n");
    
    // 测试3: 模式匹配(KMP算法)
    printf("3. 测试模式匹配(KMP算法):\n");
    Stringtype *pattern = fixed_initial();
    char pat[] = "World";
    for(int i = 0; pat[i] != '\0'; i++) {
        pattern->str[i] = pat[i];
    }
    pattern->len = strlen(pat);
    pattern->str[pattern->len] = '\0';
    
    int pos = location(str1, pattern);
    printf("模式串'%s'在str1中的位置: %d\n", pattern->str, pos);
    
    // 测试不存在的模式
    char pat2[] = "ABC";
    for(int i = 0; pat2[i] != '\0'; i++) {
        pattern->str[i] = pat2[i];
    }
    pattern->len = strlen(pat2);
    pattern->str[pattern->len] = '\0';
    
    pos = location(str1, pattern);
    printf("模式串'%s'在str1中的位置: %d\n\n", pattern->str, pos);
    destroy(pattern);
    
    // 测试4: 字符串拼接
    printf("4. 测试字符串拼接:\n");
    printf("拼接前 - str1: %s, str2: %s\n", str1->str, str2->str);
    int concatResult = strconcat(str1, str2);
    if(concatResult == 0) {
        printf("拼接后 - str1: %s, 长度: %d\n\n", str1->str, length(str1));
    } else {
        printf("拼接失败: 字符串过长\n\n");
    }
    
    // 测试5: 替换操作
    printf("5. 测试替换操作:\n");
    // 重新初始化测试数据
    destroy(str1);
    str1 = fixed_initial();
    char test3[] = "ababa";
    for(int i = 0; test3[i] != '\0'; i++) {
        str1->str[i] = test3[i];
    }
    str1->len = strlen(test3);
    str1->str[str1->len] = '\0';
    
    Stringtype *T = fixed_initial(); // 要被替换的模式
    Stringtype *V = fixed_initial(); // 替换内容
    
    char t_val[] = "aba";
    char v_val[] = "ababa";
    
    for(int i = 0; t_val[i] != '\0'; i++) {
        T->str[i] = t_val[i];
    }
    T->len = strlen(t_val);
    T->str[T->len] = '\0';
    
    for(int i = 0; v_val[i] != '\0'; i++) {
        V->str[i] = v_val[i];
    }
    V->len = strlen(v_val);
    V->str[V->len] = '\0';
    
    printf("替换前: %s\n", str1->str);
    int replaceCount = StrReplace(str1, T, V);
    printf("替换后: %s, 共替换了 %d 处\n\n", str1->str, replaceCount);
    
    // 测试6: 清空操作
    printf("6. 测试清空操作:\n");
    printf("清空前长度: %d\n",str1->str,length(str1));
    clear(str1);
    printf("清空后长度: %d\n", length(str1));
    printf("清空后内容: '%s'\n\n", str1->str);
    
    // 测试7: 遍历操作
    printf("7. 测试遍历操作:\n");
    // 重新赋值
    char test4[] = "ABC";
    for(int i = 0; test4[i] != '\0'; i++) {
        str1->str[i] = test4[i];
    }
    str1->len = strlen(test4);
    str1->str[str1->len] = '\0';
    
    printf("字符串遍历输出(ASCII码): ");
    traverse(str1);
    printf("\n");
    
    // 测试8: 边界情况和错误处理
    printf("\n8. 测试边界情况和错误处理:\n");
    
    // 测试起始位置越界
    Stringtype *error_sub = getsubstring(str1, 10, 2);
    if(error_sub == NULL) {
        printf("✓ 正确处理了起始位置越界情况\n");
    }
    
    // 测试负长度
    error_sub = getsubstring(str1, 0, -1);
    if(error_sub == NULL) {
        printf("✓ 正确处理了负长度情况\n");
    }
    
    // 测试空模式替换
    Stringtype *empty = fixed_initial();
    empty->len = 0;
    empty->str[0] = '\0';
    
    replaceCount = StrReplace(str1, empty, V);
    printf("空模式替换结果: %d (应返回0)\n", replaceCount);
    
    // 释放内存
    destroy(str1);
    destroy(str2);
    destroy(T);
    destroy(V);
    destroy(empty);
    
    printf("\n=== 所有测试完成 ===\n");
    printf("✓ 内存已正确释放\n");
    
    return 0;
}
