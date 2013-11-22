/*
这个TMyClass::Func最终会转化成 void Func(TMyClass *this);
也就是说在原第一个参数前插入指向对象本身的this指针。
我们可以利用这个特性写一个非静态类成员方法来直接作为线程回调函数，
先看_beginthread函数的定义:
unsigned long _RTLENTRY _EXPFUNC _beginthread (void (_USERENTRY *__start)(void *),unsigned __stksize, void *__arg);
其中的第一个参数就是作为线程执行主体的回调函数。
它的原型是:void Func(void *)，这个void*参数是作为自定义数据传入的。
对比一下上面所说的TMyClass::Func的最终形式，它正好可以符合这里的要求。
现在做个实验:*/
#include "stdafx.h"
#include <stdio.h>
#include <process.h>
class TMyClass
{
    int m_nCount;
    int m_nId;
public:
    TMyClass(int nId, int nCount)
        : m_nId(nId), m_nCount(nCount)
    {
    }
    void _USERENTRY ThreadProc()                // 类成员方法
    {
        for (int i = 0; i < m_nCount; i++)          // 根据m_nCount成员打印一排数字
        {
            printf("Class%d : %d/n", m_nId, i);
        }
    }
};

int main(int argc, char* argv[])
{
    union
    {                                    // 联合类，用于转换类成员方法指针到普通函数指针（试过编译器不允许在这两种函数之间强制转换），不知道有没有更好的方法。
        void (_USERENTRY* ThreadProc)(void*);
        void(_USERENTRY TMyClass::* MemberProc) ();
    } Proc;                                    // 尽管联合里的两种函数类型现在看起来有很大不同，但它们的最终形式是相同的。

    TMyClass MyClass1(1, 10), MyClass2(2, 5);  // 产生两个TMyClass对象

    Proc.MemberProc = &TMyClass::ThreadProc;       // 转换，Proc.ThreadProc就是对应的普通函数指针了

    _beginthread(Proc.ThreadProc, 4096, &MyClass1);     // 开始线程，这里的Proc.ThreadProc实际上是TMyClass::ThreadProc, 它要的this指针是我们给的&MyClass1。
    _beginthread(Proc.ThreadProc, 4096, &MyClass2);
    system("pause");
    return 0;
}
/*
运行！神奇吧？:-)
其实不止线程回调函数，其实只要是形如Func(void*,...)的回调函数
都可以用这种方法直接使用类成员方法。(前提是第一个void*是自定义数据，也就是说它不能有其它功能)。*/
