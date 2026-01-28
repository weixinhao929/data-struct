//
// Created by 34120 on 2026/1/25.
//

#include <iostream>
#include <vector>
#include <deque>
#include <climits>
#include <stack>
#include <queue>

typedef enum{ UNDISCOVERED,DISCOVERED,VISITED } VStatus;//顶点状态
typedef enum{ UNDETERMINED,TREE,CROSS,FORWARD,BACKWARD } EStatus;//边状态

using namespace std;

template<typename Tv>
struct Vertex {
    Tv data;int inDegree,outDegree;VStatus status;
    int dTime,fTime;
    int parent;
    int priority;//优先级
    Vertex(Tv const& d):data(d),inDegree(0),outDegree(0),status(UNDISCOVERED),
    dTime(-1),fTime(-1),parent(-1),
    priority(INT_MAX){}
};

template<typename Te>
struct Edge {
    Te data;//数据
    int weight;//权重
    EStatus status;//状态
    Edge(Te const& d,int w):
        data(d),weight(w),status(UNDETERMINED){}
    Edge() : weight(0), status(UNDETERMINED) {} // 默认构造函数
};

template<typename Tv,typename Te>
class Graph {
private:
    vector<Vertex<Tv>> V;
    vector<vector<Edge<Te>*>> E;

    void reset() {//边和点都初始化
        for (int i=0;i<n;i++) {
            V[i].status=UNDISCOVERED;
            V[i].dTime=-1;V[i].fTime=-1;
            V[i].parent=-1;
            V[i].priority=INT_MAX;
            for (int j=0;j<n;j++) {
                if (exists(i,j)) {
                    E[i][j]->status()=UNDETERMINED;
                }
            }
        }
    }

    void BFS(int,int &);
    void DFS(int,int &);
    void BCC(int,int&,stack<int>&);
    bool TSort(int, int&,stack<Tv>*);
    template<typename PU>void PFS(int ,PU);
public:
    Graph():n(0),e(0){}

    virtual ~Graph() {
        for (int i=0;i<n;i++) {
            for (int j=0;j<n;j++) {
                if (E[i][j]) delete E[i][j];
            }
        }
    }
    int n,e;
    virtual Tv& vertex(int i){return V[i].data;}//数据
    virtual int inDegree(int i){return V[i].inDegree;}//入度
    virtual int outDegree(int i){return V[i].outDegree;}//出度
    virtual int firstNbr(int i){return nextNbr(i,n);}//首个邻接顶点
    virtual int nextNbr(int i,int j) {//相对于邻居j的下一个邻居
        while ((-1<j)&&(!exists(i,j-1))){--j;};//逆向查找 o(n)
        return j;
    }//邻接表可o(1+outdegree(i))
    virtual VStatus& status(int i){return V[i].status;}//状态
    virtual int& dTime(int i){return V[i].dTime;}//时间标签dTime
    virtual int& fTime(int i){return V[i].fTime;}//时间标签fTime
    virtual int& parent(int i){return V[i].parent;}//遍历树中的父亲
    virtual int& priority(int i){return V[i].priority;}//在遍历树中点优先级数

    //顶点动态操作
    virtual int insert(Tv const& vertex) {//插入顶点
        for (int i=0;i<n;i++)
            E[i].push_back(nullptr);
        n++;
        vector<Edge<Te>*> newRow(n,nullptr);
        E.push_back(newRow);//创建新顶点对应边向量
        V.push_back(Vertex<Tv>(vertex));
        return n-1;//返回新顶点的索引
    }
    virtual Tv remove(int i) {//删除第i个顶点及其关联边
        Tv vBak=vertex(i);
        for (int j=0;j<n;j++) {//所有入边
            if (exists(i,j)) {
                delete E[i][j];//逐条删除
                E[i][j]=nullptr;
                e--;
                --V[j].inDegree;//减掉存在的入度
            }
        }
        for (int j=0;j<n;j++) {//所有出边
            if (exists(j,i)) {
                delete E[j][i];//逐条删除
                E[j].erase(E[j].begin()+i);
                e--;
                --V[j].outDegree;//减掉出度
            }
        }
        E.erase(E.begin()+i);//删除第i行
        n--;
        for (int j=0;j<n;j++) {
            E[j].erase(E[j].begin()+i);
        }//删除j行i列  begin()指向第1个元素
        V.erase(V.begin()+i);//删掉第i个元素
        return vBak;//返回删除的项
    }

    //边的确认函数
    virtual bool exists(int i, int j) {//(边i,j是否存在)
        return (0 <= i && i < n) && (0 <= j && j < n) && E[i][j] != nullptr;
    }//i,j合法且非空
    //边基本函数
    virtual Te& edge(int i,int j){return E[i][j].data;}//边
    virtual int weight(int i,int j){return E[i][j].weight;}//权重
    virtual EStatus& status(int i,int j){return E[i][j].status;}//状态

    virtual void insert(Te const& edge,int w,int i,int j) {//插入权重为w的边e=(i,j)
        if (exists(i,j)) return ;
        E[i][j]=new Edge<Te>(edge,w);//创建新边
        e++;
        ++V[i].outDegree;
        ++V[j].inDegree;//更新边度数
    }
    virtual Te remove(int i,int j) {
        Te eBak=edge(i,j);
        delete E[i][j];
        E[i][j]=nullptr;//备份后删除边记录
        e--;
        --V[i].outDegree;
        --V[j].inDegree;//更新边度数
        return eBak;//返回边信息
    }
    void bfs(int);//广度优先搜索算法
    void dfs(int);//深度优先搜索算法
    void bcc(int);//基于dfs双联通分量分解算法
    stack<Tv>* tSort(int);//dfs的拓补分解算法
    void prim(int);//最小支撑树prim算法
    void dijkstra(int);//最短路径dijkstra算法
    template<typename PU>
    void pfs(int,PU);//优先搜索框架
};

template<typename Tv, typename Te>//顶点类型 边类型
void Graph<Tv, Te>::bfs(int s) {//广度优先搜索
    reset();//初始化
    int clock=0;
    int v=s;
    do {
        if (status(v)==UNDISCOVERED) {
            BFS(v,clock);
        }
    }while (s!=(v=(++v%n)));
}

template<typename Tv, typename Te>
void Graph<Tv, Te>::BFS(int v, int & clock) {//assert:0<=v<n
    queue<int> q;//辅助队列
    status(v)=DISCOVERED;//初始化起点
    q.push(v);
    while (!q.empty()) {//q变空前 循环
        int v=q.front();q.pop();//取出队首顶点v
        dTime(v)=++clock;//更新时间
        for (int u=firstNbr(v);u>-1;u=nextNbr(v,u)) {//枚举所有邻居
            if (status(u)==UNDISCOVERED) {//u没发现过
                status(u)=DISCOVERED;//发现u
                q.push(u);//入队
                status(v,u)=TREE;//处理边
                parent(u)=v;//处理 u v 关系
            }else {
                status(v,u)=CROSS;//如果发现过就是双向的，跨边
            }
        }
        status(v)=VISITED;//当前节点访问完毕
    }
}

template<typename Tv, typename Te>//深度优先搜索DFS
void Graph<Tv, Te>::dfs(int s) {//传入的是序号
    reset();
    int clock=0;
    int v=s;//初始化
    do {
        if (status(v)==UNDISCOVERED) {//到了未发现的点
            DFS(v,clock);//从该点出发DFS
        }
    } while (s!=(v=(++v%n)));//按序号从前往后
}

template<typename Tv, typename Te>
void Graph<Tv, Te>::DFS(int v, int & clock) {
    dTime(v)=++clock;
    status(v)=DISCOVERED;//发现v
    for (int u=firstNbr(v); u>-1;u=nextNbr(v,u)) {//枚举v所有邻居u
        switch (status(u)) {//（找到第一个就往里走走到头）然后才去找上一个的邻居
            case UNDISCOVERED://u未发现
                status(v,u)=TREE;
                parent(u)=v;//树拓展，设置u父亲v
                DFS(u,clock);//继续往后走
                break;
            case DISCOVERED://u发现过
                status(v,u)=BACKWARD;break;
            default://u访问过
                status(v,u)=(dTime(v)<dTime(u))?FORWARD:CROSS;
                break;//比较时间，若在前forward，不然双向
        }
    }
    status(v)=VISITED;
    fTime(v)=++clock;//访问完v，再回到dfs里面往后找
}

template<typename Tv, typename Te>//基于dfs的拓补排序算法
stack<Tv> *Graph<Tv, Te>::tSort(int s) {
    reset();int clock=0;int v=s;
    stack<Tv> *S=new stack<Tv>;//用栈记录顶点
    do {
        if (status(v)==UNDISCOVERED) {
            if (!TSort(v,clock,S)) {
                while (!S->empty()) {
                    S->pop();
                }
                break;
            }
        }
    } while (s!=(v=(++v%n)));
    return S;
}

template<typename Tv, typename Te>
bool Graph<Tv, Te>::TSort(int v, int & clock, stack<Tv> * S) {
    dTime(v)=++clock;status(v)=DISCOVERED;
    for (int u=firstNbr(v); u>-1;u=nextNbr(v,u)) {
        switch (status(u)) {
            case UNDISCOVERED:
                parent(u)=v;
                status(v,u)=TREE;
                if (!TSort(u,clock,S)) {
                    return false;
                }
                break;
            case DISCOVERED:
                status(v)=BACKWARD;
                return false;
            default:
                status(v,u)=(dTime(v)<dTime(u))?FORWARD:CROSS;
                break;
        }
    }
    status(v)=VISITED;S->push(vertex(v));
    return true;
}

template<typename Tv, typename Te>
void Graph<Tv, Te>::bcc(int s) {
    reset();int clock=0;int v=s;stack<int> S;
    do {
        if (status(v)==UNDISCOVERED) {
            BCC(v,clock,S);
            S.pop();
        }
    } while (s!=(v=(++v%n)));
}

#define hca(x) (fTime(x))
template<typename Tv, typename Te>
void Graph<Tv, Te>::BCC(int v, int & clock, stack<int> & S) {
    hca(v)=++clock;dTime(v)=clock;
    status(v)=DISCOVERED;S.push(v);
    for (int u=firstNbr(v);u>-1;u=nextNbr(v,u)) {
        switch (status(u)) {
            case UNDISCOVERED:
                parent(u)=v;status(v,u)=TREE;
                BCC(u,clock,S);
                if (hca(u)<dTime(v)) {
                    hca(v)=min(hca(v),hca(u));
                }else {
                    while (v!=S.top()) {
                        S.pop();
                    }
                }
                break;
            case DISCOVERED:
                status(v,u)=BACKWARD;
                if (u!=parent(v))
                    hca(v)=min(hca(v),dTime(u));
                break;
            default:
                status(v,u)=(dTime(v)<dTime(u))?FORWARD:CROSS;
                break;
        }
    }
    status(v)=VISITED;
}

template<typename Tv, typename Te>
template<typename PU>
void Graph<Tv, Te>::pfs(int s, PU prioUpdater) {
    reset();int v=s;
    do {
        if (status(v)==UNDETERMINED) {
            PFS(v,prioUpdater);
        }
    } while (s!=(v=(++v%n)));
}

template<typename Tv, typename Te>
template<typename PU>
void Graph<Tv, Te>::PFS(int s, PU prioUpdater) {
    priority(s)=0;status(s)=VISITED;parent(s)=-1;
    while (true) {
        for (int w=firstNbr(s);w>-1;w=nextNbr(s,w)) {
            prioUpdater(this,s,w);
        }
        for (int shortest=INT_MAX,w=0;w<n;w++) {
            if (status(w)==UNDISCOVERED) {
                if (shortest>priority(w)) {
                    shortest=priority(w);
                    s=w;
                }
            }
        }
        if (status(s)==VISITED) {
            break;
        }
        status(s)=VISITED;status(parent(s),s)=TREE;
    }
}





