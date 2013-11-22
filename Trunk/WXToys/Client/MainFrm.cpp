
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WXRegion.h"

#include "MainFrm.h"

#include "Dijkstra.h"

#include "Heightmap.h"

#include "LightMap.h"

#include "GridInfo.h"

#include "XRegion.h"

#include "WXScene.h"

#include "NullInfo.h"

#include "WXTerrain.h"

#include "WXNavmap.h"

#include <sstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

const int iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId  = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
ON_WM_CREATE()
ON_WM_SETFOCUS()
ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
ON_WM_CHAR()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,               // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
    : m_fileinfo(NULL)
    , drawtype(DRAW_NULL)
    , m_bTerrainValid(FALSE)
{
}

CMainFrame::~CMainFrame()
{
}

UINT CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    BOOL bNameValid;

    // set the visual manager used to draw all user interface elements
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));

    if (!m_wndMenuBar.Create(this))
    {
        TRACE0("Failed to create menubar\n");
        return -1;              // fail to create
    }

    m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

    // prevent the menu bar from taking the focus on activation
    CMFCPopupMenu::SetForceMenuFocus(FALSE);

    // create a view to occupy the client area of the frame
    if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
    {
        TRACE0("Failed to create view window\n");
        return -1;
    }

    m_wndView.SetMainFrame(this);

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |
                               CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;              // fail to create
    }

    CString strToolBarName;
    bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
    ASSERT(bNameValid);
    m_wndToolBar.SetWindowText(strToolBarName);

    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);
    m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

    // Allow user-defined toolbars operations:
    InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;              // fail to create
    }
    m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

    // TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndMenuBar);
    DockPane(&m_wndToolBar);


    // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // Enable toolbar and docking window menu replacement
    EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

    // enable quick (Alt+drag) toolbar customization
    CMFCToolBar::EnableQuickCustomization();

    if (CMFCToolBar::GetUserImages() == NULL)
    {
        // load user-defined toolbar images
        if (m_UserImages.Load(_T(".\\UserImages.bmp")))
        {
            m_UserImages.SetImageSize(CSize(16, 16), FALSE);
            CMFCToolBar::SetUserImages(&m_UserImages);
        }
    }

    // enable menu personalization (most-recently used commands)
    // TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
    CList<UINT, UINT> lstBasicCommands;

    lstBasicCommands.AddTail(ID_APP_EXIT);
    lstBasicCommands.AddTail(ID_EDIT_CUT);
    lstBasicCommands.AddTail(ID_EDIT_PASTE);
    lstBasicCommands.AddTail(ID_EDIT_UNDO);
    lstBasicCommands.AddTail(ID_APP_ABOUT);
    lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
    lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

    CMFCToolBar::SetBasicCommands(lstBasicCommands);


    //////////////////////////////////////////////////////////////////////////
    m_fileinfo.reset(new NullInfo);
    // m_fileinfo.reset(NULL);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if ( !CFrameWndEx::PreCreateWindow(cs))
    {
        return FALSE;
    }
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
               | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass  = AfxRegisterWndClass(0);
    return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWndEx::Dump(dc);
}
#endif // _DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
    // forward focus to the view window
    m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, UINT nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // let the view have first crack at the command
    if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
    {
        return TRUE;
    }

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnViewCustomize()
{
    CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
    pDlgCust->EnableUserDefinedToolbars();
    pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp)
{
    LRESULT      lres = CFrameWndEx::OnToolbarCreateNew(wp, lp);
    if (lres == 0)
    {
        return 0;
    }

    CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
    ASSERT_VALID(pUserToolbar);

    BOOL         bNameValid;
    CString      strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
    return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
    // base class does the real work

    if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
    {
        return FALSE;
    }


    // enable customization button for all user toolbars
    BOOL    bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    for (int i = 0; i < iMaxUserToolbars; i++)
    {
        CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
        if (pUserToolbar != NULL)
        {
            pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
        }
    }

    return TRUE;
}



void CMainFrame::OnFileOpen()
{
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL);

    if (dlg.DoModal() == IDOK)
    {
        CString strFilename = dlg.GetPathName();

        CString strCity = PathFindFileName(strFilename);
        int     pos     = strCity.Find(".");
        strCity = strCity.Left(pos);

        CString strExt = ::PathFindExtension(strFilename);
        strExt.MakeLower();

        // ! 当前打开的城市不是已经解析了的城市
        // ! 必须先解析terrain
        if ( strExt != TEXT(".terrain"))
        {
            if (!m_bTerrainValid || strCity != m_strCity)
            {
                CString msg;
                msg.Format(TEXT("citye called %s 还没有解析地形数据，无法解析文件%s,你需要先解析%s.terrain"), strCity, strFilename, strCity);
                AfxMessageBox(msg);
                return;
            }
        }


        try
        {
            // ! DRAW_SCENE
            if (strExt == TEXT(".scene"))
            {
                m_fileinfo.reset(new WXScene);
                drawtype = DRAW_SCENE;
            }
            if (strExt == TEXT(".terrain"))
            {
                m_fileinfo.reset(new WXTerrain);
                drawtype = DRAW_TERRAIN;
            }
            else if (strExt == TEXT(".heightmap"))
            {
                m_fileinfo.reset(new Heightmap);
                drawtype = DRAW_HEIGHTMAP;
            }
            else if (strExt == TEXT(".region"))
            {
                m_fileinfo.reset(new XRegion);
                drawtype = DRAW_REGION;
            }
            else if (strExt == TEXT(".dijkstra"))
            {
                m_fileinfo.reset(new Dijkstra);
                drawtype = DRAW_DIJKSTRA;
            }
            else if (strExt == TEXT(".wcollision"))
            {
                m_fileinfo.reset(new XCollision);
                drawtype = DRAW_WCOLLISION;
            }
            else if (strExt == TEXT(".nav"))
            {
                m_fileinfo.reset(new WXNavmap);
                drawtype = DRAW_NAV;
            }
            else if (strExt == TEXT(".png"))
            {
                m_fileinfo.reset(new LightMap);
                drawtype = DRAW_LIGHTMAP;
            }
            else if (strExt == TEXT(".gridinfo"))
            {
                m_fileinfo.reset(new WXGridInfo);
                drawtype = DRAW_GRIDINFO;
            }

            if (strFilename.IsEmpty())
            {
                throw("no file");
            }

            // ! 检查城市地形信息

            if (m_fileinfo.get())
            {
                m_fileinfo->LoadFile(strFilename);
                if (/*!m_bTerrainValid && */ strExt == TEXT(".terrain"))
                {
                    WXTerrain* pTerrain = static_cast< WXTerrain* >(m_fileinfo.get());
                    FileInfoInterface::SetInfo(pTerrain->width, pTerrain->height, pTerrain->tileSize);
                    TRACE(TEXT("city:%s , w:%d,h:%d,titleSize:%d"), strCity, pTerrain->width, pTerrain->height, pTerrain->tileSize);
                    m_strCity       = strCity;
                    m_bTerrainValid = TRUE;
                }
            }
        }
        catch (string e)
        {
            AfxMessageBox(e.c_str());
        }
        catch (exception e)
        {
            AfxMessageBox(e.what());
        }
        catch (char* e)
        {
            AfxMessageBox(e);
        }

        catch (...)
        {
            AfxMessageBox("...");
        }

        // MoveWindow(0,0,m_maxVec.x+10 , m_maxVec.y+10);

        m_wndView.Invalidate();
    }
}



void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default
    int i = 0;

    CFrameWndEx::OnChar(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int i = 0;

    CFrameWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    if (message == WM_KEYDOWN )
    {
        m_wndView.ScreenShot();
    }

    return CFrameWndEx::DefWindowProc(message, wParam, lParam);
}
