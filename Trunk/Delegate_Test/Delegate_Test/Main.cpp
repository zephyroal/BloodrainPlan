#ifndef __DELEGATE_TEST_H__
#define __DELEGATE_TEST_H__
// lrhnfs@gmail.com [2011/2/21 YMD] [11:35:36]
// Delegate(委托)测试
#include "delegate.h"
#include <iostream>
using namespace std;
using namespace delegate;
// 按钮测试类
class ButtonTest
{
public:
    // 调用委托
    void Run()
    {
        int testNum = 3;

        m_delegateC(&testNum);
        m_delegateF(&testNum);
        m_delegateFNoArg();
    }
    CDelegate1<int*> m_delegateC;
    CDelegate1<int*> m_delegateF;
    CDelegate0       m_delegateFNoArg;
};

// function委托的测试函数
void DelegateFunctionTest(int* pT)
{
    cout << "DelegateFunctionTest OK! Arg = " << *pT << endl;
}
// class委托的测试函数
class DelegateClassTest
{
public:
    void _DelegateClassTest(int* pT)
    {
        cout << "DelegateObjectTest OK! Arg = " << *pT << endl;
    }
};
void TestNoArg()
{
    cout << "No arg Functon" << endl;
}
// 测试入口
class DelegateTest
{
public:
    static void Test()
    {
        DelegateClassTest delegateObjectTest;
        ButtonTest        buttonTest;
        buttonTest.m_delegateC      = newDelegate(&delegateObjectTest, &DelegateClassTest::_DelegateClassTest);
        buttonTest.m_delegateF      = newDelegate(DelegateFunctionTest);
        buttonTest.m_delegateFNoArg = newDelegate(TestNoArg);
        buttonTest.Run();
    }
};
#endif // __DELEGATE_TEST_H__
