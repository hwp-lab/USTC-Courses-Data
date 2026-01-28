#ifndef _LAB6_HUFFMANTREE_C_
#define _LAB6_HUFFMANTREE_C_

#include<stdio.h>
#include<stdlib.h>
//#include <direct.h>

#define MCL 30 //编码的最大长度 ------------存疑
#define n 256//定义叶节点数,8bit的unsigned char 
#define m 511 //定义结点总数，2n-1 
//定义结构体
typedef struct node
{
    unsigned char id;
    long w;
    struct node *parent,*lchild,*rchild;//typdef前，必须要加struct！
}HTNode;//一个树结点
typedef struct{
	unsigned code; //前面都是0，最后len位是编码 
	int len;   //编码长度 
} HCode;      //哈夫曼编码表

//包含头文件，用来输出html文件，查看图 
#define _LAB6_HUFFMANTREE_C_
#include "showGT.h"//不能在定义结构体前包含

//函数声明 
void printHtreeD(HTNode **node);
void printHTree(HTNode **tree, unsigned idr);
void genHCodes(HCode hc[], HTNode **HTree);//从哈夫曼树生成完整的编码表hc，输出hc为encode()的输入参数 
HTNode* creeteHTree(long *w, HTNode **node);//构造哈夫曼树
unsigned char *parseFile(const char filename[], long *w, long *fsize);//读取文件内容，得到w
void encode(unsigned char *orgi, long origin_len, unsigned char *new_code, long *new_len, HCode hc[]);//编码（原长olen的内容orgi，压缩编码后为长nlen的内容newc）
void zip(char fout[], HTNode **Htree, HTNode* root, HCode hc[], unsigned char *content, long fsize);
void unzip(char zfile[], char ofile[]);

//以下代码哈夫曼树生成 
//构造哈夫曼树/二叉树,输入长度为n=256的权向量w（每个字符出现的次数） ，返回树根
HTNode* creeteHTree(long *w,HTNode **node){
    //初始化所有节点
    int zeroNum=0;
    long allw=0;//所有权重和 long！
    for(int i=0;i<m;i++){
        node[i]=(HTNode*)malloc(sizeof(HTNode));
        if(node[i] == NULL) {
            printf("内存分配失败!\n");
            exit(0);
        }
        node[i]->id=i;
        node[i]->w=0;//先全置0，前面n个再分配！
        node[i]->lchild=node[i]->rchild=node[i]->parent=NULL;
        if(i<n){
            node[i]->w=w[i];
            if(node[i]->w==0)
                zeroNum++;
            allw+=node[i]->w;
        }
    }
    long min1,min2;
    int i=n,j;
    while(i<m-zeroNum){
        min1=min2=allw+10;//初始化！！！
        int pos1=-1,pos2=-1;
        for(j=0;j<i;j++){//j<i
            if(node[j]->parent != NULL ||node[j]->w==0)// 跳过已有父节点或权重为0的节点
                continue;
            if(node[j]->w<min1){
                min2=min1;
                pos2=pos1;
                min1=node[j]->w;//j!!!
                pos1=j;
            }
            else if(node[j]->w<min2){
                min2=node[j]->w;//j!!!
                pos2=j;
            }
        }
        if(pos1 == -1 || pos2 == -1) 
            break; // 没有足够的节点可以合并

        node[i]->lchild=node[pos1];
        node[i]->rchild=node[pos2];
        node[i]->w=node[pos1]->w+node[pos2]->w;
        node[pos1]->parent=node[pos2]->parent=node[i];
        i++;//while循环条件！
    }

    return node[m-zeroNum-1];//返回根节点
}
//遍历
void printHTree(HTNode **tree,unsigned idr){
    if(tree[idr]!=NULL){
        printf("id:%3d  w:%5ld\n",tree[idr]->id,tree[idr]->w);//为什么老师代码要加n？
        printHTree(tree,tree[idr]->lchild->id);
        printHTree(tree,tree[idr]->rchild->id);
    }
}
//直接数组打印出来，调试用 
void printHtreeD(HTNode **node){
	printf("\n哈夫曼树(数组次序)：\n"); 
	for(int i=0;i<m;++i)
		if (i<n)
			printf("%3d -> %5d => p:%3d lc:%3d rc:%3d\n",node[i]->id, node[i]->w, node[i]->parent,node[i]->lchild,node[i]->rchild); //%2x按十六进制打印，固定两个字节
		else
			printf("%3d -> %5d => p:%3d lc:%3d rc:%3d\n",node[i]->id+256, node[i]->w, node[i]->parent, node[i]->lchild,node[i]->rchild); //加256是为了和叶节点区分？不理解
}

//文件有关操作
//读取文件内容，得到w
unsigned char *parseFile(const char filename[], long *w,long *fsize){
    FILE*fp=fopen(filename,"rb");//"rb"二进制只读
    if(fp == NULL){
        printf("无法打开文件!\n");
        exit(0);
    }
    fseek(fp,0,SEEK_END);//​int fseek(FILE *stream, long offset, int origin)，将文件指针移动到由 origin参数指定的位置为基准，再偏移 offset个字节后所计算出的新位置 
    *fsize=ftell(fp);//获取文件指针的当前位置（在文件末尾时，即文件大小）
    rewind(fp);//返回开头

    unsigned char* dataArray;
    dataArray=(unsigned char*)malloc((*fsize)*sizeof(unsigned char));
    if (!dataArray) {
		printf("文件太大，内存不够，读入错误!\n");
        fclose(fp);
		exit(0);
	}
    fread(dataArray,sizeof(unsigned char),*fsize,fp);
    fclose(fp);

    int i;
    for(i=0;i<n;i++)
        w[i]=0;
    for(i=0;i<*fsize;i++){
        w[dataArray[i]]++;
    }

    return dataArray;
}
//输入任何字符c，得到它的哈夫曼编码
void getCode(HTNode **node, int c,unsigned *Code, int *code_len){
    unsigned code=0;//C语言默认unsigned指unsigned int ，一般编译器占4个字节，32位
    int codelen=0;
    int size = sizeof(unsigned) * 8; // 获取unsigned的位数
    unsigned mask=1<<(size-1);//先把mask赋值为1，可以想象成二进制的1，再左移（32-1）位

    HTNode *current = node[c];
    HTNode *parent = current->parent;
    while(parent != NULL){
        code = code >> 1; // 右移一位为新编码腾出空间
        
        if(parent->rchild == current){
            code = code | mask; // 设置最高位为1
        }// 否则最高位保持为0（左分支）
        
        codelen++;
        current = parent;
        parent = current->parent;
    }

    code=code>>(8*size-codelen);//把编码部分移到末位
    *Code=code;
    *code_len=codelen;
}
//从哈夫曼树生成完整的编码表hc，输出hc为encode()的输入参数 
void genHCodes(HCode hc[], HTNode **HTree){
	for(int i = 0; i < n; ++i){
        if(HTree[i]->w > 0) 
            getCode(HTree, i, &(hc[i].code), &(hc[i].len));
        else {
            hc[i].code = 0;
            hc[i].len = 0;
        }
    }
} 

//编码解码实现
//编码（原长olen的内容orgi，压缩编码后为长nlen的内容newc）
void encode(unsigned char *orgi, long origin_len, unsigned char *new_code, long *new_len, HCode hc[]){
    int i=0;//标记origin
    int j=0;//标记new_code
    new_code[j]=0;
    int usedbits=0; //newc[j]中被占用的bits数 
    int len;
    unsigned code;

    while(i<origin_len){
        int idx=orgi[i];//感觉就像取出来第i个字符，就是取出它的ASCII值
        len=hc[idx].len;
        code=hc[idx].code;
        while(usedbits+len>8){
            unsigned code_t=code>>(len+usedbits-8);//右移完后还剩8-usedbits位，不会超过8哎
            new_code[j]=new_code[j]|code_t;
            len=len+usedbits-8;
            usedbits=0;
            new_code[++j]=0;
        }
        code=code&((1<<len)-1);//(1<<len)-1使最后len位均是9，剩余为0；
        code=code<<8-len-usedbits;
        new_code[j]=new_code[j]|code;
        usedbits=len+usedbits;
        i++;
    }
    *new_len=j+1;
}
//生成和保存压缩文件,指定文件名fout，将所用的哈夫曼树存入文件 
void zip(char fout[],HTNode **Htree,HTNode* root,HCode hc[],unsigned char *content,long fsize){
    unsigned char* zipContent;
    long zipSize;
    zipContent=(unsigned char*)malloc(sizeof(unsigned char)*(fsize+1000));//可以因为要存入哈夫曼树有关内容，比原始长度还大。。。
    if (!zipContent) {
		printf("文件太大，内存不够，读入错误!\n");
		exit(0);
	}
    encode(content,fsize,zipContent,&zipSize,hc);

    FILE *fp = fopen(fout,"wb");
	if(fp == NULL){
        printf("无法打开写入文件!\n");
        exit(0);
    }

    //long ht_size=sizeof(HTNode)*m;//哈夫曼编码表的大小
    //fwrite(&ht_size,sizeof(long),1,fp);//
    fwrite(&fsize,sizeof(long),1,fp);//保存原始内容的大小 
    fwrite(&zipSize,sizeof(long),1,fp);//保存编码内容的大小 

    long weight[n];
    for(int i = 0; i < n; i++) //不能写指针进入文件，直接保存w，用来重构哈夫曼树吧
        weight[i] = Htree[i]->w; // 提取每个字符的频率
    fwrite(weight, sizeof(long), n, fp);
    fwrite(zipContent,sizeof(unsigned char),zipSize,fp);//保存编码后的内容
    fclose(fp);

    free(zipContent);	//释放文件内容
	printf("压缩文件 %s 已经生成！\n",fout);
}
//读取压缩文件，解压缩
void unzip(char zfile[],char ofile[]){
    FILE *fp = fopen(zfile,"rb");
	if(fp == NULL){
        printf("无法打开压缩文件进行读取!\n");
        exit(0);
    }

    long fsize,zipSize;
    long weight[n];

    fread(&fsize,sizeof(long),1,fp);
    fread(&zipSize,sizeof(long),1,fp);
    fread(weight, sizeof(long), n, fp);

    // 构建节点数组并重建哈夫曼树
    HTNode** nodeArray = (HTNode**)malloc(sizeof(HTNode*) * m);
    if(nodeArray == NULL) {
        printf("内存分配失败!\n");
        fclose(fp);
        exit(0);
    }
    HTNode *root = creeteHTree(weight, nodeArray);

    unsigned char* zipContent;
    unsigned char* oContent;
    zipContent=(unsigned char*)malloc(sizeof(unsigned char)*zipSize);
    oContent=(unsigned char*)malloc(sizeof(unsigned char)*fsize);
    fread(zipContent, sizeof(unsigned char), zipSize, fp);  
    fclose(fp);

    fp = fopen(ofile,"wb");
	if(fp == NULL){
        printf("无法打开解压后文件进行解压!\n");
        free(zipContent);
        free(oContent);
        free(nodeArray);
        exit(0);
    }

    int i=0;//标记源文件
    int j;//标记每个字节遍历到的位数
    int k=0;//zcontent的下标 
    HTNode *node=root;
    while (i<fsize)
    {
        j =128; //10000000，通过右移依次检查字节的每一位 
        while(j>0){//当前位为1，向右孩子移动
            if((zipContent[k]&j)>0){//按位与的优先级小于<
                if(node->lchild == NULL && node->rchild == NULL){//如果node是叶节点
                    oContent[i]=node->id;
                    i++;
                    fprintf(fp,"%c",oContent[i-1]);
					if (i>=fsize) //解码结束 
						break;//立即终止当前所在的循环或 switch 语句
                    node=root;
                    j=j<<1;//如果node的右孩子是空，那这一位就是下一个比特编码了，先左移一下一会再右移回来
                }
                else
                    node=node->rchild;
            }
            else{//当前位为0，向左孩子移动
                if(node->lchild == NULL && node->rchild == NULL){
                    oContent[i]=node->id;
                    i++;
                    fprintf(fp,"%c",oContent[i-1]);
                    if(i>=fsize)
                        break;
                    node=root;
                    j=j<<1;//j=!!
                }
                else
                    node=node->lchild;
            }
            j=j>>1;
        }
        if (i>=fsize) //解码结束 
			break; 
		k++;//准备读取下一个字符 
    }
    fclose(fp);
	free(oContent);
	free(zipContent);
	printf("文件 %s 已经成功解压为 %s ！\n",zfile,ofile);
    
}
//主函数
int main(){
    /*char current_dir[1024];
    
    // 获取当前工作目录
    if(_getcwd(current_dir, sizeof(current_dir)) != NULL) {
        printf("当前工作目录: %s\n", current_dir);
    }
    
    char infile[] = "KIY.txt";
    printf("正在查找文件: %s\n", infile);
    
    // 检查文件是否存在
    FILE* test_fp = fopen(infile, "rb");
    if(test_fp == NULL) {
        printf("❌ 文件不存在！请将 data.txt 放在以下目录:\n");
        printf("   %s\n", current_dir);
        
        // 尝试创建测试文件来确认权限
        FILE* test_write = fopen("test_write.txt", "w");
        if(test_write == NULL) {
            printf("⚠️  当前目录没有写入权限\n");
        } else {
            fprintf(test_write, "test");
            fclose(test_write);
            remove("test_write.txt");
            printf("✅ 当前目录有读写权限\n");
        }
    } else {
        printf("✅ 文件找到！\n");
        fclose(test_fp);
    }*/

    HCode hc[n];    //结构体数组，表示哈夫曼编码表 :code,len，用于编码文件
    HTNode *root;

    char infile[]="KIY.txt";
    long weight[n];
    long fsize;

    unsigned char* content;
    content=parseFile(infile,weight,&fsize);
    printf("文件分析完成，大小: %ld 字节\n", fsize);

    HTNode **nodeArray=(HTNode**)malloc(sizeof(HTNode*)*m);
    if(nodeArray == NULL) {
        printf("内存分配失败!\n");
        free(content);
        return -1;
    }
    root=creeteHTree(weight,nodeArray);
    genHCodes(hc,nodeArray);

    printHtreeD(nodeArray); //将哈夫曼树按数组打印出来 	
	//printHTree(nodeArray,root->id); //先序遍历哈夫曼树 
	saveTree(nodeArray, root->id, "huffman_tree.html"); //图形展示哈夫曼树 

    //编码，压缩文件,写入文件 fout
	char zfile[256]="my_KIY.lzip"; 
	zip(zfile,nodeArray,root,hc,content,fsize);

    //解码
    char ofile[256]="ndata_KIY.txt";
	unzip(zfile,ofile); 
	
	free(content);
    for(int i = 0; i < m; i++) {
        if(nodeArray[i] != NULL) {
            free(nodeArray[i]);
        }
    }
    free(nodeArray);
    
    printf("程序执行完成！\n");
	return 1; 
}
#endif