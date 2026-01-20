#include<iostream>

#include <windows.h>


using namespace std;

typedef  int Rank;//秩
#define DEFAULT_CAPACITY 10//默认初始容量

// 简单的线性同余生成器
class SimpleRand {
private:
    unsigned int seed;
    // 常见的LCG参数
    static constexpr unsigned int a = 1103515245;
    static constexpr unsigned int c = 12345;
    static constexpr unsigned int m = 2147483648; // 2^31

public:
    explicit SimpleRand(unsigned int s = 0) : seed(s ? s : 1) {}
    void srand(unsigned int s) { seed = s ? s : 1; }
    int rand() {
        seed = (a * seed + c) % m;
        return static_cast<int>(seed);
    }
    int rand_range(int min, int max) {
        return min + rand() % (max - min + 1);
    }
};

// 全局实例
static SimpleRand my_rand(static_cast<unsigned>(time(nullptr)));

class Fib {
private:
    int f, g;
public:
    explicit Fib(int n) : f(1), g(0) {
        while (g < n) {
            next();
        }
    }
    [[nodiscard]] int get() const { return g; }
    int next() {
        g += f;
        f = g - f;
        return g;
    }
    int prev() {
        f = g - f;
        g -= f;
        return g;
    }
};


template <typename T>class Vector {//向量模板类
protected:
    Rank _size{}; int _capacity{}; T* _elem;//规模，容量，数据区
    void copyFrom(T const* A,Rank lo,Rank hi);//复制数组区间A[lo,hi)
    void expand();//空间不足时扩容
    void shrink();//装填因子过小时压缩
    bool bubble(Rank lo,Rank hi);//扫描交换
    void bubbleSort(Rank lo,Rank hi);//起泡排序算法
    Rank max(Rank lo,Rank hi);//选取最大元素
    void selectionSort(Rank lo,Rank hi);//选择排序算法
    void merge(Rank lo,Rank mi,Rank hi);//归并算法
    void mergeSort(Rank lo,Rank hi);//归并排序算法
    Rank partition(Rank lo,Rank hi);//轴点构造算法
    void quickSort(Rank lo,Rank hi);//快速排序算法
    void heapSort(Rank lo,Rank hi);//堆排序

public:
    //构造函数
    explicit Vector(int c=DEFAULT_CAPACITY,int s=0,T v=0) {//容量为c，规模为s，所有元素初始化为v
        _capacity =c;
        _elem=new T[_capacity];
        for (_size=0;_size<s;_elem[_size++]=v) ;
    }//s<=c
    Vector(T const* A,Rank lo,Rank hi) {
        copyFrom(A,lo,hi);
    }//数组区间复制
    Vector(T const* A,Rank n) {
        copyFrom(A,0,n);
    }//数组整体复制
    Vector(Vector<T> const& V,Rank lo,Rank hi) {
        copyFrom(V._elem,lo,hi);
    }//向量区间复制
    Vector(Vector<T> const& V) {
        copyFrom(V._elem,0,V._size);
    }//向量整体复制
    //析构函数
    ~Vector() {
        delete [] _elem;//释放内存
    }
    //只读访问接口
    [[nodiscard]] Rank size()const {
        return _size;
    }//规模
    [[nodiscard]] bool empty()const {
        return _size==0;
    }//判空
    [[nodiscard]] int disordered()const;//判断向量是否已排序
    [[nodiscard]] Rank find(T const& e)const {
        return find(e,0,_size);
    }//无序向量整体查找
    Rank find(T const& e,Rank lo,Rank hi) const;//无序向量区间查找
    [[nodiscard]] Rank search(T const& e) const {
        return (0>=_size)?-1:search(e,0,_size);
    }//有序向量整体查找
    Rank search(T const& e,Rank lo,Rank hi) const;//有序向量区间查找
    //可写访问接口
    T &operator[](Rank r) const;//重载下标操作符，可以类似数组形式引用各函数
    Vector<T>& operator=(const Vector<T>& other);//重载赋值运算符，便于克隆向量
    void swap(T& a, T& b) noexcept;
    T remove(Rank r);//删除秩为r的元素
    int remove(Rank lo,Rank hi);//删除秩在[lo,hi)之内的元素
    Rank insert(Rank r,T const& e);//插入元素
    Rank insert(T const& e) {
        return insert(_size,e);
    }//默认作为末元素插入
    void sort(Rank lo,Rank hi);//对[lo,hi)排序
    void sort() {
        sort(0,_size);
    }//整体排序
    void unsort(Rank lo,Rank hi);//对[lo,hi)置乱
    void unsort() {
        unsort(0,_size);
    }//整体置乱
    int deduplicate();//无序去重
    int uniquify();//有序去重
    //遍历
    void traverse(void (*)(T&));//遍历（使用函数指针，只读或局部性修改）
    template <typename VST> void traverse(VST&);//遍历（使用函数对象，可全局性修改）

};//Vector

template<typename T>
Rank Vector<T>::max(Rank lo, Rank hi) {
    Rank mx = hi;
    while (lo < hi--) {
        if (_elem[hi] > _elem[mx]) mx = hi;
    }
    return mx;
}

template<typename T>//T为基本类型，或已重载赋值操作符‘=’
void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi) {
    _elem=new T[_capacity=2*(hi-lo)];//分配空间
    _size=0;//规模清零
    while (lo<hi) {//A[lo,hi)
        _elem[_size++]=A[lo++];//复制到_elem[0,hi-lo)
    }
}

template<typename T>
void Vector<T>::expand() {//向量空间不足时扩容
    if (_size<_capacity)
        return;//未满员
    // if (_capacity<DEFAULT_CAPACITY)
    //     _capacity=DEFAULT_CAPACITY;//  错的，没有申请后面的内存
    T* oldElem=_elem;//指向原来的旧地址
    int newCapacity=(_capacity<DEFAULT_CAPACITY)?DEFAULT_CAPACITY:_capacity<<1;
    _elem=new T[newCapacity];//容量翻倍
    for (int i=0;i<_size;i++) {
        _elem[i]=oldElem[i];
    }
    delete [] oldElem;//释放原空间
    _capacity=newCapacity;
}

template<typename T>
void Vector<T>::shrink() {//缩容
    if (_size>_capacity>>2||(_capacity>>1)<DEFAULT_CAPACITY) {
        return;//  >=1/4或者容量减半会小于等于最低容量 退出
    }
    int newCapacity=_capacity>>1;
    if (newCapacity<_size) {
        newCapacity=_size;//如果缩容之后 size大于_newcapacity
    }
    if (_capacity-newCapacity<4) {
        return;
    }
    T* oldElem=_elem;//指向原来的旧地址
    _elem=new T[newCapacity];//容量缩小
    for (int i=0;i<_size;i++) {
        _elem[i]=oldElem[i];
    }
    delete [] oldElem;//释放原空间
    _capacity=newCapacity;//更新容量
}

template<typename T>
T &Vector<T>::operator[](Rank r) const {
    return _elem[r];    //v[r]  ->  v.elem[r]
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)  {
    if (this==&other) {
        return *this;
    }//防止自我赋值
    T* newElem=nullptr;//先分配新内存
    if (other._capacity>0) {
        newElem=new T[other._capacity];
        for (int i=0;i<other._size;i++) {
            newElem[i]=other._elem[i];
        }//复制元素
    }
    if (_elem!=nullptr) {
        delete [] _elem;
    }//释放旧内存
    //更新指针和状态
    _elem=newElem;
    _size=other._size;
    _capacity=other._capacity;

    return *this;
}

template<typename T>
void Vector<T>::swap(T& a, T& b) noexcept {
    T temp = a;
    a = b;
    b = temp;
}

template<typename T>
void Vector<T>::unsort(Rank lo, Rank hi) {
    T* V = _elem + lo;  // 子向量_elem[lo, hi)
    for (Rank i = hi - lo; i > 0; i--) {
        swap(V[i-1], V[my_rand.rand() % i]);  // 随机交换
    }
}

template<typename T>//e作为秩为r元素插入, 0<=r<=size
Rank Vector<T>::insert(Rank r,T const & e) {//o(n-r)
    expand();//若有必要，扩容
    for (int i=_size;i>r;i--) {
        _elem[i]=_elem[i-1];    //自后向前往后挪一个
    }
    _elem[r]=e;     //置入新元素
    _size++;        //更新容量
    return r;       //返回秩
}

template<typename T>
int Vector<T>::remove(Rank lo, Rank hi) {//删除区间[lo,hi)
    if (lo==hi) return 0;//出于效率，单独处理退化情况 remove(0,0)
    while (hi<_size) {
        _elem[lo++]=_elem[hi++];
    }//[lo,hi)顺次前移hi-lo个单元
    _size=lo;//更新规模，直接丢弃尾部[lo,_size=hi)区间
    shrink();//若有必要，缩容
    return hi-lo;//返回被删除元素数目
}

template<typename T>//删除向量中秩为r的元素，0<=r<size
T Vector<T>::remove(Rank r) {//o(n-r)
    T e=_elem[r];//备份被删除元素
    remove(r,r+1);//调用区间删除算法
    return e;//返回被删除元素
}

template<typename T>
Rank Vector<T>::find(T const & e,Rank lo,Rank hi)const {
    //o(hi-lo)=o(n),在命中多个元素是返回秩最大者
    while ((lo<hi)&& (e!=_elem[hi])) hi--;//逆向查找
    return hi;//hi<lo意味着失败
}//输入敏感

template<typename T>//删除重复元素，返回被删元素数目
int Vector<T>::deduplicate() {
    int oldSize=_size;//记录原来规模
    Rank i=1;//从_elem[1]开始
    while (i<_size) {
        (find(_elem[i],0,i)<0)?//在前缀找相同
        i++//若没有则继续往后
        :remove(i);//有了就删(至多一个)
        }
    return oldSize-_size;//向量规模变化量，删除元素总数
}//find和remove 都是o(n)则为o(n2)

template<typename T>
void Vector<T>::traverse(void(*visit)(T&)) {//函数指针遍历
    for (int i=0;i<_size;i++) {
        visit(_elem[i]);
    }
}

template<typename T> template <typename VST>//元素类型，操作器
void Vector<T>::traverse(VST& visit) {//函数对象机制遍历
    for (int i=0;i<_size;i++) {
        visit(_elem[i]);
    }
}

template<typename T>//返回逆序相邻元素对总数
int Vector<T>::disordered() const{
    int n=0;//计数器
    for (int i=1;i<_size;i++) {//逐一检查各对相邻元素
        n+=(_elem[i-1]>_elem[i]);//逆序则计数
    }
    return n;//向量有序 == n = 0
}//若判断是否有序，遇到可以直接停止

// template<typename T>//有序向量中删除重复
// int Vector<T>::uniquify() {
//     int oldSize=_size;
//     int i=0;//从首元素开始
//     while (i<_size-1) {//从前向后逐一对比相邻
//         //若雷同，则删除后者，不然就继续向后
//         (_elem[i]==_elem[i+1])?remove(i+1):remove(i);i++;
//         //删除时都往前挪了一个
//     }
//     return oldSize-_size;//返回删除总数
// }//_size减小由remove完成
// //o(n2) 最坏每次都要用remove()  低效算法
// //原因  同一元素可作为被删除元素的后继多次前移
// //      若能以重复区间为单位，成批删除

template<typename T>
int Vector<T>::uniquify() {
    Rank i=0,j=0;//各对互异“相邻”元素秩
    while (j++<_size) {//逐一扫描，直至末尾元素
        //跳过雷同，发现不同，就移到紧邻i的右侧
        if (_elem[i]!=_elem[j]) {
            _elem[++i]=_elem[j];
        }
    }
    _size=++i;shrink();//截去尾部多余元素
    return j-i;//被删除元素总数
}//n-1次   o(n)
//通过remove(lo,hi)批量删除，依然不能高效率
//每次删除都会往前移

//二分查找a
// template<typename T>
// static Rank binSearch(T* A,T const& e,Rank lo,Rank hi) {
//     while (lo<hi) {//每步迭代可能两次判断，有三个分支
//         Rank mi=(lo+hi)>>1;//以中点为轴点
//         if (e<A[mi])   hi=mi;//深入前半段[mi,hi)
//         else if (e>A[mi])  lo=mi+1;//后半段(mi,hi)
//         else return mi;//在mi命中
//     }//成功查找就提前终止
//     return -1;//查找失败
// }//o(1.5*logn)

//fib查找
template<typename T>
static Rank fibSearch(T*A,T const & e,Rank lo,Rank hi) {
    Fib fib(hi-lo);//
    while (lo<hi) {
        while (hi-lo<fib.get()) fib.prev();
        Rank mi=lo+fib.get()-1;
        if (e<A[mi]) hi=mi;
        else if (e>A[mi])  lo=mi+1;
        else return mi;
    }
    return -1;
}



// //二分查找b
// template<typename T>
// static Rank binSearch(T* A,T const& e,Rank lo,Rank hi) {
//     while (1<hi-lo) {//每次迭代只做一次判断,两个分支
//         Rank mi=(lo+hi)>>1;
//         (e<A[mi])?hi=mi:lo=mi;//经比较选择深入[lo,mi)[mi,hi)
//     }出口时hi=lo+1，区间仅含A[lo]
//     return (e==A[lo])?lo:-1;//成功返回对应秩，不然就-1,不能指示失败位置
// }

//二分查找c
template<typename T>
static Rank binSearch(T* A,T const& e,Rank lo,Rank hi) {
    while (lo<hi) {//每次迭代只做一次判断,两个分支
        Rank mi=(lo+hi)>>1;//中点为轴点
        (e<A[mi])?hi=mi:lo=mi+1;//经比较选择深入[lo,mi)[mi,hi)
    }
    return --lo;//lo为大于e元素是最小秩，lo-1
}//多元素命中，返回秩最大值
//缩短至0才结束

template<typename T>//在有序向量区间[lo,hi) 确定不大于e最后一个节点的秩
Rank Vector<T>::search(T const &e, Rank lo, Rank hi) const {
    return (my_rand.rand() % 2)?  //按50%
        binSearch(_elem,e,lo,hi):fibSearch(_elem,e,lo,hi);
}//二分查找或fibonacci查找

//起泡排序
template<typename T>
void Vector<T>::sort(Rank lo,Rank hi) {
    switch (my_rand.rand()%5) {
        case 1:bubbleSort(lo,hi); break;
        case 2:selectionSort(lo,hi); break;
        case 3:mergeSort(lo,hi); break;
        case 4:heapSort(lo,hi); break;
        default: quickSort(lo,hi); break;
    }
}

//冒泡排序a
// template<typename T>
// void Vector<T>::bubbleSort(Rank lo, Rank hi) {
//     while (!bubble(lo,hi));//逐趟扫描交换
// }
//
// template<typename T>
// bool Vector<T>::bubble(Rank lo,Rank hi) {
//     bool sorted=true;//整体有序
//     while (++lo<hi) {//自左向右，逐一检查各对相邻元素
//         if (_elem[lo-1]>_elem[lo]) {//若逆序
//             sorted=false;//无序标志
//             swap(_elem[lo-1],_elem[lo]);//交换
//         }
//     }
//     return sorted;//返回有序标志
// }
////有问题 后面随着冒泡过去每次都要检查，无效检查

//冒泡b
template<typename T>
void Vector<T>::bubbleSort(Rank lo, Rank hi) {
    while (lo<(hi=bubble(lo,hi)));//逐趟扫描交换，直至全序
}

template<typename T>
bool Vector<T>::bubble(Rank lo,Rank hi) {
    Rank last=lo;//最右侧逆序对初始化[lo-1,lo]
    while (++lo<hi) {//自左向右，逐一检查各对相邻元素
        if (_elem[lo-1]<_elem[lo]) {//若逆序
            last=lo;//更新最右侧逆序对位置
            swap(_elem[lo-1],_elem[lo]);//交换
        }
    }
    return last;//返回最右侧逆序对
}



template<typename T>
void Vector<T>::selectionSort(Rank lo, Rank hi) {
    while (lo < hi--) {
        Rank mx = max(lo, hi);
        swap(_elem[mx], _elem[hi]);
    }
}

template<typename T>
void Vector<T>::quickSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    Rank mi = partition(lo, hi - 1);
    quickSort(lo, mi);
    quickSort(mi + 1, hi);
}

template<typename T>
void Vector<T>::merge(Rank lo, Rank mi,Rank hi) {
    T* A=_elem+lo;
    int lb=mi-lo;T*B=new T[lb];
    for (Rank i=0;i<lb;B[i]=A[i++]) {}
    int lc=hi-mi;T* C=_elem+mi;
    for (Rank i=0,j=0,k=0;(j<lb)||(k<lc);) {
        if ((j<lb)&&((lc<=k)||(B[j]<=C[k])))  A[i++]=B[j++];
        if ((k<lc)&&((lb<=j)||(C[j]<B[k])))   A[i++]=C[k++];
    }
    delete[] B;
}

template<typename T>
void Vector<T>::mergeSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    Rank mi = (lo + hi) >> 1;
    mergeSort(lo, mi);
    mergeSort(mi, hi);
    merge(lo, mi, hi);
}

template<typename T>
Rank Vector<T>::partition(Rank lo, Rank hi) {
    swap(_elem[lo], _elem[lo + my_rand.rand() % (hi - lo + 1)]);
    T pivot = _elem[lo];
    while (lo < hi) {
        while (lo < hi && pivot <= _elem[hi]) hi--;
        _elem[lo] = _elem[hi];
        while (lo < hi && _elem[lo] <= pivot) lo++;
        _elem[hi] = _elem[lo];
    }
    _elem[lo] = pivot;
    return lo;
}

template<typename T>
void Vector<T>::heapSort(Rank lo, Rank hi) {
    // 堆排序实现
    // 由于篇幅限制，这里使用简单的建堆排序
    // 实际实现应使用Floyd建堆算法
    for (Rank i = (hi - 2) / 2; i >= lo; i--) {
        Rank p = i;
        T temp = _elem[p];
        Rank child = 2 * p + 1;
        while (child < hi) {
            if (child + 1 < hi && _elem[child] < _elem[child + 1]) {
                child++;
            }
            if (temp >= _elem[child]) break;
            _elem[p] = _elem[child];
            p = child;
            child = 2 * p + 1;
        }
        _elem[p] = temp;
    }

    for (Rank i = hi - 1; i > lo; i--) {
        swap(_elem[lo], _elem[i]);
        Rank p = lo;
        T temp = _elem[p];
        Rank child = 2 * p + 1;
        while (child < i) {
            if (child + 1 < i && _elem[child] < _elem[child + 1]) {
                child++;
            }
            if (temp >= _elem[child]) break;
            _elem[p] = _elem[child];
            p = child;
            child = 2 * p + 1;
        }
        _elem[p] = temp;
    }
}



int main(){
    SetConsoleOutputCP(65001);
     // 测试1: 基本功能
    cout << "=== 测试1: 基本功能 ===" << endl;
    Vector<int> v1;
    for (int i = 0; i < 10; i++) {
        v1.insert(i);
    }
    cout << "Size: " << v1.size() << endl;
    for (int i = 0; i < v1.size(); i++) {
        cout << v1[i] << " ";
    }
    cout << endl;

    // 测试2: 排序
    cout << "\n=== 测试2: 排序 ===" << endl;
    Vector<int> v2;
    v2.insert(5); v2.insert(2); v2.insert(8); v2.insert(1); v2.insert(9);
    cout << "排序前: ";
    for (int i = 0; i < v2.size(); i++) {
        cout << v2[i] << " ";
    }
    cout << endl;

    v2.sort();
    cout << "排序后: ";
    for (int i = 0; i < v2.size(); i++) {
        cout << v2[i] << " ";
    }
    cout << endl;

    // 测试3: 去重
    cout << "\n=== 测试3: 去重 ===" << endl;
    Vector<int> v3;
    v3.insert(1); v3.insert(2); v3.insert(2); v3.insert(3); v3.insert(3); v3.insert(3);
    cout << "去重前: ";
    for (int i = 0; i < v3.size(); i++) {
        cout << v3[i] << " ";
    }
    cout << ", Size: " << v3.size() << endl;

    v3.deduplicate();
    cout << "去重后: ";
    for (int i = 0; i < v3.size(); i++) {
        cout << v3[i] << " ";
    }
    cout << ", Size: " << v3.size() << endl;

    // 测试4: 查找
    cout << "\n=== 测试4: 查找 ===" << endl;
    Vector<int> v4;
    for (int i = 0; i < 10; i++) {
        v4.insert(i * 2);  // 插入偶数
    }
    v4.sort();  // 确保有序
    Rank idx = v4.search(6);
    cout << "查找6的位置: " << idx << endl;
    idx = v4.search(5);
    cout << "查找5的位置: " << idx << endl;

    // 测试5: 删除
    cout << "\n=== 测试5: 删除 ===" << endl;
    Vector<int> v5;
    for (int i = 0; i < 10; i++) {
        v5.insert(i);
    }
    cout << "删除前: ";
    for (int i = 0; i < v5.size(); i++) {
        cout << v5[i] << " ";
    }
    cout << ", Size: " << v5.size() << endl;

    v5.remove(3, 7);
    cout << "删除[3,7)后: ";
    for (int i = 0; i < v5.size(); i++) {
        cout << v5[i] << " ";
    }
    cout << ", Size: " << v5.size() << endl;

    // 测试6: 无序化
    cout << "\n=== 测试6: 无序化 ===" << endl;
    Vector<int> v6;
    for (int i = 0; i < 10; i++) {
        v6.insert(i);
    }
    cout << "无序化前: ";
    for (int i = 0; i < v6.size(); i++) {
        cout << v6[i] << " ";
    }
    cout << endl;

    v6.unsort();
    cout << "无序化后: ";
    for (int i = 0; i < v6.size(); i++) {
        cout << v6[i] << " ";
    }
    cout << endl;
    return 0;
}