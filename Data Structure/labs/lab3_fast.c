#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>//meset

#define Maxsize 2000

int *solution;
int len;

//冲突统计数组
int row_conflict[Maxsize];//row是行，col是列
int diag1_conflict[Maxsize];//对角线x-y=k
int diag2_conflict[Maxsize];//对角线x+y=k

//函数声明
void initSolution();
void restart();
int traverseNeighbors();
int getQueenConflicts(int col);
void updateConflicts(int col, int old_row, int new_row);
void initializeConflicts();
int findMaxConflictQueen();
int evaluate();
void swap(int i,int j);
void swapColumns(int a, int b);
//主函数
int main(){
    // 使用当前时间作为随机数种子，使每次运行产生不同的随机扰动
    srand((unsigned)time(NULL));

    // printf("请输入皇后个数：");
    // scanf("%d",&len);     	// 键盘读入棋盘的大小
    len = 4;    // 棋盘大小
    solution = (int *)malloc(sizeof(int)*len);
    if (!solution)  return 0; //分配空间失败
    
    initSolution();            // 初始化一个棋盘布局
    
    int flag;
    while ((flag = traverseNeighbors())!=0)//遍历顺道就最大冲突算法了
        if (flag < 0)    restart();
    
    printf("冲突数为：%d\n得到的解为：",evaluate());
    for(int i=0;i<len;++i){
        if(i) printf(",");
        printf("%d",solution[i]);
    }
    printf("\n");
    free(solution);
    return 0;
}

// 初始化解：构造一个排列并做若干随机交换以扰动初始解
void initSolution(){
    for(int i = 0; i < len; ++i)
        solution[i] = i;
    for(int k = 0; k < 2*len; ++k)
        swap(rand()%len, rand()%len);
    initializeConflicts();
}

void initializeConflicts(){
    memset(row_conflict,0,sizeof(row_conflict));//void *memset(void *str, int c, size_t n),将指针 str所指向的内存块的前 n个字节，每个字节都设置为值 c
    memset(diag1_conflict,0,sizeof(diag1_conflict));
    memset(diag2_conflict,0,sizeof(diag2_conflict));

    for(int col=0;col<len;col++){
        int row=solution[col];
        row_conflict[row]++;
        diag1_conflict[row-col+len-1]++;
        diag2_conflict[row+col]++;
    }
}
//皇后移动后更新冲突统计数组
void updateConflicts(int col, int old_row, int new_row){
    row_conflict[old_row]--;
    diag1_conflict[old_row-col+len-1]--;
    diag2_conflict[old_row+col]--;

    row_conflict[new_row]++;
    diag1_conflict[new_row-col+len-1]++;
    diag2_conflict[new_row+col]++;

    solution[col]=new_row;
}
//每个皇后冲突数
int getQueenConflicts(int col){
    int conflict=0;
    int row=solution[col];
    conflict+=(row_conflict[row]-1);
    conflict+=(diag1_conflict[row-col+len-1]-1);
    conflict+=(diag2_conflict[row+col]-1);

    return conflict;
}
//寻找冲突数最大的皇后
int findMaxConflictQueen(){
    int max_conflicts=-1;
    int max_col=-1;
    
    for(int col=0;col<len;col++){
        int current_conflicts=getQueenConflicts(col);
        if(current_conflicts>max_conflicts){
            max_conflicts = current_conflicts;
            max_col=col;
        }
    }
    if(max_conflicts<=0) // 没有冲突或所有冲突为0
        return -1;
    
    return max_col;
}
//最大冲突法遍历
int traverseNeighbors(){
    int max_confilct_col = findMaxConflictQueen();
    if(max_confilct_col == -1) 
        return 0;

    // 使用 "交换列" 的操作来保持每一行只有一个皇后（即保持排列性质），避免出现重复行，比在一列里面换行好多了，还是copilot聪明
    int best_swap_col = -1;
    int min_total_conflicts = evaluate(); // 当前总体冲突

    for(int j = 0; j < len; ++j){
        if(j == max_confilct_col) continue;

        // 交换两列并测试总体冲突
        swapColumns(max_confilct_col, j);
        int total_conf = evaluate();
        // 立即交换回去
        swapColumns(max_confilct_col, j);

        if(total_conf < min_total_conflicts){
            min_total_conflicts = total_conf;
            best_swap_col = j;
        } else if(total_conf == min_total_conflicts && rand()%2==0){//增加随机性哎
            best_swap_col = j;
        }
    }

    if(best_swap_col != -1){
        // 执行最佳交换
        swapColumns(max_confilct_col, best_swap_col);
        return min_total_conflicts;
    }

    return -1; // 无改进，触发重启
}
//重新生成
void restart(){
	int times = 20;  // 随机交换解的次数，扰动大小,设置times>1即可
	for(int i = 0; i < times; ++i)
		swap(rand()%len,rand()%len);
    initializeConflicts();
	puts("restart()被调用！");
}
//交换两列 生成和重新生成时用，和swapColumns相比，时间复杂度小
void swap(int i, int j){
	int t=solution[i];
	solution[i]=solution[j];
	solution[j]=t;
}
// 交换两列（列 a 与列 b）并同时更新冲突统计（保持行排列唯一性）
void swapColumns(int a, int b){
    if(a==b) return;
    int ra = solution[a];
    int rb = solution[b];

    // 减去旧贡献
    row_conflict[ra]--;
    diag1_conflict[ra - a + len - 1]--;
    diag2_conflict[ra + a]--;

    row_conflict[rb]--;
    diag1_conflict[rb - b + len - 1]--;
    diag2_conflict[rb + b]--;

    // 交换位置
    solution[a] = rb;
    solution[b] = ra;

    // 加上新贡献
    row_conflict[rb]++;
    diag1_conflict[rb - a + len - 1]++;
    diag2_conflict[rb + a]++;

    row_conflict[ra]++;
    diag1_conflict[ra - b + len - 1]++;
    diag2_conflict[ra + b]++;
}
//计算总冲突数
int evaluate(){
    int conflicts=0;
    for(int col=0;col<len;col++)
        conflicts+=getQueenConflicts(col);
    
    return conflicts/2;
}