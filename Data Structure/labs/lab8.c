/*广度优先搜索，依次输出搜索时经过的结点
深度优先搜索，依次输出搜索时经过的结点
Dijistra，依次输出遍历的结点及距离、最短路径
求联通片数量，输出结果*/
#ifndef _NETWORK_CPP_
#define _NETWORK_CPP_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAXVERTEX 100
#define MAXEDGE 20000

//顶点结构体
typedef struct Vertex {
	int id;//顶点编号（唯一）
	int w;//顶点权值
	int degree;//顶点的度
	int edges[MAXVERTEX];//边的编号
} Vertex;

//边结构体
typedef struct Edge {
	int id;//边的编号（唯一）
	int h;//边头顶点编号
	int t;//边尾顶点编号
	int w;//权值
} Edge;

//图结构体
typedef struct Graph {
	Vertex v[MAXVERTEX];//顶点数组
	Edge e[MAXEDGE];//边数组
	int nv;//顶点数
	int ne;//边数
	bool dirctional;//t:有向图，f:无向图
	bool weighted;//t:带权图，f:等权图
}Graph ;

//生成一个随机图，包括n个顶点，每个顶点和其它顶点连边的概率为p
int randgengraph(int n, float p, bool directional, bool weighted, const char* filename, int seed) {
	srand(seed);
	char* content = (char*)malloc(5000000);
	if (!content) {
		printf("分配存储空间失败！\n");
		exit(0);
	}
	FILE* fp = fopen(filename, "wb");
	if (!fp) {
		printf("生成图文件%s时失败！\n",filename);
		exit(0);
	}
	long offset = 0;//content当前的写入位置
	int ne = 0;//生成边数
	for (int i = 0; i < n; i++)
		offset += sprintf(content + offset, "%3d %5d\n", i, rand() % 100);//写入顶点的编号和顶点随机权值
	int pn = (int)(10000 * p);
	if (directional) {//有向图
		for(int i = 0; i < n; i++)
			for(int j = 0; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	}
	else {//无向图
		for(int i = 0; i < n; i++)
			for(int j = i; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	}
	char ch[40];
	int sz = sprintf(ch, "%d %7d %5s %5s\n", n, ne, weighted ? "true" : "flase", directional ? "true" : "flase");
	fwrite(&ch, sz, 1, fp);//写入图的顶点数和边数
	fwrite(content, offset, 1, fp);//写入图的顶点和边的信息
	fclose(fp);
	free(content);
}
//从文件中读入图，在内存中生成图的表示
void initGraphFromFile(Graph* g, const char* gfile) {
	FILE* fp = fopen(gfile, "r");
	if (!fp) {
		printf("读取图数据文件出错！\n");
		exit(0);
	}
	char bv[10];
	//读取图的基本信息
	fscanf(fp, "%u%u%s", &(g->nv), &(g->ne), bv);
	//printf("%d %d %s", g->ne, g->nv, bv);
	g->weighted = strcmp(bv, "true") == 0 ? true : false;
	fscanf(fp, "%s", bv);
	g->dirctional = strcmp(bv, "true") == 0 ? true : false;
	//读取顶点信息
	for (int i = 0; i < g->nv; i++) {
		fscanf(fp, "%d%d", &(g->v[i].id), &(g->v[i].w));
		g->v[i].degree = 0;
		//printf("顶点%d-权%d\n", g->v[i].id, g->v[i].w);
	}
	//读取边信息
	for (unsigned long i = 0; i < g->ne; i++) {
		fscanf(fp, "%u%u%d", &(g->e[i].h), &(g->e[i].t), &(g->e[i].w));
		//printf("边:%u-%u=>%d\n", g->e[i].h, g->e[i].t, g->e[i].w);
		g->e[i].id = i;
		unsigned h, t;
		h = g->e[i].h;
		t = g->e[i].t;
		g->v[h].edges[g->v[h].degree++] = i;
		if(g->dirctional==false)
			g->v[t].edges[g->v[t].degree++] = i;
	}
	fclose(fp);
}
// 生成一个空图
Graph* initGraph(bool directional, bool weighted){
	Graph * g = (Graph *)malloc(sizeof(Graph));
	g->nv = g->ne = 0;
	g->dirctional = directional;
	g->weighted = weighted;
	return g;
}
//打印邻接表的信息
void printgraph(Graph* g) {
	if (!g) return;
	printf("图的基本信息:顶点数(%d)-边数(%d)-%s-%s\n", g->nv, g->ne, g->weighted ? "加权图" : "等权图", g->dirctional ? "有向图" : "无向图");
	for (int i = 0; i < g->nv; i++) {
		printf("ID(%d)-度(%d)-权(%d)-边表 t(eid|w):%d", g->v[i].id, g->v[i].degree, g->v[i].w, g->v[i].id);
		for (int j = 0; j < g->v[i].degree; j++) {
			int e = g->v[i].edges[j];
			if (e < 0 || e >= g->ne) continue;
			if (g->v[i].id == g->e[e].h)
				printf("->%d(%d)", g->e[e].t, g->e[e].w);
			else 
				printf("->%d(%d)", g->e[e].h, g->e[e].w);
		}
		printf("\n");
	}
}

//查找给定编号的顶点，返回其顶点数组下标
int getVexIdx(Graph* g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return i;
	printf("input wrong vertex id in getVexIdx()!\n");
	return -1;
}

//广度优先搜索
//队列
typedef struct queue
{
	int data[MAXVERTEX];//放的都是顶点数组中的编号
	int front;
	int rear;
}Queue;
//初始化
void initQ(Queue *q){
	q->front=q->rear=0;//初始化为0
}
//判断是否为空
bool isEmptyQ(Queue *q){
	if(q->front==q->rear)
		return true;
	else
		return false;
}
//出队
int dequeue(Queue *q){
	if(!isEmptyQ(q)){
		int x=q->data[q->front];
		q->front++;
		return x;
	}
	return -1;
}
//入队
void enqueue(Queue *q,int x){
	if(q->rear<MAXVERTEX){
		q->data[q->rear]=x;
		q->rear++;
	}
}
//BFS算法
void BFS(Graph *g,int start_id){
	if(!g||start_id<0)
		return;

	int start = getVexIdx(g, start_id);
    if (start== -1)  // 顶点不存在
        return;
	
	Queue q;//*q的话需要手动分配内存哎，难绷，这数据结构学的
	initQ(&q);
	bool visited[MAXVERTEX]={false};
	enqueue(&q,start);
	visited[start] = true;

	printf("BFS from vertex %d: ", start_id);
	while(!isEmptyQ(&q)){
		int u=dequeue(&q);
		printf("%d ", g->v[u].id);

		for(int i=0;i<g->v[u].degree;i++){//degree是从0开始吧
			int eID=g->v[u].edges[i];//边的ID和数组中的编号相同，删除的时候也这么处理了
			if (eID < 0 || eID >= g->ne) 
				continue;
			int v_id;
			if(g->v[u].id==g->e[eID].h)
				v_id=g->e[eID].t;
			else
				v_id=g->e[eID].h;
            
			if(g->dirctional&&g->v[u].id!=g->e[eID].h)
				continue;
			int v=getVexIdx(g,v_id);
			if (v == -1) 
				continue;
			if (!visited[v]) {
				visited[v] = true;
				enqueue(&q, v);
			}
		}
	}

	/* 若图非连通，继续从未访问顶点开始遍历 */
	for (int vi = 0; vi < g->nv; vi++) {
		if (!visited[vi]) {
			enqueue(&q, vi);
			visited[vi] = true;
			while (!isEmptyQ(&q)) {
				int u = dequeue(&q);
				printf("%d ", g->v[u].id);
				for (int i = 0; i < g->v[u].degree; i++) {
					int eID = g->v[u].edges[i];
					if (eID < 0 || eID >= g->ne) 
						continue;
					int v_id = (g->v[u].id == g->e[eID].h) ? g->e[eID].t : g->e[eID].h;
					if (g->dirctional && g->e[eID].h != g->v[u].id) 
						continue;
					int v = getVexIdx(g, v_id);
					if (v == -1) 
						continue;
					if (!visited[v]) {
						visited[v] = true;
						enqueue(&q, v);
					}
				}
			}
		}
	}
	printf("\n");
}
//深度优先搜索
//递归
void DFS_helper(Graph *g,int u,bool *visited){
	visited[u]=true;
	printf("%2d ",g->v[u].id);
	for(int i=0;i<g->v[u].degree;i++){
		int eID=g->v[u].edges[i];
		int v_id;
		if(g->v[u].id==g->e[eID].h)
			v_id=g->e[eID].t;
		else
			v_id=g->e[eID].h;
		
		if(g->dirctional&&g->v[u].id!=g->e[eID].h)
			continue;
		int v=getVexIdx(g,v_id);
		if(!visited[v])
			DFS_helper(g,v,visited);
	}
}
//DFS算法
void DFS(Graph *g,int start_id){
	if(!g||start_id<0)
		return;
	int start=getVexIdx(g,start_id);
	if(start==-1)
		return;
	
	bool visited[MAXVERTEX]={false};
	printf("DFS from vertex %d: ", start_id);
	DFS_helper(g,start,visited);
	/* 若图非连通，从未访问顶点继续DFS */
	for (int i = 0; i < g->nv; i++) {
		if (!visited[i]) {
			DFS_helper(g, i, visited);
		}
	}
	printf("\n");
}
// Dijkstra最短路径算法
void dijkstra(Graph* g, int src_id){
    if(!g||src_id<0||src_id>=g->nv)
        return;

    const int INF=0x3f3f3f3f;
    int i,j;
    int dist[MAXVERTEX];
    bool used[MAXVERTEX];
    int prev[MAXVERTEX];
    for(i=0;i<g->nv;i++){
        dist[i]=INF;
        used[i]=false;
        prev[i]=-1;
    }
    int src=getVexIdx(g,src_id);
    dist[src]=0;
    
    printf("Dijkstra from vertex %d:\n", src_id);

    for(i=0;i<g->nv;i++){
        int u=-1;
        int best=INF;
        for(j=0;j<g->nv;j++){
            if(!used[j]&&dist[j]<best){
                best=dist[j];
                u=j;//u是在顶点数组的编号
            }
        }
        if(u==-1)
            break;
        used[u]=true;
        printf("vertex:%2d dist:%3d path:%2d",g->v[u].id,dist[u],src_id);

        int path[MAXVERTEX];
        int count=0;
        int cur=u;
        while(cur!=-1){
            path[count]=cur;
            cur=prev[cur];
            count++;
        }
		if(count>1)
        	for(j=count-2;j>=0;j--)
            	printf("->%d",g->v[path[j]].id);
        printf("\n");

        for(j=0;j<g->v[u].degree;j++){
            int eID=g->v[u].edges[j];
            int v_id;
            if(g->e[eID].h==g->v[u].id)
                v_id=g->e[eID].t;
            else
                v_id=g->e[eID].h;
            
            if(g->dirctional&&g->e[eID].h!=g->v[u].id)
                continue;
            
            int w=g->e[eID].w;
            int v=getVexIdx(g,v_id);
            if(dist[u]+w<dist[v]){
                dist[v]=dist[u]+w;
                prev[v]=u;
            }
        }
    }
}
//计算连通分量
int connectedComponents(Graph *g){
	if(!g)
		return 0;
	int component=0;
	bool visited[MAXVERTEX]={false};
	Queue q;

	for(int i=0;i<g->nv;i++){
		if (!visited[i]) {
			component++;
			initQ(&q);
			enqueue(&q, i);
			visited[i] = true;

			while(!isEmptyQ(&q)){
				int u=dequeue(&q);
				for(int j=0;j<g->v[u].degree;j++){
					int eID=g->v[u].edges[j];
					int v_id;
					if(g->v[u].id==g->e[eID].h)
						v_id=g->e[eID].t;
					else
						v_id=g->e[eID].h;

					if(g->dirctional&&g->v[u].id!=g->e[eID].h)
						continue;
					int v=getVexIdx(g,v_id);
					if (!visited[v]) {
						visited[v] = true;
						enqueue(&q, v);
					}
				}
			}
		}
	}
	return component;
}
//删除边
void deleteEdge(Graph* g, int id) {
	int d = -1;
	// 找到要删除的边在数组中的下标 d
	for (int i = 0; i < g->ne; i++) {
		if (g->e[i].id == id) {
			d = i;
			break;
		}
	}
	if (d == -1) {
		printf("边ID %d 不存在！\n", id);
		return;
	}

	int head = g->e[d].h;
	int tail = g->e[d].t;

	// 从 head 的边表中移除下标为 d 的条目（边表存储的是边数组下标）
	for (int i = 0; i < g->v[head].degree; i++) {
		if (g->v[head].edges[i] == d) {
			for (int j = i; j < g->v[head].degree - 1; j++)
				g->v[head].edges[j] = g->v[head].edges[j + 1];
			g->v[head].degree--;
			break;
		}
	}

	// 如果是无向图，也需要从 tail 的边表中移除
	if (!g->dirctional) {
		for (int i = 0; i < g->v[tail].degree; i++) {
			if (g->v[tail].edges[i] == d) {
				for (int j = i; j < g->v[tail].degree - 1; j++)
					g->v[tail].edges[j] = g->v[tail].edges[j + 1];
				g->v[tail].degree--;
				break;
			}
		}
	}

	// 从边数组中删除并左移后面的元素
	for (int i = d + 1; i < g->ne; i++) {
		g->e[i - 1] = g->e[i];
	}
	g->ne--;

	// 重新编号边的 id 字段（保持与数组下标一致）
	for (int i = 0; i < g->ne; i++) {
		g->e[i].id = i;
	}

	// 更新所有顶点的边表中受影响的下标：大于 d 的下标都应减一
	for (int vi = 0; vi < g->nv; vi++) {
		for (int j = 0; j < g->v[vi].degree; j++) {
			if (g->v[vi].edges[j] > d) {
				g->v[vi].edges[j]--;
			} else if (g->v[vi].edges[j] == d) {
				// 若仍存在等于 d 的残留项（保险起见），将其移除
				for (int k = j; k < g->v[vi].degree - 1; k++)
					g->v[vi].edges[k] = g->v[vi].edges[k + 1];
				g->v[vi].degree--;
				j--; // 重新检查当前位置
			}
		}
	}
}
//增加顶点
void addVex(Graph* g, int w) {
	if (g->nv >= MAXVERTEX) {
		printf("顶点数量已达上限 %d，无法添加新顶点！\n", MAXVERTEX);
		return;
	}
	g->v[g->nv].id = g->nv;
	g->v[g->nv].w = w;
	g->v[g->nv].degree = 0;
	for (int i = 0; i < MAXVERTEX; i++) g->v[g->nv].edges[i] = -1;
	g->nv++;
}
//增加边
void addEdge(Graph* g, int w, int h, int t) {
	if (g->ne >= MAXEDGE) {
		printf("边数量已达上限 %d，无法添加新边！\n", MAXEDGE);
		return;
	}
	g->e[g->ne].id = g->ne;
	g->e[g->ne].h = h;
	g->e[g->ne].t = t;
	g->e[g->ne].w = w;

	for (int i = 0; i < g->nv; i++) {
		if (g->v[i].id == h) {
			g->v[i].edges[g->v[i].degree++] = g->ne;
		}
		if (g->v[i].id == t && !g->dirctional) {
			g->v[i].edges[g->v[i].degree++] = g->ne;
		}
	}
	g->ne++;
}
//删除顶点，同时与其关联的边全部删除，记得调用 deleteEdge() 哟~
void deleteVex(Graph* g, int id) {
    int d = -1;
    for(int i = 0; i < g->nv; i++) {
        if(g->v[i].id == id) {
            d = i;
            break;
        }
    }
    if(d == -1) {
        printf("顶点ID %d 不存在！\n", id);
        return;
    }

    // 先删除与顶点相关的所有边
    for(int i = g->ne-1; i >= 0; i--) {  // 从后往前删除
        if(g->e[i].h == id || g->e[i].t == id) {
            deleteEdge(g, g->e[i].id);
        }
    }

    // 记录被删除顶点的ID
    int deleted_id = g->v[d].id;
    
    // 删除顶点，后面的顶点前移
    for(int i = d+1; i < g->nv; i++) {
        g->v[i-1] = g->v[i];
        g->v[i-1].id = i-1;  // 重新编号顶点的ID
    }
    g->nv--;

    // 更新所有边中顶点ID大于被删除顶点ID的边
    for(int i = 0; i < g->ne; i++) {
        if(g->e[i].h > deleted_id) g->e[i].h--;
        if(g->e[i].t > deleted_id) g->e[i].t--;
    }
    
    // 重新建立顶点的边表
    for(int i = 0; i < g->nv; i++) {
        g->v[i].degree = 0;
    }
    
    for(int i = 0; i < g->ne; i++) {
        int h = g->e[i].h;
        int t = g->e[i].t;
        
        // 找到顶点h在顶点数组中的位置
        int h_idx = -1;
        for(int j = 0; j < g->nv; j++) {
            if(g->v[j].id == h) {
                h_idx = j;
                break;
            }
        }
        if(h_idx != -1 && g->v[h_idx].degree < MAXVERTEX) {
            g->v[h_idx].edges[g->v[h_idx].degree++] = i;
        }
        
        // 如果是无向图，还要添加反向边
        if(!g->dirctional) {
            int t_idx = -1;
            for(int j = 0; j < g->nv; j++) {
                if(g->v[j].id == t) {
                    t_idx = j;
                    break;
                }
            }
            if(t_idx != -1 && t_idx != h_idx && g->v[t_idx].degree < MAXVERTEX) {
                g->v[t_idx].edges[g->v[t_idx].degree++] = i;
            }
        }
    }
}

//主函数
int main() {
	Graph* g = (Graph*)malloc(sizeof(Graph));
	if (!g) {
		printf("error\n");
		exit(0);
	}

	// 生成并读取图（无向带权示例）
	randgengraph(6, 0.5, 0, 1, "graph.txt", 9);
	initGraphFromFile(g, "graph.txt");
	printgraph(g);
	deleteVex(g,5);
	// BFS/DFS/Dijkstra/Connected components 测试
	printf("\n=== BFS Test ===\n");
	BFS(g, 0);

	printf("\n=== DFS Test ===\n");
	DFS(g, 0);

	printf("\n=== Dijkstra Test ===\n");
	dijkstra(g, 0);

	printf("\n=== Connected Components ===\n");
	int comps = connectedComponents(g);
	printf("Connected components: %d\n", comps);

	
	printgraph(g);
	return 0;
} 
#endif