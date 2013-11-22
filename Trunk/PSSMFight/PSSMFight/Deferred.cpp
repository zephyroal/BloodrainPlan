// Deferred.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Deferred.h"
#include "MainFrm.h"
#include "TestCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDeferredApp

BEGIN_MESSAGE_MAP(CDeferredApp, CWinApp)
ON_COMMAND(ID_APP_ABOUT, &CDeferredApp::OnAppAbout)
END_MESSAGE_MAP()


// CDeferredApp 构造

CDeferredApp::CDeferredApp()
{
    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDeferredApp 对象

CDeferredApp theApp;


// CDeferredApp 初始化

BOOL CDeferredApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();
    SetRegistryKey(_T("????"));

    CMainFrame*          pFrame = new CMainFrame;
    if (!pFrame)
    {
        return FALSE;
    }

    m_pMainWnd = pFrame;

    pFrame->LoadFrame(IDR_MAINFRAME,
                      WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
                      NULL);

    pFrame->ShowWindow(SW_RESTORE);

    TestCore*            pTestCore = new TestCore;

    RECT                 rect;
    pFrame->m_wndView.GetClientRect(&rect);

    bool                 res = pTestCore->Initialize(pFrame->GetSafeHwnd(),
                                                     pFrame->m_wndView.GetSafeHwnd(), rect.right - rect.left, rect.bottom - rect.top);

    if ( res == false )
    {
        return false;
    }

    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();

    return TRUE;
}


// CDeferredApp 消息处理程序




// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CDeferredApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CDeferredApp 消息处理程序
