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

//#include<bits/stdc++.h>万能头文件哎，长知识
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define Maxsize 200


typedef struct{//堆分配存储
    char *str; 
    int len;
}Stringtype;

//初始化：初始化串
Stringtype *fixed_initial(){
    Stringtype *str=(Stringtype*)malloc(sizeof(Stringtype));
    if(str!=NULL){
        str->len=0;
        
        str->str = (char*)malloc(1);//先初始化个1吧
        if (str->str == NULL) {
            free(str);
            printf("Error: Memory allocation failed\n");
            return NULL;
        }
        str->str[0]='\0';
        return str;
    }
    return NULL;
}
//销毁：销毁串，释放空间
void destroy(Stringtype *str){
    if(str!=NULL){
        free(str->str);//!!!
        free(str);
    }
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
void computeNext(Stringtype *str, int next[]){//还是用j和k吧，别混了
    if(str->len==0)
        return;

    int j=0,k=-1;
    next[0]=-1;
    
    while(j<str->len-1){
        if(k==-1||str->str[j]==str->str[k]){
            j++;
            k++;
            next[j]=k;
        }
        else
            k=next[k];
    }
}
// KMP 求子串第一次出现位置（若未找到返回 -1）
int location(Stringtype *s, Stringtype *t){
    if(t->len==0)
        return 0;

    // 为 next 数组按模式串长度动态分配，避免固定大小限制
    int *next = (int*)malloc(sizeof(int) * (t->len > 0 ? t->len : 1));
    if (next == NULL) {
        printf("Error: Memory allocation failed for KMP next array\n");
        return -1;
    }
    computeNext(t, next); // 计算模式串的 next 数组

    int j = 0, k = 0;
    // 标准 KMP 主循环：当 j==-1 或匹配时同时移动 i 和 j，否则回退 j
    while (j < s->len && k < t->len) {
        if (k == -1 || s->str[j] == t->str[k]) {
            j++;
            k++;
        } else {
            k = next[k];
        }
    }

    int result = -1;
    if (k >= t->len)
        result = j - t->len;

    free(next);
    return result;
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
    
    // allocate buffer for substring exactly len+1 bytes
    t->str = (char*)malloc((len + 1) * sizeof(char));//
    if (t->str == NULL) {
        free(t);
        printf("Error: Memory allocation failed\n");
        return NULL;
    }
    for (int j = 0; j < t->len; j++, i++)
        t->str[j] = s->str[i];

    t->str[len]='\0';

    return t;
}
//替换：Replace(S,T,V), S是主串，用V替换主串S中出现的所有与T相等的不重叠的子串
int StrReplace(Stringtype *S, Stringtype *T,Stringtype *V) {
    // 如果模式串为空，按测试预期返回 0（不做替换）
    if (T->len == 0)
        return 0;
    // 第一次扫描：统计 S 中非重叠出现 T 的次数
    int occ = 0;//（要重新分配内存，所以需要这一步）
    if (T->len <= S->len) {
        int idx = 0;//标记S中的遍历位置
        while (idx <= S->len - T->len) {
            if (strncmp(&S->str[idx], T->str, T->len) == 0) {
                occ++;
                idx += T->len;
            } else {
                idx++;
            }
        }
    }

    int finalLen = S->len + occ * (V->len - T->len);// 计算替换后最终长度

    // 分配临时缓冲区，大小为 finalLen+1 字节
    char *temp = (char*)malloc((finalLen + 1) * sizeof(char));
    if (temp == NULL) {
        printf("Error: Memory allocation failed\n");
        return 0;
    }

    int i = 0; // S 的读指针
    int j = 0; // temp 的写指针
    int Replacecount = 0;
    while (i < S->len) {
        if (i + T->len <= S->len && strncmp(&S->str[i], T->str, T->len) == 0) {
            // 复制 V 到临时缓冲
            for (int k = 0; k < V->len; k++)
                temp[j++] = V->str[k];
            i += T->len;
            Replacecount++;
        } else {
            temp[j++] = S->str[i++];
        }
    }
    temp[j] = '\0';

    // 重新调整 S->str 大小为 finalLen+1（使用 realloc）
    char *newbuf = (char*)realloc(S->str, (finalLen + 1) * sizeof(char));
    if (newbuf == NULL) {
        free(temp);
        printf("Error: Memory allocation failed during resize\n");
        return 0;
    }
    S->str = newbuf;
    memcpy(S->str, temp, finalLen + 1);//void *memcpy(void *dest, const void *src, size_t n);从源内存地址 src开始，拷贝 n个字节的数据到目标内存地址 dest(不限制数据类型，只是按字节复制)
    S->len = finalLen;
    free(temp);

    return Replacecount;
}
//拼接：拼接两个串
void strconcat(Stringtype *s,Stringtype *t){
    char *newbuf = (char*)realloc(s->str, (s->len + t->len + 1) * sizeof(char));//新建一个串来弄哎
    if (newbuf == NULL) {
        printf("Error: Memory allocation failed during concat\n");
        return;
    }
    s->str = newbuf;
    for (int i = 0; i < t->len; i++)
        s->str[s->len + i] = t->str[i];
    s->len += t->len;
    s->str[s->len] = '\0';
}
//遍历：依次输出串中所有字符
void traverse(Stringtype *str){
    for(int i=0;i<str->len;i++)
        printf("%c",str->str[i]);
}

// 辅助：安全地把 C 字符串赋给 Stringtype（按需 realloc）
void setString(Stringtype *s, const char *src) {
    if (s == NULL || src == NULL) return;
    size_t n = strlen(src);
    char *newbuf = (char*)realloc(s->str, n + 1);
    if (newbuf == NULL) {
        // 如果 realloc 失败，保持原状并打印错误
        printf("Error: Memory allocation failed in setString\n");
        return;
    }
    s->str = newbuf;
    memcpy(s->str, src, n + 1);
    s->len = (int)n;
}
int main() {
    printf("=== 串ADT测试程序（堆分配存储）===\n\n");
    
    // 测试1: 初始化操作
    printf("1. 测试初始化操作:\n");
    Stringtype *str1 = fixed_initial();
    Stringtype *str2 = fixed_initial();
    
    if (str1 == NULL || str2 == NULL) {
        printf("Error: 初始化失败\n");
        return -1;
    }
    
    // 使用setString赋值
    setString(str1, "ababa");
    setString(str2, "Data Structure");
    
    printf("str1: %s, 长度: %d\n", str1->str, length(str1));
    printf("str2: %s, 长度: %d\n\n", str2->str, length(str2));
    
    // 测试2: 求子串操作
    printf("2. 测试求子串操作:\n");
    Stringtype *sub1 = getsubstring(str1, 6, 5);
    if (sub1 != NULL) {
        printf("从str1位置6开始长度为5的子串: %s\n", sub1->str);
        destroy(sub1);
    }
    
    // 测试边界情况
    Stringtype *sub2 = getsubstring(str1, 8, 10);
    if (sub2 != NULL) {
        printf("从str1位置8开始长度为10的子串(自动调整): %s\n", sub2->str);
        destroy(sub2);
    }
    printf("\n");
    
    // 测试3: 模式匹配(KMP算法)
    printf("3. 测试模式匹配(KMP算法):\n");
    Stringtype *pattern = fixed_initial();
    setString(pattern, "aba");
    
    int pos = location(str1, pattern);
    printf("模式串'%s'在str1中的位置: %d\n", pattern->str, pos);
    
    // 测试不存在的模式
    setString(pattern, "ABC");
    pos = location(str1, pattern);
    printf("模式串'%s'在str1中的位置: %d\n\n", pattern->str, pos);
    destroy(pattern);
    
    // 测试4: 字符串拼接
    printf("4. 测试字符串拼接:\n");
    printf("拼接前 - str1: %s, str2: %s\n", str1->str, str2->str);
    strconcat(str1, str2);
    printf("拼接后 - str1: %s, 长度: %d\n\n", str1->str, length(str1));
    
    // 测试5: 替换操作 - 重点测试
    printf("5. 测试替换操作:\n");
    
    // 重新初始化测试数据
    destroy(str1);
    str1 = fixed_initial();
    setString(str1, "ababa");
    
    Stringtype *T = fixed_initial();
    Stringtype *V = fixed_initial();
    setString(T, "aba");
    setString(V, "ababa");
    
    printf("替换前: %s\n", str1->str);
    printf("被替换串 T: %s\n", T->str);
    printf("替换串 V: %s\n", V->str);
    
    int replaceCount = StrReplace(str1, T, V);
    printf("替换后: %s, 共替换了 %d 处\n", str1->str, replaceCount);
    printf("预期结果: abababa\n\n");
    
    // 测试6: 清空操作
    printf("6. 测试清空操作:\n");
    printf("清空前长度: %d\n", length(str1));
    clear(str1);
    printf("清空后长度: %d\n", length(str1));
    printf("清空后内容: '%s'\n\n", str1->str);
    
    // 测试7: 遍历操作
    printf("7. 测试遍历操作:\n");
    // 重新赋值
    setString(str1, "Hello Data Structure");
    printf("字符串遍历输出: ");
    traverse(str1);
    printf("\n\n");
    
    // 测试8: 复杂替换场景
    printf("8. 测试复杂替换场景:\n");
    Stringtype *S3 = fixed_initial();
    Stringtype *T3 = fixed_initial();
    Stringtype *V3 = fixed_initial();
    
    setString(S3, "Hello Hello Hello");
    setString(T3, "Hello");
    setString(V3, "Hi");
    
    printf("多次替换前: %s\n", S3->str);
    replaceCount = StrReplace(S3, T3, V3);
    printf("多次替换后: %s, 共替换了 %d 处\n\n", S3->str, replaceCount);
    
    // 测试9: 边界情况和错误处理
    printf("9. 测试边界情况和错误处理:\n");
    
    // 测试起始位置越界
    Stringtype *error_sub = getsubstring(str1, 100, 2);
    if (error_sub == NULL) {
        printf("✓ 正确处理了起始位置越界情况\n");
    }
    
    // 测试负长度
    error_sub = getsubstring(str1, 0, -1);
    if (error_sub == NULL) {
        printf("✓ 正确处理了负长度情况\n");
    }
    
    // 测试空模式替换
    Stringtype *empty = fixed_initial();
    empty->len = 0;
    empty->str[0] = '\0';
    
    replaceCount = StrReplace(str1, empty, V3);
    printf("空模式替换结果: %d (应返回0)\n", replaceCount);
    
    // 内存释放
    destroy(str1);
    destroy(str2);
    destroy(T);
    destroy(V);
    destroy(S3);
    destroy(T3);
    destroy(V3);
    destroy(empty);
    
    printf("\n=== 所有测试完成 ===\n");
    printf("✓ 内存已正确释放\n");
    
    return 0;
}
// 测试用主函数：覆盖替换/子串/KMP/拼接/遍历等功能
/*int main(void) {
    printf("=== lab4_2 测试开始 ===\n\n");

    // 测试：替换 (S = "ababa", T = "aba", V = "ababa")
    Stringtype *S = fixed_initial();
    Stringtype *T = fixed_initial();
    Stringtype *V = fixed_initial();
    setString(S, "ababa");
    setString(T, "aba");
    setString(V, "ababa");
    printf("替换前: %s\n", S->str);
    int rc = StrReplace(S, T, V);
    printf("替换后: %s, 共替换了 %d 处\n\n", S->str, rc);

    // 测试：getsubstring & KMP
    Stringtype *S2 = fixed_initial();
    setString(S2, "Hello World");
    Stringtype *sub = getsubstring(S2, 6, 5);
    if (sub) {
        printf("从 S2 位置6 长度5 的子串: %s\n", sub->str);
        destroy(sub);
    }
    setString(T, "World");
    int pos = location(S2, T);
    printf("模式 '%s' 在 S2 中的位置: %d\n\n", T->str, pos);

    // 测试：拼接
    Stringtype *A = fixed_initial();
    Stringtype *B = fixed_initial();
    setString(A, "Hello");
    setString(B, "_" );
    strconcat(A, B);
    strconcat(A, S2); // Hello_ + Hello World
    printf("拼接后 A: %s\n\n", A->str);

    // 遍历测试
    printf("遍历 A: "); traverse(A); printf("\n\n");

    // 清理
    destroy(S);
    destroy(T);
    destroy(V);
    destroy(S2);
    destroy(A);
    destroy(B);

    printf("=== 测试结束 ===\n");
    return 0;
}*/