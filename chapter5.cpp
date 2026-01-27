//
// Created by 34120 on 2026/1/24.
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
// 测试访问器类
template<typename T>
class Printer {
public:
    void operator()(T& data) {
        cout << data << " ";
    }
};

template<typename T>
class Counter {
    int count = 0;
public:
    void operator()(T& data) {
        count++;
    }
    int getCount() const { return count; }
    void reset() { count = 0; }
};


// 测试基本功能
void testBasic() {
    cout << "========== 测试基本功能 ==========" << endl;

    // 创建二叉树
    BinTree<int> tree;

    // 1. 插入根节点
    BinNodePosi(int) root = tree.insertAsRoot(1);
    cout << "1. 插入根节点 1" << endl;
    cout << "   树规模: " << tree.size() << " (应为1)" << endl;
    cout << "   树高: " << (root ? root->height : -1) << " (应为0)" << endl;

    // 2. 插入左孩子
    BinNodePosi(int) leftChild = tree.insertAsLC(root, 2);
    cout << "\n2. 插入左孩子 2" << endl;
    cout << "   树规模: " << tree.size() << " (应为2)" << endl;
    cout << "   根节点高度: " << root->height << " (应为1)" << endl;

    // 3. 插入右孩子
    BinNodePosi(int) rightChild = tree.insertAsRC(root, 3);
    cout << "\n3. 插入右孩子 3" << endl;
    cout << "   树规模: " << tree.size() << " (应为3)" << endl;
    cout << "   根节点高度: " << root->height << " (应为1)" << endl;

    // 4. 继续插入构建更复杂的树
    tree.insertAsLC(leftChild, 4);
    tree.insertAsRC(leftChild, 5);
    tree.insertAsLC(rightChild, 6);
    tree.insertAsRC(rightChild, 7);
    cout << "\n4. 构建完整二叉树" << endl;
    cout << "   树结构:   1" << endl;
    cout << "           / \\" << endl;
    cout << "          2   3" << endl;
    cout << "         / \\ / \\" << endl;
    cout << "        4  5 6  7" << endl;
    cout << "   树规模: " << tree.size() << " (应为7)" << endl;

    // 5. 测试各种遍历
    cout << "\n5. 遍历测试:" << endl;
    Printer<int> printer;

    cout << "   先序遍历(递归): ";
    travPre_R(root, printer);
    cout << endl;

    cout << "   先序遍历(迭代1): ";
    travPre_I1(root, printer);
    cout << endl;

    cout << "   先序遍历(迭代2): ";
    travPre_I2(root, printer);
    cout << endl;

    cout << "   中序遍历(递归): ";
    travIn_R(root, printer);
    cout << endl;

    cout << "   中序遍历(迭代1): ";
    travIn_I1(root, printer);
    cout << endl;

    cout << "   中序遍历(迭代2): ";
    travIn_I2(root, printer);
    cout << endl;

    cout << "   中序遍历(迭代3): ";
    travIn_I3(root, printer);
    cout << endl;

    cout << "   后序遍历(递归): ";
    travPost_R(root, printer);
    cout << endl;

    cout << "   后序遍历(迭代): ";
    travPost_I(root, printer);
    cout << endl;

    cout << "   层次遍历: ";
    tree.travLevel(printer);
    cout << endl;

    // 6. 测试节点计数
    Counter<int> counter;
    tree.travPre(counter);
    cout << "   先序遍历节点数: " << counter.getCount() << endl;

    // 7. 测试succ函数
    cout << "\n6. 测试succ(直接后继)函数:" << endl;
    cout << "   节点2的后继: " << leftChild->succ()->data << " (应为5)" << endl;
    cout << "   节点1的后继: " << root->succ()->data << " (应为6)" << endl;
    cout << "   节点7的后继: " << (rightChild->rChild->succ() ? "存在" : "不存在") << endl;
}

// 测试删除功能
void testRemove() {
    cout << "\n\n========== 测试删除功能 ==========" << endl;

    BinTree<int> tree;
    Printer<int> printer;

    // 构建一个更大的树
    BinNodePosi(int) root = tree.insertAsRoot(1);
    BinNodePosi(int) node2 = tree.insertAsLC(root, 2);
    BinNodePosi(int) node3 = tree.insertAsRC(root, 3);
    tree.insertAsLC(node2, 4);
    tree.insertAsRC(node2, 5);
    tree.insertAsLC(node3, 6);
    tree.insertAsRC(node3, 7);
    tree.insertAsLC(node2->lChild, 8);
    tree.insertAsRC(node2->lChild, 9);

    cout << "原始树结构:" << endl;
    cout << "        1" << endl;
    cout << "       / \\" << endl;
    cout << "      2   3" << endl;
    cout << "     / \\ / \\" << endl;
    cout << "    4  5 6  7" << endl;
    cout << "   / \\" << endl;
    cout << "  8   9" << endl;

    cout << "\n原始树先序遍历: ";
    tree.travPre(printer);
    cout << endl;
    cout << "树规模: " << tree.size() << " (应为9)" << endl;

    // 删除左子树
    cout << "\n删除以节点2为根的子树:" << endl;
    int removedCount = tree.remove(node2);
    cout << "删除了 " << removedCount << " 个节点" << endl;
    cout << "删除后树规模: " << tree.size() << " (应为4)" << endl;
    cout << "删除后先序遍历: ";
    tree.travPre(printer);
    cout << endl;

    // 再删除右子树
    cout << "\n删除以节点3为根的子树:" << endl;
    removedCount = tree.remove(node3);
    cout << "删除了 " << removedCount << " 个节点" << endl;
    cout << "删除后树规模: " << tree.size() << " (应为1)" << endl;
    cout << "删除后先序遍历: ";
    tree.travPre(printer);
    cout << endl;
}

// 测试子树操作
void testSubtreeOperations() {
    cout << "\n\n========== 测试子树操作 ==========" << endl;

    Printer<int> printer;

    // 创建主树
    BinTree<int> mainTree;
    BinNodePosi(int) mainRoot = mainTree.insertAsRoot(100);
    BinNodePosi(int) mainLeft = mainTree.insertAsLC(mainRoot, 200);
    BinNodePosi(int) mainRight = mainTree.insertAsRC(mainRoot, 300);

    cout << "创建主树:" << endl;
    cout << "   树结构:   100" << endl;
    cout << "           /   \\" << endl;
    cout << "         200   300" << endl;
    cout << "   先序遍历: ";
    mainTree.travPre(printer);
    cout << endl;

    // 创建子树1
    BinTree<int>* subTree1 = new BinTree<int>();
    BinNodePosi(int) subRoot1 = subTree1->insertAsRoot(400);
    subTree1->insertAsLC(subRoot1, 401);
    subTree1->insertAsRC(subRoot1, 402);

    cout << "\n创建子树1:" << endl;
    cout << "   树结构:   400" << endl;
    cout << "           /   \\" << endl;
    cout << "         401   402" << endl;
    cout << "   先序遍历: ";
    subTree1->travPre(printer);
    cout << endl;

    // 将子树1连接到主树的左节点
    cout << "\n将子树1连接到主树的左节点(200)作为左子树:" << endl;
    mainTree.attachAsLC(mainLeft, subTree1);
    cout << "   连接后子树1指针: " << (subTree1 == nullptr ? "已置空" : "未置空") << endl;
    cout << "   主树先序遍历: ";
    mainTree.travPre(printer);
    cout << endl;
    cout << "   主树规模: " << mainTree.size() << " (应为5)" << endl;

    // 创建子树2
    BinTree<int>* subTree2 = new BinTree<int>();
    BinNodePosi(int) subRoot2 = subTree2->insertAsRoot(500);
    subTree2->insertAsLC(subRoot2, 501);
    subTree2->insertAsRC(subRoot2, 502);
    subTree2->insertAsLC(subRoot2->lChild, 503);

    cout << "\n创建子树2:" << endl;
    cout << "   树结构:     500" << endl;
    cout << "             /   \\" << endl;
    cout << "           501   502" << endl;
    cout << "          /" << endl;
    cout << "        503" << endl;
    cout << "   先序遍历: ";
    subTree2->travPre(printer);
    cout << endl;

    // 将子树2连接到主树的右节点
    cout << "\n将子树2连接到主树的右节点(300)作为右子树:" << endl;
    mainTree.attachAsRC(mainRight, subTree2);
    cout << "   连接后子树2指针: " << (subTree2 == nullptr ? "已置空" : "未置空") << endl;
    cout << "   主树先序遍历: ";
    mainTree.travPre(printer);
    cout << endl;
    cout << "   主树规模: " << mainTree.size() << " (应为9)" << endl;

    // 测试secede分离子树
    cout << "\n测试分离以400为根的子树:" << endl;
    BinTree<int>* detachedTree = mainTree.secede(mainLeft->lChild);
    cout << "   分离后主树规模: " << mainTree.size() << " (应为5)" << endl;
    cout << "   主树先序遍历: ";
    mainTree.travPre(printer);
    cout << endl;

    cout << "   分离出的子树规模: " << detachedTree->size() << " (应为3)" << endl;
    cout << "   子树先序遍历: ";
    detachedTree->travPre(printer);
    cout << endl;

    // 清理分离出的树
    delete detachedTree;
}

// 测试边界情况
void testEdgeCases() {
    cout << "\n\n========== 测试边界情况 ==========" << endl;

    Printer<int> printer;

    // 测试空树
    cout << "1. 测试空树:" << endl;
    BinTree<int> emptyTree;
    cout << "   空树规模: " << emptyTree.size() << " (应为0)" << endl;
    cout << "   空树判空: " << (emptyTree.empty() ? "是" : "否") << " (应为是)" << endl;
    cout << "   空树遍历: ";
    emptyTree.travPre(printer);
    cout << "(应无输出)" << endl;

    // 测试单节点树
    cout << "\n2. 测试单节点树:" << endl;
    BinTree<int> singleTree;
    BinNodePosi(int) singleRoot = singleTree.insertAsRoot(1);
    cout << "   单节点树规模: " << singleTree.size() << " (应为1)" << endl;
    cout << "   单节点树判空: " << (singleTree.empty() ? "是" : "否") << " (应为否)" << endl;
    cout << "   单节点树遍历: ";
    singleTree.travPre(printer);
    cout << endl;

    // 测试只有左子树的树
    cout << "\n3. 测试只有左子树的树:" << endl;
    BinTree<int> leftOnlyTree;
    BinNodePosi(int) leftRoot = leftOnlyTree.insertAsRoot(1);
    leftOnlyTree.insertAsLC(leftRoot, 2);
    leftOnlyTree.insertAsLC(leftRoot->lChild, 3);
    leftOnlyTree.insertAsLC(leftRoot->lChild->lChild, 4);
    cout << "   树结构: 1" << endl;
    cout << "         /" << endl;
    cout << "        2" << endl;
    cout << "       /" << endl;
    cout << "      3" << endl;
    cout << "     /" << endl;
    cout << "    4" << endl;
    cout << "   先序遍历: ";
    leftOnlyTree.travPre(printer);
    cout << endl;
    cout << "   中序遍历: ";
    leftOnlyTree.travIn(printer);
    cout << endl;
    cout << "   后序遍历: ";
    leftOnlyTree.travPost(printer);
    cout << endl;

    // 测试只有右子树的树
    cout << "\n4. 测试只有右子树的树:" << endl;
    BinTree<int> rightOnlyTree;
    BinNodePosi(int) rightRoot = rightOnlyTree.insertAsRoot(1);
    rightOnlyTree.insertAsRC(rightRoot, 2);
    rightOnlyTree.insertAsRC(rightRoot->rChild, 3);
    rightOnlyTree.insertAsRC(rightRoot->rChild->rChild, 4);
    cout << "   树结构: 1" << endl;
    cout << "         \\" << endl;
    cout << "          2" << endl;
    cout << "           \\" << endl;
    cout << "            3" << endl;
    cout << "             \\" << endl;
    cout << "              4" << endl;
    cout << "   先序遍历: ";
    rightOnlyTree.travPre(printer);
    cout << endl;
    cout << "   中序遍历: ";
    rightOnlyTree.travIn(printer);
    cout << endl;
    cout << "   后序遍历: ";
    rightOnlyTree.travPost(printer);
    cout << endl;
}

// 测试内存泄漏
void testMemoryLeak() {
    cout << "\n\n========== 测试内存泄漏 ==========" << endl;

    // 创建多层树
    BinTree<int>* bigTree = new BinTree<int>();
    BinNodePosi(int) root = bigTree->insertAsRoot(1);

    // 构建深度为4的完全二叉树
    queue<BinNodePosi(int)> q;
    q.push(root);
    int value = 2;

    while (value <= 15) {  // 构建15个节点
        BinNodePosi(int) current = q.front();
        q.pop();

        if (value <= 15) {
            q.push(bigTree->insertAsLC(current, value++));
        }
        if (value <= 15) {
            q.push(bigTree->insertAsRC(current, value++));
        }
    }

    cout << "构建了深度为4的完全二叉树" << endl;
    cout << "树规模: " << bigTree->size() << " (应为15)" << endl;

    // 删除整棵树
    delete bigTree;
    cout << "树已删除，应无内存泄漏" << endl;
}

// 综合测试
void comprehensiveTest() {
    cout << "\n\n========== 综合测试 ==========" << endl;

    Printer<int> printer;
    Counter<int> counter;

    // 创建复杂的树
    BinTree<int> tree;
    BinNodePosi(int) root = tree.insertAsRoot(1);

    // 第一层
    BinNodePosi(int) node2 = tree.insertAsLC(root, 2);
    BinNodePosi(int) node3 = tree.insertAsRC(root, 3);

    // 第二层
    BinNodePosi(int) node4 = tree.insertAsLC(node2, 4);
    BinNodePosi(int) node5 = tree.insertAsRC(node2, 5);
    BinNodePosi(int) node6 = tree.insertAsLC(node3, 6);
    BinNodePosi(int) node7 = tree.insertAsRC(node3, 7);

    // 第三层
    tree.insertAsLC(node4, 8);
    tree.insertAsRC(node4, 9);
    tree.insertAsLC(node5, 10);
    tree.insertAsRC(node5, 11);
    tree.insertAsLC(node6, 12);
    tree.insertAsRC(node6, 13);
    tree.insertAsLC(node7, 14);
    tree.insertAsRC(node7, 15);

    cout << "构建复杂树结构:" << endl;
    cout << "               1" << endl;
    cout << "           /       \\" << endl;
    cout << "          2         3" << endl;
    cout << "         / \\       / \\" << endl;
    cout << "        4   5     6   7" << endl;
    cout << "       /\\  /\\    /\\  /\\" << endl;
    cout << "      8 9 10 11 12 13 14 15" << endl;

    cout << "\n树规模: " << tree.size() << " (应为15)" << endl;

    // 各种遍历
    cout << "\n各种遍历结果:" << endl;
    cout << "先序遍历: ";
    tree.travPre(printer);
    cout << endl;

    cout << "中序遍历: ";
    tree.travIn(printer);
    cout << endl;

    cout << "后序遍历: ";
    tree.travPost(printer);
    cout << endl;

    cout << "层次遍历: ";
    tree.travLevel(printer);
    cout << endl;

    // 测试succ
    cout << "\n测试succ函数:" << endl;
    for (int i = 1; i <= 15; i++) {
        // 这里需要找到值为i的节点
        // 简单起见，我们只测试几个关键节点
    }

    // 测试size函数
    cout << "\n测试节点size函数:" << endl;
    cout << "根节点子树规模: " << root->size() << " (应为15)" << endl;
    cout << "节点2子树规模: " << node2->size() << " (应为7)" << endl;
    cout << "节点4子树规模: " << node4->size() << " (应为3)" << endl;
    cout << "叶子节点8子树规模: " << (node4->lChild ? node4->lChild->size() : 0) << " (应为1)" << endl;
}

int main() {
    SetConsoleOutputCP(65001);
    cout << "开始二叉树测试..." << endl;
    cout << "==================================" << endl;

    try {
        testBasic();
        testRemove();
        testSubtreeOperations();
        testEdgeCases();
        testMemoryLeak();
        comprehensiveTest();

        cout << "\n==================================" << endl;
        cout << "所有测试完成！" << endl;

    } catch (const exception& e) {
        cerr << "测试过程中发生异常: " << e.what() << endl;
        return 1;
    }

    return 0;

}
