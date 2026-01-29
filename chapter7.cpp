//
// Created by 34120 on 2026/1/28.
//
#include <windows.h>
#include <iostream>
#include <queue>
#include <stack>
using namespace std;
#define BinNodePosi(T) BinNode<T>*//节点位置
#define stature(p)((p)?(p)->height:-1)//节点高度（与空树高为-1约定相统一）
typedef enum{RB_RED,RB_BLACK} RBColor;//节点颜色

#define IsRoot(x) (!(x)->parent)
#define IsLChild(x) (!IsRoot(x) && (x)==(x)->parent->lChild)
#define IsRChild(x) (!IsRoot(x) && (x)==(x)->parent->rChild)
#define HasLChild(x) ((x)->lChild)
#define HasRChild(x) ((x)->rChild)
#define FromParentTo(x) (IsRoot(x)? _root:(IsLChild(x)?(x)->parent->lChild:(x)->parent->rChild))

template <typename T>//二叉树节点模版类
struct BinNode {
    //成员
    T data;//数值
    BinNodePosi(T) parent;BinNodePosi(T) lChild;BinNodePosi(T) rChild;//父节点及左右孩子
    int height;int npl;RBColor color;//高度  左式堆（Null Path Length，零路径长）   颜色（红黑树）
    // NPL(X) = 0，如果 X 是外部节点（空节点）
    // NPL(X) = 1 + min(NPL(左子), NPL(右子))，如果 X 是内部节点
//构造函数
    BinNode():parent(nullptr),lChild(nullptr),rChild(nullptr),height(0),npl(1),color(RB_RED){}
    BinNode(T e,BinNodePosi(T) p= nullptr,BinNodePosi(T) lc= nullptr,BinNodePosi(T) rc= nullptr,
        int h=0,int l=1,RBColor c=RB_RED)
    :data(e),parent(p),lChild(lc),rChild(rc),height(h),npl(l),color(c){}
//操作接口
    int size();//统计当前节点后代总数，  以其为根的子树规模
    BinNodePosi(T) insertAsLc(T const&);//左孩子插入新节点
    BinNodePosi(T) insertAsRc(T const&);//右孩子插入新节点
    BinNodePosi(T) succ();//取当前节点的直接后继（中序遍历意义下）
    template <typename VST> void travLevel(VST&);//子树层次遍历
    template <typename VST> void travPre(VST&);//子树先序遍历
    template <typename VST> void travIn(VST&);//子树中序遍历
    template <typename VST> void travPost(VST&);//子树后序遍历
//比较器
    bool operator<(BinNode const& bn) {
        return data<bn.data;
    } //小于
    bool operator==(BinNode const& bn) {
        return data==bn.data;
    }//等于
};

//插入孩子节点
template<typename T>
BinNode<T> *BinNode<T>::insertAsLc(T const &e) {
    return lChild = new BinNode<T>(e,this);//左孩子
}

template<typename T>
BinNode<T> *BinNode<T>::insertAsRc(T const &e) {
    return rChild = new BinNode<T>(e,this);//右孩子
}

template<typename T>
int BinNode<T>::size() {//后代总数，以当前为根节点的子树规模
    int s=1;//计入本身
    if (lChild) s += lChild->size();//递归计入左子树规模
    if (rChild) s += rChild->size();//递归计入右子树规模
    return s;
}//o(n=|size|)

template<typename T>template<typename VST>
void BinNode<T>::travPre(VST& visit) {
    travPre_I2(this, visit);
}

template<typename T>template<typename VST>
void BinNode<T>::travIn(VST& visit) {
    travIn_I2(this, visit);
}

template<typename T>template<typename VST>
void BinNode<T>::travPost(VST& visit) {
    travPost_I(this, visit);
}

template<typename T>
class BinTree {//二叉树
protected:
    int _size;//规模
    BinNodePosi(T) _root;//根节点
    virtual int updateHeight(BinNodePosi(T) x);//更新节点x的高度
    void updateHeightAbove(BinNodePosi(T) x);//更新节点x及其祖先的高度
public:
    BinTree() :_size(0),_root(nullptr) {}//构造函数
    ~BinTree(){//析构函数
        if (_size>0) {
            remove(_root);
        }
    }
    int size() const {
        return _size;
    }//规模
    bool empty()const {//判空
        return !_root;
    }
    BinNodePosi(T) root() {
        return _root;//树根
    }
    BinNodePosi(T) insertAsRoot(T const& e) ;//插入根节点
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x,T const& e);//e作为x左孩子插入
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x,T const& e);//e作为x右孩子插入
    BinNodePosi(T) attachAsLC(BinNodePosi(T) x,BinTree<T>* &S);//S作为左子树接入
    BinNodePosi(T) attachAsRC(BinNodePosi(T) x,BinTree<T>* &S);//S作为右子树接入
    void release(BinTree<T>* tree);
    int remove(BinNodePosi(T) x);//删除以x为根的子树，返回该子树原先规模
    BinTree<T>* secede(BinNodePosi(T) x);//将x子树摘除，变成新的独立子树
    template <typename VST>
    void travLevel(VST& visit) {//层次遍历
        if (_root) _root->travLevel(visit);
    }
    template <typename VST>//先序遍历
    void travPre(VST& visit) {
        if (_root) _root->travPre(visit);
    }
    template <typename VST>
    void travIn(VST& visit) {//中序遍历
        if (_root) _root->travIn(visit);
    }
    template <typename VST>
    void travPost(VST& visit) {//后序遍历
        if (_root) _root->travPost(visit);
    }
    bool operator<(BinTree<T> const& t) {//比较器
        return _root&&t._root&&_root<t._root;
    }
    bool operator==(BinTree<T> const& t) {//判等器
        return _root&&t._root&&_root==t._root;
    }
};

template <typename T>
int BinTree<T>::updateHeight(BinNodePosi(T) x) {//更新节点x高度
    return x->height=1+max(stature(x->lChild),stature(x->rChild));
}

template<typename T>
void BinTree<T>::updateHeightAbove(BinNode<T> *x) {//更新v及祖先高度
    while (x) {
        updateHeight(x);
        x=x->parent;//可优化 高度未变则停止
    }
}

// template<typename T>
// void BinTree<T>::updateHeightAbove(BinNode<T> *x) {//更新v及祖先高度(优化)
//     while (x) {
//         if (x->parent->height!=x->height) {
//             updateHeight(x);
//             x=x->parent;//可优化 高度未变则停止
//         }
//         break;
//     }
// }

template<typename T>
BinNode<T> *BinTree<T>::insertAsRoot(T const &e) {//将e当做根节点插入空二叉树
    _size=1;
    return _root=new BinNode<T>(e);
}

template<typename T>
BinNode<T> *BinTree<T>::insertAsLC(BinNode<T> *x, T const &e) {//e插入为左孩子
    _size++;
    x->insertAsLc(e);
    updateHeightAbove(x);
    return x->lChild;
}

template<typename T>
BinNode<T> *BinTree<T>::insertAsRC(BinNode<T> *x, T const &e) {//e插入为右孩子
    _size++;
    x->insertAsRc(e);
    updateHeightAbove(x);//祖先高度可能增加，别的必然不变
    return x->rChild;
}

template<typename T>
BinNode<T> *BinTree<T>::attachAsLC(BinNode<T> *x, BinTree<T> *&S) {
    if (x == nullptr) {//x为空的情况
        return nullptr;
    }
    if (S == nullptr || S->_root == nullptr) {//S为空或S->_root为空的情况
        return x;
    }
    if (x->lChild != nullptr) {
        //检查x是否已有左孩子
        // 处理方式1：先分离并删除原来的左子树
        remove(x->lChild);
        // 处理方式2：抛出异常
        // throw std::runtime_error("attachAsLC: x already has left child");
    }
    x->lChild =S->_root;
    S->_root->parent=x;
    _size+=S->_size;
    updateHeightAbove(x);//更新全树规模及x所有祖先高度
    S->_root=NULL;
    S->_size=0;
    release(S);//释放原树
    S=nullptr;
    return x;//返回接入节点
}

template<typename T>
BinNode<T> *BinTree<T>::attachAsRC(BinNode<T> *x, BinTree<T> *&S) {
        if (x == nullptr) {//x为空的情况
            return nullptr;
        }
        if (S == nullptr || S->_root == nullptr) {//S为空或S->_root为空的情况
            return x;
        }
        if (x->rChild != nullptr) {//检查x是否已有右孩子
            // 处理方式1：先分离并删除原来的右子树
            remove(x->rChild);
            // 处理方式2：抛出异常
            // throw std::runtime_error("attachAsLC: x already has right child");
        }
    x->rChild =S->_root;
    S->_root->parent=x;
    _size+=S->_size;
    updateHeightAbove(x);//更新全树规模及x所有祖先高度
    S->_root=NULL;
    S->_size=0;
    release(S);//释放原树
    S=nullptr;
    return x;//返回接入节点
}

template<typename T>
void BinTree<T>::release(BinTree<T>* tree) {
    if (tree) {
        if (tree==this) {
            if (_root) {
                remove(_root);
            }
        }else {
            if (tree->_root) {
                removeAt(tree->_root);
                tree->_root = nullptr;
                tree->_size = 0;
            }
            delete tree;
        }
    }
}

template<typename T>
int BinTree<T>::remove(BinNode<T> *x) {
    if (!x) return 0;
    FromParentTo(x)=nullptr;//切断父节点的指针
    updateHeightAbove(x->parent);//更新祖先高度
    int n=removeAt(x);//删除子树
    _size-=n;//更新规模
    return n;//返回删除节点总数
}

template<typename T>
static int removeAt(BinNodePosi(T) x) {//assert:二叉树中合法位置
    if (!x) return 0;//递归基空树
    int n=1+removeAt(x->lChild)+removeAt(x->rChild);//递归释放左右子树
    delete x;
    return n;//返回删除总数
}

template<typename T>
BinTree<T> *BinTree<T>::secede(BinNode<T> *x) {//分离树
    FromParentTo(x)=NULL;//切断父节点的指针
    updateHeightAbove(x->parent);
    BinTree<T>* S=new BinTree<T>;//新树以x为根
    S->_root=x;
    x->parent=NULL;
    S->_size=x->size();_size-=S->_size;//更新规模
    return S;//返回子树
}

template<typename T,typename VST>
void travPre_R(BinNodePosi(T) x,VST& visit) {//先序遍历递归版  中左右
    if (!x) return;
    visit(x->data);
    travPre_R(x->lChild,visit);
    travPre_R(x->rChild,visit);
}

template<typename T,typename VST>
void travPre_I1(BinNodePosi(T) x,VST & visit) {//先序遍历迭代1
    stack<BinNodePosi(T)> S;//辅助栈
    if (x) S.push(x);//根节点入栈
    while (!S.empty()) {//在栈空前循环
        x=S.top();
        S.pop();
        visit(x->data);//弹出并访问当前节点
        if (HasRChild(x))
            S.push(x->rChild);//右孩子先入后出
        if (HasLChild(x))
            S.push(x->lChild);//左孩子后入先出
    }
}

template<typename T,typename VST>//元素类型，操作器
static void visitAlongLeftBranch(BinNodePosi(T) x,VST& visit,stack<BinNodePosi(T)>& S) {
    while (x) {
        // visit(x->data);           // 访问当前节点
        // S.push(x->rChild);    // 右孩子入栈
        // x = x->lChild;            // 沿左分支深入

        // visit(x->data);//访问当前节点
        // if (x->rChild==NULL) {
        //     //若没右孩子跳过
        //     x=x->lChild;//沿左分支深入一层
        //     continue;
        // }
        // S.push(x->rChild);//右孩子入栈
        // x=x->lChild;//沿左分支深入一层
        visit(x->data);           // 访问当前节点
        if (x->rChild) {          // 如果有右孩子
            S.push(x->rChild);    // 右孩子入栈
        }
        x = x->lChild;            // 沿左分支深入
    }
}

template<typename T,typename VST>
void travPre_I2(BinNodePosi(T) x,VST& visit) {//先序遍历迭代2
    stack<BinNodePosi(T)> S;//辅助栈
    while (true) {
        visitAlongLeftBranch(x,visit,S);//从当前节点出发，逐批访问
        if (S.empty()) break;//直到栈空
        x=S.top();//弹出下一批起点
        S.pop();
    }
}

template<typename T,typename VST>
void travPost_R(BinNodePosi(T) x,VST& visit) {//后序遍历递归版
    if (!x) return;
    travPost_R(x->lChild,visit);
    travPost_R(x->rChild,visit);
    visit(x->data);
}

template<typename T>//以S栈顶节点为根子树中找到最高左侧可见叶节点
static void gotoHLVFL(stack<BinNodePosi(T)>& S) {//沿途节点一次入栈
    while (BinNodePosi(T) x=S.top())
        if (HasLChild(x)) {//尽可能向左
            if (HasRChild(x)) S.push(x->rChild);//若有右孩子，依次入栈
            S.push(x->lChild);//然后才转向左孩子
        }else//不行才向右
            S.push(x->rChild);
    S.pop();//返回之前弹出栈顶空节点
}

// //后序遍历迭代
// template<typename T,typename VST>
// void travPost_I(BinNodePosi(T) x,VST& visit) {
//     stack<BinNodePosi(T)>S;//辅助栈
//     if (x) S.push(x);//根节点入栈
//     while (!S.empty()) {
//         if (S.top()!=x->parent)//栈顶不是当前节点的父亲，就是右边的兄弟
//             gotoHLVFL(S);//在以右兄为跟的子树中
//         x=S.top();
//         S.pop();
//         visit(x->data);
//     }
// }

template<typename T,typename VST>
void travPost_I(BinNodePosi(T) x,VST& visit) {
    if (!x) return;
    stack<BinNodePosi(T)> S;
    BinNodePosi(T) prev = nullptr;  // 记录前一个访问的节点
    while (x || !S.empty()) {
        while (x) {
            S.push(x);
            x = x->lChild;
        }
        x = S.top();
        if (!x->rChild || x->rChild == prev) {
            S.pop();
            visit(x->data);
            prev = x;
            x = nullptr;
        } else {
            x = x->rChild;
        }
    }
}

template<typename T,typename VST>
void travIn_R(BinNodePosi(T) x,VST& visit) {//中序遍历递归版
    if (!x) return;
    travIn_R(x->lChild,visit);
    visit(x->data);
    travIn_R(x->rChild,visit);
}

template<typename T>//中序遍历迭代版1
static void goAlongLeftBranch(BinNodePosi(T) x,stack<BinNodePosi(T)>& S) {
    while (x) {
        S.push(x);
        x=x->lChild;//反复压栈，一直往左分支深入
    }
}

template<typename T,typename V>
void travIn_I1(BinNodePosi(T) x,V& visit) {
    stack<BinNodePosi(T)> S;//辅助栈
    while (true) {//反复
        goAlongLeftBranch(x,S);//从当前节点出发往左深入，中间的都压栈
        if (S.empty())
            break;//直到所有节点完毕
        x=S.top();//回到上面的一个节点
        S.pop();
        visit(x->data);//访问
        x=x->rChild;//看右子树
    }
}

template<typename T,typename VST>//中序遍历迭代版2
void travIn_I2(BinNodePosi(T) x,VST& visit) {
    stack<BinNodePosi(T)> S;//辅助栈
    while(true) {
        if (x) {
            S.push(x);//根节点进栈
            x=x->lChild;//深入左子树
        }else if (!S.empty()) {
            x=S.top();//回到上一个祖先节点
            S.pop();
            visit(x->data);
            x=x->rChild;//看右子树
        }else
            break;
    }
}

template<typename T,typename VST>//中序遍历迭代版3
void travIn_I3(BinNodePosi(T) x,VST& visit) {
    bool backtrack=false;//前一步是否从右子树回溯，01辅助空间
    while (true) {
        if (!backtrack&&HasLChild(x))//有左子树且刚刚没回溯
            x=x->lChild;//深入左子树
        else {//无左子树，或者刚刚回溯
            visit(x->data);
            if (HasRChild(x)) {//若右子树非空
                x=x->rChild;//进入右子树
                backtrack=false;//
            }else {//右子树空
                if (!((x=x->succ())))
                    break;//回溯（抵达末节点时退出）
                backtrack=true;//设置回溯标准
            }
        }
    }
}


template<typename T>
BinNode<T> *BinNode<T>::succ() {//定位直接后继
    BinNodePosi(T) s=this;
    if (rChild) {//若有右孩子，则后继必在右子树中最靠左（最小）节点
        s=rChild;
        while (HasLChild(s)) {
            s=s->lChild;//最靠左的节点
        }
    }else {//不然就是当前节点包含于其左子树的最低祖先
        while (s->parent && s==s->parent->rChild){
            s=s->parent;
        }
        s=s->parent;
    }
    return s;
}

template<typename T>template<typename VST>
void BinNode<T>::travLevel(VST& visit) {//层次遍历
    queue<BinNodePosi(T)> Q;//辅助队列
    Q.push(this);//根节点入队
    while (!Q.empty()) {//队列变空之前反复迭代
        BinNodePosi(T) x= Q.front();
        Q.pop();
        visit(x->data);//访问队首节点
        if (HasLChild(x))//有左孩子就入左孩子
            Q.push(x->lChild);
        if (HasRChild(x))//入右孩子
            Q.push(x->rChild);
    }
}






template<typename K,typename V>
struct Entry {//词条模版类
    K key;V value;//关键码 数值
    Entry(K k=K(),V v=V()):key(k),value(v){}//默认构造
    Entry(Entry<K,V>const & e):key(e.key),value(e.value){}//克隆
    bool operator< (Entry<K,V> const& e){return key<e.key;}//操作符重载
    bool operator> (Entry<K,V> const& e){return key>e.key;}
    bool operator==(Entry<K,V> const& e){return key==e.key;}
    bool operator!=(Entry<K,V> const& e){return key!=e.key;}
};

template<typename T>
class BST:public BinTree<T> {//bintree派生
public://以virtual修饰以便派生类重写
    virtual BinNodePosi(T)& search(const T&);//查找
    virtual BinNodePosi(T) insert(const T&);//插入
    virtual bool remove(const T &);//删除
protected:
    BinNodePosi(T) _hot;//父节点
    BinNodePosi(T) connect34(//3+4重构
        BinNodePosi(T),BinNodePosi(T),BinNodePosi(T),
        BinNodePosi(T),BinNodePosi(T),BinNodePosi(T),BinNodePosi(T));
    BinNodePosi(T) rotateAt(BinNodePosi(T) x);//旋转调整
    using BinTree<T>::_root;  // 添加这行
    using BinTree<T>::updateHeightAbove;  // 添加这行
};

template<typename T>//查找
BinNode<T> *&BST<T>::search(const T & e) {
    return searchIn(this->root(),e,_hot=NULL);//从根节点开始，根节点父亲设置为空
}

template<typename T>
static BinNodePosi(T)& searchIn(BinNodePosi(T)& v,const T&e,BinNodePosi(T)& hot) {//尾递归，可改为迭代版
    //树根 目标 父亲
    if (!v||e==v->data) return v;//如果为空，或者找到了，就返回当前
    hot=v;//记住当前节点
    return searchIn(((e<v->data)?v->lChild:v->rChild),e,hot);//小了就往左，大了往右
}//时间复杂度o(h)

template<typename T>
BinNode<T> *BST<T>::insert(const T & e) {
    BinNodePosi(T) &x=search(e);//先查找
    if (!x) {//如果不存在
        x=new BinNode<T>(e,_hot);//以hot父节点
        ++this->_size;//更新规模
        updateHeightAbove(x);//更新祖先高度
    }
    return x;//不管有没有都能返回x
}

template<typename T>
bool BST<T>::remove(const T & e) {
    BinNodePosi(T) &x=search(e);//定位目标节点
    if (!x) {
        return false;
    }//目标不存在删除失败
    removeAt(x,_hot);
    --this->_size;//更新规模
    updateHeightAbove(x);//更新祖先高度
    return true;
}

template<typename T>
static BinNodePosi(T) removeAt(BinNodePosi(T)&x,BinNodePosi(T)& hot) {
    BinNodePosi(T) w=x;
    BinNodePosi(T) succ=NULL;
    if (!HasLChild(*x)) {x=x->rChild;succ=x;}//左空就用右子树
    else if (!HasRChild(*x)) {x=x->lChild;succ=x;}//右空用左子树
    else//左右都不空
    {
        w=w->succ();swap(x->data,w->data);//x与w后继交换数据
        BinNodePosi(T) u=w->parent;
        succ=w->rChild;
        (u==x?u->rChild:u->lChild)=succ;//摘除非二度节点
    }
    hot=w->parent;
    if (succ)succ->parent=hot;
    delete w;
    return succ;
}

template<typename T>//基于BST的AVL树
class AVL:public BST<T> {
public:
    BinNodePosi(T) insert(const T & e);//插入
    bool remove(const T & e);//删除
    //search及其余接口可以沿用
    using BST<T>::_hot;  // 添加这行
    using BST<T>::rotateAt;  // 添加这行
};

#define Balanced(x) (stature((x).lChild)==stature((x).rChild))//节点高度一致,理想平衡条件
#define BalFac(x) (stature((x).lChild)-stature((x).rChild))//平衡因子
#define AvlBalanced(x) ((-2<BalFac(x))&&(BalFac(x)<2))//AVL平衡条件，差两行以内

#define tallerChild(x)(stature((x)->lChild) > stature((x)->rChild)?(x)->lChild:(\
        stature((x)->rChild) > stature((x)->lChild)?(x)->rChild:(\
        IsLChild(*(x)) ? (x)->lChild:(x)->rChild)))
//在左右孩子中取最高者，等高则与父亲同侧优先

template<typename T>
BinNode<T> *AVL<T>::insert(const T &e) {
    BinNodePosi(T) &x=search(e);
    if (x) return x;
    x=new BinNode<T>(e,this->_hot); ++this->_size;
    for (BinNodePosi(T) g=this->_hot;g;g=g->parent) {
        if (!AvlBalanced(*g)) {
            FromParentTo(*g) =rotateAt(tallerChild(tallerChild(g)));
            break;
        }else
            updateHeight(g);
    }
    return x;
}

template<typename T>
bool AVL<T>::remove(const T &e) {
    BinNodePosi(T)& x=search(e);if (!x) return false;
    removeAt(x,this->_hot);
    --this->size();
    for (BinNodePosi(T) g=this->_hot;g;g=g->parent) {
        if (!AvlBalanced(*g))
            g=FromParentTo(*g)=rotateAt(tallerChild(tallerChild(g)));
        updateHeight(g);
    }
    return true;
}

template<typename T>
BinNode<T> *BST<T>::connect34(
    BinNode<T> * a, BinNode<T> * b, BinNode<T> * c,
    BinNode<T> * T0, BinNode<T> * T1, BinNode<T> * T2, BinNode<T> * T3) {

    a->lChild=T0;if (T0) T0->parent=a;
    a->rChild=T1;if (T1) T1->parent=a; updateHeight(a);
    c->lChild=T2;if (T2) T2->parent=c;
    c->rChild=T3;if (T3) T3->parent=c; updateHeight(c);
    b->lChild=a;a->parent=b;
    b->rChild=c;c->parent=b;
    updateHeight(b);
    return b;
}

template<typename T>
BinNode<T> *BST<T>::rotateAt(BinNode<T> *v) {
    BinNodePosi(T) p=v->parent;BinNodePosi(T) g=p->parent;
    if (IsChild(*p)) {
        if (IsLChild(*v)) {//zig-zig
            p->parent=g->parent;
            return connect34(v,p,g,v->lChild,v->rChild,p->rChild,g->rChild);
        }else {//zig-zag
            v->parent=g->parent;
            return connect34(p,v,g,p->lChild,v->lChild,v->rChild,g->rChild);
        }
    }else {
        if (IsRChild(*v)) {//zag-zag
            p->parent=g->parent;
            return connect34(g,p,v,g->lChild,p->lChild,v->lChild,v->rChild);
        }else {//zag-zig
            v->parent=g->parent;
            return connect34(g,v,p,g->lChild,v->lChild,v->rChild,p->rChild);
        }
    }
}




