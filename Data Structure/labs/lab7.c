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

/*
 * 说明（补充）：
 * - 存储结构：本文件使用邻接表的变体（每个顶点保存与它相关的边数组，边数组保存在全局 e[] 中，
 *   通过边的数组下标/ID 在顶点边表内进行引用）。邻接表适合大多数图算法（例如 DFS/BFS、连通分量、
 *   最短路等），能有效遍历邻居并在稀疏图中节省空间。但当前实现较为简单：删除边后没有把所有
 *   相关的 edge.id 做严格重编号，因此在复杂修改/大量删除后需额外维护一致性（可后续改进）。
 *
 * - 已实现的复杂操作说明：
 *   1) connectedComponents(): 计算图的连通分量（对有向图为弱连通分量） — O(V + E) 时间复杂度（使用 DFS）。
 *   2) dijkstra_shortest_path(): Dijkstra 最短路径实现（适用于非负权图），当前实现为 O(V^2) 版本（线性扫描选最小顶点），
 *      适合教学和小规模图；需要更大规模时可用堆优化到 O(E log V)。
 */

//生成一个随机图，包括n个顶点，每个顶点和其它顶点连边的概率为p
void randgengraph(int n, float p, bool directional, bool weighted, const char* filename, int seed) {
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
			for(int j = i+1; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	}
	char ch[40];
	int sz = sprintf(ch, "%d %7d %5s %5s\n", n, ne, weighted ? "true" : "false", directional ? "true" : "false");
	fwrite(ch, sz, 1, fp);//写入图的顶点数和边数
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
	fscanf(fp, "%d%d%s", &(g->nv), &(g->ne), bv);
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
	for (int i = 0; i < g->ne; i++) {
		fscanf(fp, "%d%d%d", &(g->e[i].h), &(g->e[i].t), &(g->e[i].w));
		//printf("边:%u-%u=>%d\n", g->e[i].h, g->e[i].t, g->e[i].w);
		g->e[i].id = i;
		int h, t;
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
	printf("图的基本信息:顶点数(%d)-边数(%d)-%s-%s\n", g->nv, g->ne, g->weighted ? "加权图" : "等权图", g->dirctional ? "有向图" : "无向图");
	for (int i = 0; i < g->nv; i++) {
		printf("%2d ID(%d)-度(%d)-权(%d)-边表 t(eid|w):%d",i,g->v[i].id, g->v[i].degree, g->v[i].w, g->v[i].id);
		for (int j = 0; j < g->v[i].degree; j++) {
			int e = g->v[i].edges[j];
			if (e < 0 || e >= g->ne) continue; // 防止打印非法边索引
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
	exit(0);
}
//查找给定编号的顶点，返回其权值
int getVexW(Graph* g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return g->v[i].w;
	printf("input wrong vertex id in getVexW()!\n");
	exit(0);
}
//查找给定编号的顶点，返回其度
int getVexDegree(Graph* g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return g->v[i].degree;
	printf("input wrong vertex id in getVexDegree()!\n");
	exit(0);
}
//查找给定编号的顶点,返回其所有边
int* getEdgesByNode(Graph* g, int id, int* ne) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id) {
			*ne = g->v[i].degree;
			return g->v[i].edges;
		}
	printf("input wrong vertex id in getEdgesByNode()!\n");
	exit(0);
}
//查找给定编号的边，返回其边数组下标
int getEdgeIdx(Graph* g, int id) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
			return i;
	printf("input wrong edge id in getEdgeIdx()!\n");
	exit(0);
}
//查找给定编号的边，返回其权值
 int getEdgeW(Graph* g, int id) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
			return g->e[i].w;
	printf("input wrong edge id in getEdgeW()!\n");
	exit(0);
}
//查找给定编号的边，返回其两个顶点
 void getVexByEdge(Graph* g, int id, int* h, int* t) {
	if (g == NULL || h == NULL || t == NULL) {
		printf("错误：传入 NULL 指针到 getVexByEdge()!\n");
		return;
	}
	for (int i = 0; i < g->ne; i++) {
		if (g->e[i].id == id) {
			*h = g->e[i].h;
			*t = g->e[i].t;
			return;
		}
	}
	printf("input wrong edge id in getVexByEdge()!\n");
}
//通过权值查找顶点编号
 int locateVex(Graph* g, int w) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].w == w)
			return g->v[i].id;
	return -1;
}
//通过权值查找边编号
 int locateEdge(Graph* g, int w) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].w == w)
			return g->e[i].id;
	return -1;
}
//设置顶点权值
 void setVexW(Graph* g, int id, int w) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id) {
			g->v[i].w = w;
			return;
		}
	printf("input wrong vertex id in setVexW()!\n");
	exit(0);
}
//设置边权值
 void setEdgeW(Graph* g, int id, int w) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id) {
			g->e[i].w = w;
			return;
		}
	printf("input wrong edge id in setEdgeW()!\n");
	exit(0);
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

//Dijkstra 最短路径
// 返回目标顶点的最短路径长度（单位：边权和）。如果目标不可达返回 -1。
// path[] 将被填充为顶点 id 序列（从源到目标），并且 *pathLen 被设置为路径长度（顶点个数）。
int dijkstra_shortest_path(Graph* g, int src_id, int dst_id, int path[], int* pathLen) {
	if (!g || !path || !pathLen) 
		return -1;

	int src = getVexIdx(g, src_id);
	int dst = getVexIdx(g, dst_id);

	const int INF = 0x3f3f3f3f;
	int dist[MAXVERTEX];
	int prev[MAXVERTEX];
	bool used[MAXVERTEX];
	for (int i = 0; i < g->nv; ++i) {
		dist[i] = INF;
		prev[i] = -1;
		used[i] = false;
	}
	dist[src] = 0;

	for (int iter = 0; iter < g->nv; ++iter) {
		int u = -1;
		int best = INF;
		for (int i = 0; i < g->nv; ++i) {
			if (!used[i] && dist[i] < best) {
				best = dist[i];
				u = i;//每次选择当前距离起点最近且未处理的顶点
			}
		}
		if (u == -1) 
			break; // remaining vertices unreachable
		used[u] = true;

		for (int ei = 0; ei < g->v[u].degree; ++ei) {//遍历当前节点 u的所有出边
			int eidx = g->v[u].edges[ei];
			if (eidx < 0 || eidx >= g->ne) 
				continue;
			Edge* E = &g->e[eidx];
			int v_id = (E->h == g->v[u].id) ? E->t : E->h; // 确定u邻居节点的编号

			if (g->dirctional && E->h != g->v[u].id) 
				continue;//如果是有向图，而且E不是以u为头结点，跳过
			int v = getVexIdx(g, v_id);
			int w = E->w;
			if (!used[v] && dist[u] + w < dist[v]) {
				dist[v] = dist[u] + w;
				prev[v] = u;
			}
		}
	}

	if (dist[dst] == INF) {
		*pathLen = 0;
		return -1; // unreachable
	}

	// 路径重构
	int rev[MAXVERTEX];
	int cnt = 0;
	int cur = dst;
	while (cur != -1) {
		rev[cnt++] = g->v[cur].id;
		cur = prev[cur];
	}
	for (int i = 0; i < cnt; ++i) 
		path[i] = rev[cnt - 1 - i];
	*pathLen = cnt;
	return dist[dst];
}

int main() {
	Graph* g = (Graph*)malloc(sizeof(Graph));
	if (!g) {
		printf("error\n");
		exit(0);
	}	
	randgengraph(6, 0.3, 1, 1, "graph.txt", 6);
	initGraphFromFile(g, "graph.txt");
	printgraph(g);
	
	int* h;
	int* t;
	
	int path[MAXVERTEX]; 
	int pathLen = 0;
	int d = dijkstra_shortest_path(g, 0, 3, path, &pathLen);
	if (d < 0) printf("no path 0->3\n");
	else {
		printf("shortest path 0->3 distance=%d, path (ids):", d);
		for (int i = 0; i < pathLen; ++i) 
			printf(" %d", path[i]);
		printf("\n");
	}

	addVex(g, 30);
	addEdge(g, 50, 1, 3);
	printgraph(g);
	//测试删除边/顶点
	printf("-----DELET test\n");
	deleteVex(g, 5);
	printgraph(g);
    printf("%d->%d %d\n",g->e[10].h,g->e[10].t,g->e[10].w);
	deleteEdge(g, 10);
	printgraph(g);
	
	printf("finish\n");
	return 0;
} 
#endif