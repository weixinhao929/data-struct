//
// Created by 34120 on 2026/1/21.
//
#include <iostream>
#include <windows.h>
using namespace std;

typedef int Rank;//秩
#define ListNodePosi(T) ListNode<T>*//列表节点位置


template<typename T>
struct ListNode {//节点模版类（双向链表）
    //成员
    T data;ListNodePosi(T) pred;ListNodePosi(T) succ;//数值，前驱，后继
    //构造函数
    ListNode() : pred(nullptr), succ(nullptr) {
    }
    explicit ListNode(T e,ListNodePosi(T) p,ListNodePosi(T) s)
        :data(e),pred(p),succ(s){}//默认构造
    //操作接口
    ListNodePosi(T) insertAsPred(T const& e);
    ListNodePosi(T) insertAsSucc(T const& e);
};

template<typename T>
class List {//列表模版类
private:
    int _size;ListNodePosi(T) header;ListNodePosi(T) trailer;//规模，头哨兵，尾哨兵
protected:
    void init();//创建列表初始化
    int clear();//清除所有节点
    void copyNodes(ListNodePosi(T),int n);//复制列表中自位置p起的n项
    void merge(ListNodePosi(T)&,int,List<T>& L,ListNodePosi(T)&,int);//有序列表区间归并
    void insertionSort(ListNodePosi(T)&,int n);
    void mergeSort(ListNodePosi(T)&,int n);//对从p开始的连续n个节点归并排序
    void selectionSort(ListNodePosi(T)&,int n);//对从p开始的n个节点插入排序
public:
    //构造函数
    List(){init();}//默认
    List(List<T> const& L);//整体复制L
    List(List<T> const& L,Rank r,int n);//复制列表L中自r项起的n项
    List(ListNodePosi(T) p,int n);//复制列表L中自位置p起的n项
    //析构函数
    ~List();//释放包括头尾哨兵的所有节点
    //只读访问接口
    Rank size() const{return _size;}//规模
    bool empty() const{return _size<=0;}//判空
    T& operator[](Rank r) const;//重载，支持寻秩访问
    ListNodePosi(T) first(){return header->succ;}//首节点位置
    ListNodePosi(T) last(){return trailer->pred;}//末节点位置
    bool vaild(ListNodePosi(T) p) {//判断位置p是否合法
        return p&&(trailer!=p)&&(header!=p);//非头非尾非空
    }
    [[nodiscard]] int disordered()const;//判断是否已经排序
    ListNodePosi(T) find(T const& e,int n,ListNodePosi(T) p) const;//无序列表区间查找
    [[nodiscard]] ListNodePosi(T) find(T const& e) const {//无序列表查找
        return find(e,_size,trailer);
    }
    ListNodePosi(T) search(T const& e,int n,ListNodePosi(T) p) const;//有序列表区间查找
    [[nodiscard]] ListNodePosi(T) search(T const& e) const {//有序列表查找
        return search(e,_size,trailer);
    }
    ListNodePosi(T) selectMax(ListNodePosi(T) p,int n);//在p及前n-1个后继中查找最大值
    ListNodePosi(T) selectMax() {//全局最大者
        return selectMax(header->succ,_size);
    }

    ListNodePosi(T) insertAsFirst(T const& e);//将e头插
    ListNodePosi(T) insertAsLast(T const& e);//尾插
    ListNodePosi(T) insertBefore(ListNodePosi(T) p,T const& e);//将e前驱插入
    ListNodePosi(T) insertAfter(ListNodePosi(T) p,T const& e);//后驱插入
    T remove(ListNodePosi(T) p);//删除位置p的节点，返回被删除节点
    void merge(List<T>& L) {//全列表归并
        merge(first(),_size,L,L.first(),L.size());
    }
    void sort(ListNodePosi(T) p,int n);//区间排序
    void sort() {
        sort(first(),_size);
    }//整体排序
    int deduplicate();//无序去重
    int uniquify();//有序去重
    void reverse();//翻转列表
//遍历
    void traverse(void (*)(T&));//依次实施visit操作(函数指针，只读或局部修改)
    template<typename VST>//操作器
    void traverse(VST&);//依次实施visit操作(函数对象，可全局性修改)
};//List

template<typename T>
void List<T>::init() {//列表初始化，创建列表对象时统一调用
    //头哨兵->尾哨兵,头哨兵<-尾哨兵
    header=new ListNode<T>;//头前尾后都为空
    trailer=new ListNode<T>;
    header->pred=nullptr;
    trailer->succ=nullptr;
    header->succ=trailer;
    trailer->pred=header;
    _size=0;//记录规模
}

template<typename T>
//重载下标操作符，以便通过秩直接访问列表节点(虽方便，但效率低)
T &List<T>::operator[](Rank r) const {//assert:<=r<size
    ListNodePosi(T) p = first();//从首节点出发
    while (r--) {//顺r个节点
        p=p->succ;
    }
    return p->data;//返回目标节点数据域
}

template<typename T>//无序列表内节点p的n个前驱中，找到等于e的最后者
ListNodePosi(T) List<T>::find(T const& e,int n,ListNodePosi(T) p) const {
    while (0<n--) {//对于p的n个前驱，从右到左
        if (e==(p=p->pred)->data) {//到命中或越界
            return p;
        }
    }
    return nullptr;//越界则失败
}

template<typename T>
ListNodePosi(T) List<T>::insertAsFirst(T const &e) {//首节点插
    _size++;
    return header->insertAsSucc(e);
}

template<typename T>
ListNode<T> *List<T>::insertAsLast(T const &e) {//末节点插
    _size++;
    return trailer->insertAsPred(e);
}

template<typename T>
ListNodePosi(T) List<T>::insertBefore(ListNode<T> *p, T const &e) {//前驱插
    _size++;
    return p->insertAsPred(e);
}

template<typename T>
ListNodePosi(T) List<T>::insertAfter(ListNode<T> *p, T const &e) {//后继插
    _size++;
    return p->insertAsSucc(e);
}

template<typename T>
ListNode<T> *ListNode<T>::insertAsPred(T const &e) {//将e插在当前节点之前
    ListNodePosi(T) x=new ListNode(e,pred,this);//新节点
    pred->succ=x;
    pred=x;
    return x;
}

template<typename T>
ListNode<T> *ListNode<T>::insertAsSucc(T const &e) {//后插
    ListNodePosi(T) x=new ListNode(e,this,succ);//设置前驱为x
    //ListNodePosi(T) x=new ListNode(e,this,this->succ);
    this->succ->pred=x;
    this->succ=x;
    return x;
}

template<typename T>//成员函数    基于复制的构造
void List<T>::copyNodes(ListNode<T> * p, int n) {//复制p起的n项，至少有n-1个后继
    this->init();//可以省略this
    while (n--) {
        this->insertAsLast(p->data);//可以省略this
        p=p->succ;
    }
}

template<typename T>//p合法 且有至少n-1个后继
List<T>::List(ListNode<T> *p, int n) {
    copyNodes(p,n);//复制p起后n项
}

template<typename T>
List<T>::List(List<T> const &L) {
    copyNodes(L.first(),L.size());//整体复制列表L
}

template<typename T>
List<T>::List(List<T> const &L, Rank r, int n) {
    copyNodes(L[r],n);//复制L r项起的n项
}

template<typename T>
T List<T>::remove(ListNode<T> *p) {//释放p处节点，返回其数值
    T e=p->data;//备份节点数值
    p->pred->succ=p->succ;
    p->succ->pred=p->pred;//后继前驱连起来
    delete p;//释放节点
    _size--;//更新规模
    return e;//返回备份数值
}

template<typename T>
List<T>::~List() {//析构函数
    clear();
    delete trailer;
    delete header;//清空列表 释放节点
}

template<typename T>
int List<T>::clear() {
    int old_size=_size;
    while (0<_size--) {
        remove(header->succ);
    }
    return old_size;
}

template<typename T>
int List<T>::deduplicate() {//去掉无序列表中点重复元素
    if (_size<2) {
        return 0;
    }
    int old_size=_size;//原始规模
    ListNodePosi(T) p=first();//从首节点开始
    Rank r=1;//第一个开始
    while (trailer!=(p=p->succ)) {//依次往后遍历至尾哨兵
        ListNodePosi(T) q=find(p->data,r,p);//查找p前r个
        if (q){//若有则删除，不然继续往后走，r++
            remove(q);
        }else {
            r++;
        }
    }
    return old_size-_size;//返回删除个数
}

// template<typename T>
// int List<T>::uniquify() {//去除重复元素
//     if (_size<2) {
//         return 0;
//     }
//     int old_size=_size;//记录原规模
//     ListNodePosi(T) p;//前
//     ListNodePosi(T) q;//后
//     for (p=header,q=p->succ;trailer!=q;p=q,q=p->succ) {//从左向右扫描
//         if (p->data==q->data) {//若相同删后面的
//             remove(q);
//             q=p;
//         }
//     }
//     return old_size-_size;//删除总数
// }

template<typename T>
int List<T>::uniquify() {
    if (_size < 2) return 0;
    int old_size = _size;
    ListNodePosi(T) p = first();

    while (p->succ != trailer) {
        if (p->data == p->succ->data) {
            remove(p->succ);  // 删除后继
        } else {
            p = p->succ;  // 移动到下一个节点
        }
    }
    return old_size - _size;
}

template<typename T>//在p的n个前驱中，找不大于e的最后者
ListNode<T> *List<T>::search(T const &e, int n, ListNode<T> *p) const {
    while (0<=n--) {//n个前驱逐个比较
        if ((p=p->pred)->data<=e) {//命中或更小则跳出
            break;
        }
    }
    return p;//返回终止位置
}

template<typename T>
void List<T>::sort(ListNode<T> *p, int n) {//排序
    switch (rand()%3) {
        case 1:insertionSort(p,n);break;
        case 2:selectionSort(p,n);break;
        default:mergeSort(p,n);break;
    }
}

// template<typename T>//对起始于p的n个元素排序
// void List<T>::selectionSort(ListNodePosi(T)& p,int n) {//vaild(p)&&rank(p)+n<=_size
//     ListNodePosi(T) head=p->pred;
//     ListNodePosi(T) tail=p;
//     for (int i=0;i<n;i++) {
//         tail=tail->succ;//待排区间(head,tail)
//     }
//     while (1<n) {//在至少还剩两个节点之前，待排序间内
//         ListNodePosi(T) max=selectMax(head->succ,n);//找出最大值   比较费时
//         insertBefore(tail,remove(max));//将其移至tail前
//         tail=tail->pred;//往前进一位
//         n--;
//     }
// }//o(n2)


template<typename T>//对起始于p的n个元素排序
void List<T>::selectionSort(ListNodePosi(T)& p,int n) {//vaild(p)&&rank(p)+n<=_size
    ListNodePosi(T) head=p->pred;
    ListNodePosi(T) tail=p;
    for (int i=0;i<n;i++) {
        tail=tail->succ;//待排区间(head,tail)
    }
    while (1<n) {//在至少还剩两个节点之前，待排序间内
        ListNodePosi(T) max=selectMax(head->succ,n);//找出最大值   比较费时
        swap(tail->pred->data,max->data);
        tail=tail->pred;//往前进一位
        n--;
    }
}//o(n2) new delete消耗大直接换会更快


template<typename T>//从p开始的n个元素中最大（包括p）
ListNode<T> *List<T>::selectMax(ListNode<T> *p, int n) {
    ListNodePosi(T) max=p;//最大者暂定p
    for (ListNodePosi(T) cur=p;1<n;n--) {//逐一往后
        if ((cur=cur->succ)->data>=max->data) {//若后面的大
            max=cur;//更新
        }
    }
    return max;//返回最大节点位置
}

// template<typename T>//插入排序
// void List<T>::insertionSort(ListNode<T> *& p, int n) {
//     for (int r=0;r<n;r++) {
//         insertAfter(search(p->data,r,p),p->data);//放到查找到的恰好大于的元素的后面
//         p=p->succ;
//         remove(p->pred);//下一节点并删除这个节点
//     }
// }

template<typename T>//插入排序
void List<T>::insertionSort(ListNode<T> *& p, int n) {
    for (int r=0;r<n;r++) {
        swap(search(p->data,r,p)->succ->data,p->data);//直接交换
        p=p->succ;
    }
}//o(n2)

template<typename T>
void List<T>::merge(ListNodePosi(T)& p,int n,List<T>& L,ListNodePosi(T)& q,int m) {
    ListNodePosi(T) pp=p->pred;//借助前驱
    while (0<m) {
        if (0<n&&(p->data<=q->data)){
            if (q==(p=p->succ)) {
                break;
            }
            n--;
        }else {
            insertBefore(p,L.remove((q=q->succ)->pred));
            m--;
        }
    }
    p=pp->succ;
}

template <typename T>
void List<T>::mergeSort(ListNodePosi(T)& p,int n) {
    if (n<2) {
        return ;
    }
    int m=n>>1;
    ListNodePosi(T) q=p;
    for (int i=0;i<m;i++) {
        q=q->succ;
    }
    mergeSort(p,m);
    mergeSort(p,n-m);
    merge(p,m,*this,q,n-m);
}

// template<typename T>
// void List<T>::merge(ListNodePosi(T)& p, int n, ListNodePosi(T)& q, int m) {
//     // 这个版本的merge函数合并两个有序区间[p, p+n)和[q, q+m)
//     // 假设p和q是同一个链表中的两个相邻有序区间
//     // 合并后的结果放在p开始的位置
//
//     ListNodePosi(T) head = p->pred; // 保存p的前驱
//     ListNodePosi(T) tail = q;
//     for (int i = 0; i < m; i++) tail = tail->succ; // 找到第二个区间的尾后位置
//
//     ListNodePosi(T) p1 = p;
//     ListNodePosi(T) p2 = q;
//     ListNodePosi(T) cur = head;
//
//     while (n > 0 && m > 0) {
//         if (p1->data <= p2->data) {
//             cur->succ = p1;
//             p1->pred = cur;
//             p1 = p1->succ;
//             n--;
//         } else {
//             cur->succ = p2;
//             p2->pred = cur;
//             p2 = p2->succ;
//             m--;
//         }
//         cur = cur->succ;
//     }
//
//     // 处理剩余部分
//     if (n > 0) {
//         cur->succ = p1;
//         p1->pred = cur;
//         // 不需要更新尾部，因为p1后面的连接已经正确
//     } else if (m > 0) {
//         cur->succ = p2;
//         p2->pred = cur;
//         // 将tail连接到链表末尾
//         ListNodePosi(T) last = p2;
//         for (int i = 1; i < m; i++) last = last->succ;
//         last->succ = tail;
//         tail->pred = last;
//     } else {
//         cur->succ = tail;
//         tail->pred = cur;
//     }
//
//     // 更新p为新区间的起始
//     p = head->succ;
// }

template<typename T>
int List<T>::disordered() const {
    if (_size < 2) return 0; // 少于2个元素，肯定有序

    int count = 0;
    ListNodePosi(T) p = header->succ;
    while (p != trailer && p->succ != trailer) {
        if (p->data > p->succ->data) {
            count++; // 逆序对计数
        }
        p = p->succ;
    }
    return count; // 返回逆序对总数
}

template<typename T>
void List<T>::reverse() {
    if (_size < 2) return;
    // 遍历所有节点（不包括哨兵），交换每个节点的前驱和后继
    ListNodePosi(T) p = header->succ;
    while (p != trailer) {
        // 交换当前节点的前驱和后继
        ListNodePosi(T) temp = p->succ;
        p->succ = p->pred;
        p->pred = temp;
        p = temp;
    }
    // 处理哨兵节点的连接
    ListNodePosi(T) first = header->succ;
    ListNodePosi(T) last = trailer->pred;
    // 将原第一个节点连接到header
    header->succ = last;
    last->pred = header;
    // 将原最后一个节点连接到trailer
    trailer->pred = first;
    first->succ = trailer;
}

// template<typename T>
// void List<T>::reverse() {
//     if (_size < 2) return;
//     // 通过交换数据来实现反转（最简单的方法）
//     ListNodePosi(T) left = header->succ;
//     ListNodePosi(T) right = trailer->pred;
//     for (int i = 0; i < _size / 2; i++) {
//         swap(left->data, right->data);
//         left = left->succ;
//         right = right->pred;
//     }
// }

// 5. traverse函数（函数指针版本）
template<typename T>
void List<T>::traverse(void (*visit)(T&)) {
    for (ListNodePosi(T) p = header->succ; p != trailer; p = p->succ) {
        visit(p->data);
    }
}

// 6. traverse函数（函数对象版本）
template<typename T>
template<typename VST>
void List<T>::traverse(VST& visit) {
    for (ListNodePosi(T) p = header->succ; p != trailer; p = p->succ) {
        visit(p->data);
    }
}

int main() {
    // 测试代码
    SetConsoleOutputCP(65001);
    List<int> list;

    // 插入一些元素
    list.insertAsLast(5);
    list.insertAsLast(2);
    list.insertAsLast(8);
    list.insertAsLast(1);
    list.insertAsLast(4);

    cout << "原始列表: ";
    list.traverse([](int& val) { cout << val << " "; });
    cout << endl;

    // 测试是否有序
    cout << "逆序对数量: " << list.disordered() << endl;

    // 测试排序
    list.sort();
    cout << "排序后: ";
    list.traverse([](int& val) { cout << val << " "; });
    cout << endl;

    // 测试反转
    list.reverse();
    cout << "反转后: ";
    list.traverse([](int& val) { cout << val << " "; });
    cout << endl;

    // 测试去重
    list.insertAsLast(2);
    list.insertAsLast(2);
    list.uniquify();
    cout << "去重后: ";
    list.traverse([](int& val) { cout << val << " "; });
    cout << endl;

    return 0;
}