﻿ // lrhnfs@gmail.com [2011/2/22 YMD] [12:58:39]
// 代码修改自MyGUI
namespace delegate
{
// 定义链接字符串
#define MYGUI_COMBINE(a, b)         MYGUI_COMBINE1(a, b)
#define MYGUI_COMBINE1(a, b)        a ## b

#define MYGUI_I_DELEGATE        MYGUI_COMBINE(IDelegate, MYGUI_SUFFIX)

#define MYGUI_C_STATIC_DELEGATE         MYGUI_COMBINE(CStaticDelegate, MYGUI_SUFFIX)
#define MYGUI_C_METHOD_DELEGATE         MYGUI_COMBINE(CMethodDelegate, MYGUI_SUFFIX)

#define MYGUI_C_DELEGATE        MYGUI_COMBINE(CDelegate, MYGUI_SUFFIX)
#define MYGUI_C_MULTI_DELEGATE      MYGUI_COMBINE(CMultiDelegate, MYGUI_SUFFIX)

// 委托基类
MYGUI_TEMPLATE MYGUI_TEMPLATE_PARAMS
class MYGUI_I_DELEGATE
{
public:
    virtual ~MYGUI_I_DELEGATE()
    {
    }
    virtual bool isType(const std::type_info& _type) = 0;
    virtual void invoke(MYGUI_PARAMS) = 0;
    virtual bool compare(MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS* _delegate) const = 0;
};

// 静态方法委托类
MYGUI_TEMPLATE MYGUI_TEMPLATE_PARAMS
class MYGUI_C_STATIC_DELEGATE
    : public MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS
{
public:
    typedef void (* Func)(MYGUI_PARAMS);

    MYGUI_C_STATIC_DELEGATE (Func _func) : mFunc(_func)
    {
    }

    virtual bool isType(const std::type_info& _type)
    {
        return typeid(MYGUI_C_STATIC_DELEGATE MYGUI_TEMPLATE_ARGS) == _type;
    }

    virtual void invoke(MYGUI_PARAMS)
    {
        mFunc(MYGUI_ARGS);
    }

    virtual bool compare(MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS* _delegate) const
    {
        if (0 == _delegate || !_delegate->isType(typeid(MYGUI_C_STATIC_DELEGATE MYGUI_TEMPLATE_ARGS)))
        {
            return false;
        }
        MYGUI_C_STATIC_DELEGATE MYGUI_TEMPLATE_ARGS* cast = static_cast<MYGUI_C_STATIC_DELEGATE MYGUI_TEMPLATE_ARGS*>(_delegate);
        return cast->mFunc == mFunc;
    }

private:
    Func mFunc;
};

// 类方法委托类
template MYGUI_T_TEMPLATE_PARAMS
class MYGUI_C_METHOD_DELEGATE
    : public MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS
{
public:
    typedef void (T::* Method)(MYGUI_PARAMS);

    MYGUI_C_METHOD_DELEGATE(T* _object, Method _method) : mObject(_object), mMethod(_method)
    {
    }

    virtual bool isType(const std::type_info& _type)
    {
        return typeid(MYGUI_C_METHOD_DELEGATE MYGUI_T_TEMPLATE_ARGS) == _type;
    }

    virtual void invoke(MYGUI_PARAMS)
    {
        (mObject->*mMethod)(MYGUI_ARGS);
    }

    virtual bool compare(MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS* _delegate) const
    {
        if (0 == _delegate || !_delegate->isType(typeid(MYGUI_C_METHOD_DELEGATE MYGUI_T_TEMPLATE_ARGS)))
        {
            return false;
        }
        MYGUI_C_METHOD_DELEGATE MYGUI_T_TEMPLATE_ARGS* cast = static_cast<  MYGUI_C_METHOD_DELEGATE MYGUI_T_TEMPLATE_ARGS* >(_delegate);
        return cast->mObject == mObject && cast->mMethod == mMethod;
    }
private:
    T*     mObject;
    Method mMethod;
};
} // namespace delegate
// newDelegate beg
// new静态方法委托
// newDelegate(class_name::static_method_name);
MYGUI_TEMPLATE MYGUI_TEMPLATE_PARAMS
inline delegate::MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS* newDelegate(void (* _func)(MYGUI_PARAMS))
{
    return new delegate::MYGUI_C_STATIC_DELEGATE MYGUI_TEMPLATE_ARGS(_func);
}

// new类方法委托
// newDelegate(&object_name, &class_name::method_name);
template MYGUI_T_TEMPLATE_PARAMS
inline delegate::MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS* newDelegate(T* _object, void (T::* _method)(MYGUI_PARAMS))
{
    return new delegate::MYGUI_C_METHOD_DELEGATE MYGUI_T_TEMPLATE_ARGS(_object, _method);
}
// newDelegate end

namespace delegate
{
// 单委托
MYGUI_TEMPLATE MYGUI_TEMPLATE_PARAMS
class MYGUI_C_DELEGATE
{
public:
    typedef  MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS IDelegate;

    MYGUI_C_DELEGATE () : mDelegate(0)
    {
    }
    MYGUI_C_DELEGATE (const MYGUI_C_DELEGATE MYGUI_TEMPLATE_ARGS& _event)
    {
        mDelegate                                                           = _event.mDelegate;
        const_cast<MYGUI_C_DELEGATE MYGUI_TEMPLATE_ARGS&>(_event).mDelegate = 0;
    }
    ~MYGUI_C_DELEGATE ()
    {
        clear();
    }

    bool empty() const
    {
        return mDelegate == 0;
    }

    void clear()
    {
        if (mDelegate)
        {
            delete mDelegate;
            mDelegate = 0;
        }
    }

    MYGUI_C_DELEGATE MYGUI_TEMPLATE_ARGS& operator=(IDelegate* _delegate)
    {
        delete mDelegate;
        mDelegate = _delegate;
        return *this;
    }

    MYGUI_C_DELEGATE MYGUI_TEMPLATE_ARGS& operator=(const MYGUI_C_DELEGATE MYGUI_TEMPLATE_ARGS& _event)
    {
        delete mDelegate;
        mDelegate                                                           = _event.mDelegate;
        const_cast<MYGUI_C_DELEGATE MYGUI_TEMPLATE_ARGS&>(_event).mDelegate = 0;

        return *this;
    }

    void operator()(MYGUI_PARAMS)
    {
        if (mDelegate == 0)
        {
            return;
        }
        mDelegate->invoke(MYGUI_ARGS);
    }

private:
    IDelegate* mDelegate;
};

// 多委托
MYGUI_TEMPLATE MYGUI_TEMPLATE_PARAMS
class MYGUI_C_MULTI_DELEGATE
{
public:
    typedef MYGUI_I_DELEGATE MYGUI_TEMPLATE_ARGS IDelegate;
    typedef MYGUI_TYPENAME std::list<IDelegate*>    ListDelegate;
    typedef MYGUI_TYPENAME ListDelegate::iterator ListDelegateIterator;
    typedef MYGUI_TYPENAME ListDelegate::const_iterator ConstListDelegateIterator;

    MYGUI_C_MULTI_DELEGATE ()
    {
    }
    ~MYGUI_C_MULTI_DELEGATE ()
    {
        clear();
    }

    bool empty() const
    {
        for (ConstListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if (*iter)
            {
                return false;
            }
        }
        return true;
    }

    void clear()
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if (*iter)
            {
                delete (*iter);
                (*iter) = 0;
            }
        }
    }

    MYGUI_C_MULTI_DELEGATE MYGUI_TEMPLATE_ARGS& operator+=(IDelegate* _delegate)
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if ((*iter) && (*iter)->compare(_delegate))
            {
                assert(false, "dublicate delegate");
            }
        }
        mListDelegates.push_back(_delegate);
        return *this;
    }

    MYGUI_C_MULTI_DELEGATE MYGUI_TEMPLATE_ARGS& operator-=(IDelegate* _delegate)
    {
        for (ListDelegateIterator iter = mListDelegates.begin(); iter != mListDelegates.end(); ++iter)
        {
            if ((*iter) && (*iter)->compare(_delegate))
            {
                if ((*iter) != _delegate)
                {
                    delete (*iter);
                }
                (*iter) = 0;
                break;
            }
        }
        delete _delegate;
        return *this;
    }

    void operator()(MYGUI_PARAMS)
    {
        ListDelegateIterator iter = mListDelegates.begin();
        while (iter != mListDelegates.end())
        {
            if (0 == (*iter))
            {
                iter = mListDelegates.erase(iter);
            }
            else
            {
                (*iter)->invoke(MYGUI_ARGS);
                ++iter;
            }
        }
    }

private:
    // constructor and operator =, without implementation, just for private
    MYGUI_C_MULTI_DELEGATE (const MYGUI_C_MULTI_DELEGATE MYGUI_TEMPLATE_ARGS& _event);
    MYGUI_C_MULTI_DELEGATE MYGUI_TEMPLATE_ARGS& operator=(const MYGUI_C_MULTI_DELEGATE MYGUI_TEMPLATE_ARGS& _event);


private:
    ListDelegate mListDelegates;
};

// 撤销定义的字符串
#undef MYGUI_COMBINE
#undef MYGUI_COMBINE1

#undef MYGUI_I_DELEGATE

#undef MYGUI_C_STATIC_DELEGATE
#undef MYGUI_C_METHOD_DELEGATE

#undef MYGUI_C_DELEGATE
#undef MYGUI_C_MULTI_DELEGATE

#undef MYGUI_SUFFIX
#undef MYGUI_TEMPLATE
#undef MYGUI_TEMPLATE_PARAMS
#undef MYGUI_TEMPLATE_ARGS
#undef MYGUI_T_TEMPLATE_PARAMS
#undef MYGUI_T_TEMPLATE_ARGS
#undef MYGUI_PARAMS
#undef MYGUI_ARGS
#undef MYGUI_TYPENAME
} // namespace delegate
