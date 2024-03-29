
// WXRegion.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "WXRegion.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWXRegionApp

BEGIN_MESSAGE_MAP(CWXRegionApp, CWinAppEx)
ON_COMMAND(ID_APP_ABOUT, &CWXRegionApp::OnAppAbout)
END_MESSAGE_MAP()


// CWXRegionApp construction

CWXRegionApp::CWXRegionApp()
{
    m_bHiColorIcons = TRUE;

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// The one and only CWXRegionApp object

CWXRegionApp theApp;


// CWXRegionApp initialization

BOOL CWXRegionApp::InitInstance()
{
    CWinAppEx::InitInstance();

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);    // Shutdown


    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    InitContextMenuManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
                                                 RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object
    CMainFrame*     pFrame = new CMainFrame;
    if (!pFrame)
    {
        return FALSE;
    }
    m_pMainWnd = pFrame;
    // create and load the frame with its resources
    pFrame->LoadFrame(IDR_MAINFRAME,
                      WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
                      NULL);






    // The one and only window has been initialized, so show and update it
    pFrame->ShowWindow(SW_SHOWMAXIMIZED);
    pFrame->UpdateWindow();
    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}


// CWXRegionApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Implementation
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

// App command to run the dialog
void CWXRegionApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CWXRegionApp customization load/save methods

void CWXRegionApp::PreLoadState()
{
    BOOL    bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CWXRegionApp::LoadCustomState()
{
}

void CWXRegionApp::SaveCustomState()
{
}

// CWXRegionApp message handlers




int CWXRegionApp::ExitInstance()
{
    GdiplusShutdown(gdiplusToken);

    return CWinAppEx::ExitInstance();
}
