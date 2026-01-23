//
// Created by 34120 on 2026/1/23.
//

#include <vector>
#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>

template <typename T>
class Stack:public std::vector<T> {
public:
    void push(T const &data) {
        this->push_back(data);
    }
    T pop() {
        if (this->empty()) {
            throw std::out_of_range("Stack<>::pop(): empty stack");
        }
        T top=this->back();
        this->pop_back();
        return top;
    }
    T& top() {
        if (this->empty()) {
            throw std::out_of_range("Stack<>::pop(): empty stack");
        }
        return this->back();
    }
};

template <typename T>
class Queue:public std::vector<T> {
    public:
    void enqueue(T const &data) {
        this->push_back(data);
    }
    T dequeue() {
        if (this->empty()) {
            throw std::out_of_range("Stack<>::pop(): empty stack");
        }
        T first=this->front();
        this->pop_front();
        return first;
    }
    T & front() {
        return this->first();
    }
};

//进制转换
void convert(Stack<char>& S,int n,int base) {
    static char digit[]={0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F'};
    while (n>0) {
        S.push(digit[n%base]);
        n/=base;
    }
}

//递归
void convert2(Stack<char>& S,int n,int base) {
    static char digit[]={0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F'};
    if (n>0) {
        convert2(S,n/base,base);
        S.push(digit[n%base]);
    }
}


//括号匹配
// 括号匹配核心函数 - 使用switch-case
bool isParenthesesBalanced(const std::string& e) {
    Stack<char> S;

    for (int i=0;e[i];i++) {
        switch (e[i]) {
            // 左括号入栈
            case '(':
            case '[':
            case '{':
                S.push(e[i]);
                break;
                // 右括号检查匹配
            case ')':
                if (S.empty() || S.pop() != '(') return false;
                break;
            case ']':
                if (S.empty() || S.pop() != '[') return false;
                break;
            case '}':
                if (S.empty() || S.pop() != '{') return false;
                break;
                // 其他字符忽略
            default:
                break;
        }
    }
    return S.empty();
}


// 获取运算符优先级
int getPrecedence(char op) {
    switch (op) {
        case '^':           // 幂运算
            return 6;
        case '*':
        case '/':
        case '%':
            return 5;
        case '+':
        case '-':
            return 4;
            // 如果需要支持更多运算符，可以继续添加
        default:
            return 0;       // 其他情况
    }
}

// 比较两个运算符的优先级
bool hasHigherPrecedence(char op1, char op2) {
    int prec1 = getPrecedence(op1);
    int prec2 = getPrecedence(op2);

    // 如果优先级相同，左结合性
    if (prec1 == prec2) {
        // 对于左结合的运算符，左侧的先计算
        return false;  // 或者根据具体需求调整
    }
    return prec1 > prec2;
}

// 判断是否是运算符
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

// 执行运算
int applyOperation(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) throw std::runtime_error("除零错误");
            return a / b;
        case '%':
            if (b == 0) throw std::runtime_error("取模零错误");
            return a % b;
        case '^': return static_cast<int>(pow(a, b));
        default: return 0;
    }
}

// 中缀表达式求值主函数
int evaluateInfix(const std::string& expression) {
    std::stack<int> values;
    std::stack<char> ops;

    for (size_t i = 0; i < expression.length(); i++) {
        char c = expression[i];

        // 跳过空格
        if (c == ' ') {
            continue;
        }
        // 如果是数字
        else if (isdigit(c)) {
            int num = 0;
            while (i < expression.length() && isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            i--;  // 回退一个字符
            values.push(num);
        }
        // 如果是左括号
        else if (c == '(') {
            ops.push(c);
        }
        // 如果是右括号
        else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperation(val1, val2, op));
            }
            if (!ops.empty()) {
                ops.pop();  // 弹出左括号
            }
        }
        // 如果是运算符
        else if (isOperator(c)) {
            while (!ops.empty() && getPrecedence(ops.top()) >= getPrecedence(c)) {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperation(val1, val2, op));
            }
            ops.push(c);
        }
    }

    // 处理剩余的运算符
    while (!ops.empty()) {
        int val2 = values.top(); values.pop();
        int val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperation(val1, val2, op));
    }

    return values.top();
}


int main() {
    Stack<char> S;
    convert(S,10,16);
    while (!S.empty()) {
        std::cout<<S.pop();
    }
}