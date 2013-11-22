// Deferred.cpp : ����Ӧ�ó��������Ϊ��
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


// CDeferredApp ����

CDeferredApp::CDeferredApp()
{
    // TODO: �ڴ˴���ӹ�����룬
    // ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDeferredApp ����

CDeferredApp theApp;


// CDeferredApp ��ʼ��

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


// CDeferredApp ��Ϣ�������




// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// �Ի�������
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CDeferredApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CDeferredApp ��Ϣ�������
