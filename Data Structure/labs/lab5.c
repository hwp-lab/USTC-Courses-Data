/*
1.实验内容​
二叉树的实现及应用
2.实验说明​
读懂老师给出的代码框架中的代码（二叉链表实现的二叉树），几种构造二叉树的方法的代码细节都要读懂。
代码下载：https://rec.ustc.edu.cn/share/dd8337e0-9241-11ed-b64a-eb675d79527e
(1)实现操作：删除节点 value=x 的节点及其子树。
(2)实现操作：给定 id1 和 id2，输出其最近共同祖先 id，节点的 id 具有唯一性。
(3)实现操作：给定 id 值，输出从根节点到 id 值节点的路径，用“左右右...”的左右孩子指针标记从根到节点的路径。
(4)编写递归算法，输出二叉树节点中最大的 value 和最小的 value 之差。
3.立志多学几个树有关的代码，那几节课呃在神游在赶ddl的样子
*/
#ifndef LAB5_C_//查了查源文件用处不大的样子，不过反正也不费事
#define LAB5_C_

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
//定义存放数据的类型,假设是可以存放多个数据的结构体类型 
typedef struct{
    int id;
    int value;
}Elemtype;
//定义二叉树的结点
typedef struct tnode{
    Elemtype data;
    struct tnode *lchild;//typedef前，数据类型是struct tnode！！！
    struct tnode *rchild;
}bNode;

//二叉树构造函数
//二叉树1：二叉搜索/排序树
bNode *buildBTreeByValue(int *value,int len){
    bNode *root=(bNode*)malloc(sizeof(bNode));
    if(!root){
        printf("Error:Memory allocation failed\n");
        exit(1);//非0值表示异常退出
    }
    root->data.id=0;
    root->data.value=value[0];//value数组第0个元素为根的value
    root->lchild=root->rchild=NULL;

    int idx;
    for(idx=1;idx<len;idx++){
        bNode *t=(bNode*)malloc(sizeof(bNode));
        if(!t){
        printf("Error:Memory allocation failed\n");
        exit(0);
        }
        t->data.id=idx;
        t->data.value=value[idx];
        t->lchild=t->rchild=NULL;//置空哎

        bNode *p=root;//标记父节点
        bNode *q=root;
        while(q){
            p=q;
            if(value[idx]<q->data.value)//value依次和根比较，若比根小，就和根的左子树比较；否则和右子树比较
                q=q->lchild;
            else
                q=q->rchild;
        }
        
        if(value[idx]<p->data.value)
            p->lchild=t;
        else
            p->rchild=t;
    }
    return root;
}
//二叉树2：基于双亲
bNode *buildBTreeByParents(int *value,int *edge,int len){
    bNode **nodes=(bNode**)malloc(sizeof(bNode*)*len);//指向结构体数组*nodes的指针的指针，里面每个元素都是bNode*
    if(!nodes){
        printf("Error:Memory allocation failed\n");
        exit(1);//非0值表示异常退出
    }

    int idx;
    for(idx=0;idx<len;idx++){
        nodes[idx]=(bNode*)malloc(sizeof(bNode));//再内存分配！！指针都要内存分配吧
        if(!nodes[idx]){
        printf("Error:Memory allocation failed\n");
        exit(1);//非0值表示异常退出
        }
        nodes[idx]->data.id=idx;
        nodes[idx]->data.value=value[idx];
        nodes[idx]->lchild=nodes[idx]->rchild=NULL;
    }

    bNode *root;
    for(idx=0;idx<len;idx++){//二叉树不唯一，左右子树可以互换，优先构造左子树 
        if(edge[idx]!=-1){//edge[id]表示编号为id的结点的双亲结点的编号;根结点的双亲编号为-1 
            if(!nodes[edge[idx]]->lchild)
                nodes[edge[idx]]->lchild=nodes[idx];
            else if(!nodes[edge[idx]]->rchild)
                nodes[edge[idx]]->rchild=nodes[idx];
            else{
                printf("Error: edge\n");
                exit(1);
            }
        }
        else
            root=nodes[idx];
    }
    return root;
}
//二叉树3：先序序列prelist[i]
bNode *buildBTreeByPrelist_helper(int *prelist, int *idx) {//二叉树的节点数量为n,数组prelist长度为len=2n+1
    #define NULLNODE -9999 //先序序列中空节点的值
    if(prelist[*idx] == NULLNODE) {
        (*idx)++;
        return NULL;
    }
    
    bNode *t = (bNode*)malloc(sizeof(bNode));
    if(!t) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }
    t->data.id = *idx;
    t->data.value = prelist[*idx];
    (*idx)++;
    
    t->lchild = buildBTreeByPrelist_helper(prelist, idx);
    t->rchild = buildBTreeByPrelist_helper(prelist, idx);
    
    return t;
}
bNode *buildBTreeByPrelist(int *prelist) {
    int idx = 0;  // 每次调用都重新初始化，不要想着用静态变量了还是
    return buildBTreeByPrelist_helper(prelist, &idx);
}
//二叉树4：先序序列(pre)+中序序列(in)（无空节点）
bNode *buildBTreeByPI(int *pre, int pl, int pu, int *in, int il, int iu){
    if(pu-pl!=iu-il){//u-upper bound上界  l-lower bound下界
        printf("Error: \n");
        exit(1);
    }
    if(pu-pl<0)//如果左/右孩子是空的话，就会小于0
        return NULL;//空树

    bNode *t=(bNode*)malloc(sizeof(bNode));
    if(!t){
        printf("Error:Memory allocation failed\n");
        exit(1);//非0值表示异常退出
    }
    t->data.id=pl;//用先序序列表示的编号
    t->data.value=pre[pl];

    int pl_1,pl_2,pu_1,pu_2;
    int il_1,il_2,iu_1,iu_2;
    for(int i=il;i<=iu;i++){
        if(pre[pl]==in[i]){
            il_1=il;
            iu_1=i-1;
            il_2=i+1;
            iu_2=iu;
            pl_1=pl+1;
            pu_1=pl_1+(iu_1-il_1);
            pl_2=pu_1+1;
            pu_2=pl_2+(iu_2-il_2);
        }
    }
    t->lchild=buildBTreeByPI(pre,pl_1,pu_1,in,il_1,iu_1);
    t->rchild=buildBTreeByPI(pre,pl_2,pu_2,in,il_2,iu_2);

    return t;
}

//三种遍历，神奇！
//先序DLR
void PreTraverse(bNode *root,void (*ptrf)(bNode *a)){//第二个形参--有一个bNode*类型形参的，void类型的函数
    if(root){
        ptrf(root);//ptrf是一个函数指针，它指向一个函数;(*ptrf)表示通过指针来调用函数
        PreTraverse(root->lchild,ptrf);
        PreTraverse(root->rchild,ptrf);
    }
}
//中序LDR
void InTraverse(bNode *root,void (*ptrf)(bNode *a)){
    if(root){//递归出口哎！
        InTraverse(root->lchild,ptrf);
        ptrf(root);
        InTraverse(root->rchild,ptrf);
    }
}
//后续LRD
void PostTraverse(bNode *root,void (*ptrf)(bNode *a)){
    if(root){
        PostTraverse(root->lchild,ptrf);
        PostTraverse(root->rchild,ptrf);//编译器自动将ptrf其视为 (*ptrf)
        ptrf(root);
    }
}

//查找父节点
bNode *getParent(bNode *root,bNode *p){
    if(!root||root==p)
        return NULL;//根节点无双亲，空树也无
    
    if(p&&(root->lchild==p||root->rchild==p))
        return root;
    bNode *left = getParent(root->lchild, p);
    if(left) 
        return left;
    
    return getParent(root->rchild, p);  
}
//由id查找节点
bNode *locateByID(bNode *root,int id){
    if(!root)
        return NULL;//递归结束
    
    if(root->data.id==id)
        return root;
    bNode *t=locateByID(root->lchild,id);
    if(t)
        return t;
    return locateByID(root->rchild,id);
}
//由value查找节点
bNode *locateByValue(bNode *root,int x){
    if(!root)
        return NULL;//递归结束
    
    if(root->data.value==x)
        return root;
    bNode *t=locateByValue(root->lchild,x);
    if(t)
        return t;
    return locateByValue(root->rchild,x);
}
//求树的深度
int getDepth(bNode *root){
    if(!root)
        return 0;
    
    int left=getDepth(root->lchild);
    int right=getDepth(root->rchild);
    if(left>right)
        return left+1;
    else
        return right+1;
}
//求特定节点距树根的深度
void getDepthX(bNode *root,bNode *x,int *d){
    bNode *p=getParent(root,x);
    if(x==root){
        *d=0;
        return;
    }
    *d=1;
    while(p!=root){
        (*d)++;
        p=getParent(root,p);
    }
}
//销毁树
void DestroyBTree(bNode *root){
    if(root==NULL)
        return;//递归结束条件！！！ root为空的话，root->lchild可能出问题
        
    DestroyBTree(root->lchild);
    DestroyBTree(root->rchild);
    free(root);
}

//(1)删除节点 value=x 的节点及其子树
void DestroyValue(bNode *root,int x){
    bNode *nodex=locateByValue(root,x);

    if(!nodex){
        printf("Error: failed to find\n");
        return;
    }

    if(nodex!=root){//要置空来自双亲的节点哎，否则多了个悬空指针（指针曾经指向一个有效的内存地址，但该内存被释放或失效后，指针未被置空，仍然指向原地址）
        bNode *nodex_p=getParent(root,nodex);
        if(nodex_p->lchild==nodex)
            nodex_p->lchild=NULL;
        else
            nodex_p->rchild=NULL;
    }
    DestroyBTree(nodex);
}
//(2)寻找最近共同祖先
bNode *getCoParent(bNode *root,bNode *p,bNode *q){
    if(root==NULL||root==p||root==q)//不等价于root == (p || q)!!!
        return root;

    bNode *left=getCoParent(root->lchild,p,q);
    bNode *right=getCoParent(root->rchild,p,q);

    if(left&&right)
        return root;//p,q一个在左孩子一个在右孩子，就是共同祖先，找到第一个以后，再往上就。呃怪怪的，明白又不明白
    else 
        return left?left:right;//若左右孩子都是空，就返回空；若左右孩子都没找到，也是返回空
}
bNode *getCoParentID(bNode *root,int id1,int id2){
    bNode *node1=locateByID(root,id1);
    bNode *node2=locateByID(root,id2);

    if(!node1||!node2)
        return NULL;
    return getCoParent(root,node1,node2);
}
//(3)输出从根节点到 id 值节点的路径
void getPath(bNode *root,bNode *node,int *path){
    //已排除根节点情况
    bNode *parent=getParent(root,node);
    bNode *current=node;
    int i=0;
    while(current!=root){
        parent=getParent(root,current);
        if (parent->lchild == current) {
            path[i] = 0; // 0 表示"左"
        } else if (parent->rchild == current) {
            path[i] = 1; // 1 表示"右"
        } 
        i++;
        current=parent;
    }
}
void printPath(bNode *root,int id){
    bNode *node=locateByID(root,id);

    if(!node){
        printf("Error: failed to find\n");
        return;
    }
    if(root==node){
        printf("The current node is the root node\n");
        return;
    }

    int depth;
    getDepthX(root,node,&depth);
    int path[depth];
    getPath(root,node,path);
    int i;
    for(i=depth-1;i>=0;i--){
        if(path[i]==0)
            printf("左->");
        else if(path[i]==1)
            printf("右->");
        else
            printf("Error");
    }
    printf("（目标节点）\n");
}
//(4)得到最大值和最小值的差值
void getMaxMin(bNode *root,int *max,int *min){//还是不要尝试用static了，只能用一次，太难受了
    if(!root)
    return;//空节点的话也返回个值

    if(root->data.value>*max)
        *max=root->data.value;
    if(root->data.value<*min)
        *min=root->data.value;

    getMaxMin(root->lchild,max,min);
    getMaxMin(root->rchild,max,min);
}
int getMaxDiff(bNode *root){
    int max=INT_MIN;//int *max = INT_MIN不行
    int min=INT_MAX;
    getMaxMin(root,&max,&min);

    return max-min;
}

//结点访问函数 visit() 的不同实现方式
void bPrintNode(bNode *a){ 
	printf("\n(id = %2d) = %5d",a->data.id,a->data.value); //%2d--若实际整数的位数小于2，则在左边用空格填充，使其总宽度为2；如果实际整数的位数等于或大于2，则按实际位数完整输出
}
void cPrintNode(bNode *a){ 
	printf("\n*(addr = %lx):(id=%2d)= %5d",a,a->data.id,a->data.value); //%lx--将unsigned long类型的整数以十六进制格式输出，并且字母使用小写（a-f）
}
void add1(bNode *a){
	a->data.value += 1;
} 
// 测试主函数
int main() {
    printf("=== 二叉树操作测试 ===\n\n");
    
    // 构建示例中的二叉树 [3,5,1,6,2,0,8,null,null,7,4]
    int prelist[] = {3,5,6,-9999,-9999,2,7,-9999,-9999,4,-9999,-9999,1,0,-9999,-9999,8,-9999,-9999};
    bNode *root = buildBTreeByPrelist(prelist);
    
    printf("1. 先序遍历结果：");
    PreTraverse(root, bPrintNode);
    printf("\n");
    
    printf("2. 中序遍历结果：");
    InTraverse(root, bPrintNode);
    printf("\n");
    
    printf("3. 后序遍历结果：");
    PostTraverse(root, bPrintNode);
    printf("\n\n");
    
    // 测试(1)：删除节点 value=5 的节点及其子树
    printf("4. 删除value=5的节点及其子树前：\n");
    PreTraverse(root, bPrintNode);
    printf("\n");
    
    DestroyValue(root, 3);
    printf("删除value=5的节点及其子树后：\n");
    //PreTraverse(root, bPrintNode);
    printf("\n\n");
    
    // 重新构建二叉树进行后续测试
    //DestroyBTree(root);
    int prelist2[] = {3,5,6,-9999,-9999,2,7,-9999,-9999,4,-9999,-9999,1,0,-9999,-9999,8,-9999,-9999};
    root = buildBTreeByPrelist(prelist2);
    
    // 测试(2)：寻找最近共同祖先（对应图片中的示例）
    printf("5. 最近共同祖先测试：\n");
    
    // 示例1：节点5(id=1)和节点1(id=5)的最近公共祖先
    bNode *lca1 = getCoParentID(root, 1, 12);
    if(lca1) {
        printf("   节点5(id=1)和节点1(id=5)的最近公共祖先是：节点%d\n", lca1->data.value);
    }
    
    // 示例2：节点5(id=1)和节点4(id=9)的最近公共祖先  
    bNode *lca2 = getCoParentID(root, 1, 9);
    if(lca2) {
        printf("   节点5(id=1)和节点4(id=9)的最近公共祖先是：节点%d\n", lca2->data.value);
    }
    printf("\n");
    
    // 测试(3)：输出从根节点到指定id节点的路径
    printf("6. 路径测试：\n");
    printf("   到节点5(id=1)的路径：");
    printPath(root, 1);
    
    printf("   到节点4(id=9)的路径：");
    printPath(root, 9);
    
    printf("   到节点0(id=13)的路径：");
    printPath(root, 13);
    printf("\n");
    
    // 测试(4)：输出二叉树节点中最大的value和最小的value之差
    printf("7. 最大值和最小值之差：\n");
    int diff = getMaxDiff(root);
    printf("   最大value和最小value之差为：%d\n", diff);
    
    // 清理资源
    DestroyBTree(root);
    
    return 0;
}
#endif