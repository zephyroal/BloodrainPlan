/*
���TMyClass::Func���ջ�ת���� void Func(TMyClass *this);
Ҳ����˵��ԭ��һ������ǰ����ָ��������thisָ�롣
���ǿ��������������дһ���Ǿ�̬���Ա������ֱ����Ϊ�̻߳ص�������
�ȿ�_beginthread�����Ķ���:
unsigned long _RTLENTRY _EXPFUNC _beginthread (void (_USERENTRY *__start)(void *),unsigned __stksize, void *__arg);
���еĵ�һ������������Ϊ�߳�ִ������Ļص�������
����ԭ����:void Func(void *)�����void*��������Ϊ�Զ������ݴ���ġ�
�Ա�һ��������˵��TMyClass::Func��������ʽ�������ÿ��Է��������Ҫ��
��������ʵ��:*/
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
    void _USERENTRY ThreadProc()                // ���Ա����
    {
        for (int i = 0; i < m_nCount; i++)          // ����m_nCount��Ա��ӡһ������
        {
            printf("Class%d : %d/n", m_nId, i);
        }
    }
};

int main(int argc, char* argv[])
{
    union
    {                                    // �����࣬����ת�����Ա����ָ�뵽��ͨ����ָ�루�Թ��������������������ֺ���֮��ǿ��ת��������֪����û�и��õķ�����
        void (_USERENTRY* ThreadProc)(void*);
        void(_USERENTRY TMyClass::* MemberProc) ();
    } Proc;                                    // ��������������ֺ����������ڿ������кܴ�ͬ�������ǵ�������ʽ����ͬ�ġ�

    TMyClass MyClass1(1, 10), MyClass2(2, 5);  // ��������TMyClass����

    Proc.MemberProc = &TMyClass::ThreadProc;       // ת����Proc.ThreadProc���Ƕ�Ӧ����ͨ����ָ����

    _beginthread(Proc.ThreadProc, 4096, &MyClass1);     // ��ʼ�̣߳������Proc.ThreadProcʵ������TMyClass::ThreadProc, ��Ҫ��thisָ�������Ǹ���&MyClass1��
    _beginthread(Proc.ThreadProc, 4096, &MyClass2);
    system("pause");
    return 0;
}
/*
���У�����ɣ�:-)
��ʵ��ֹ�̻߳ص���������ʵֻҪ������Func(void*,...)�Ļص�����
�����������ַ���ֱ��ʹ�����Ա������(ǰ���ǵ�һ��void*���Զ������ݣ�Ҳ����˵����������������)��*/
