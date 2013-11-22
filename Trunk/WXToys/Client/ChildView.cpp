
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "WXRegion.h"
#include "ChildView.h"

#include "MainFrm.h"

#include <stdlib.h>
#include <MMSystem.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
    : m_pMainFrame(0)
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_ERASEBKGND()
ON_WM_TIMER()
ON_WM_CHAR()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
    {
        return FALSE;
    }

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style     &= ~WS_BORDER;
    cs.lpszClass  = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
                                        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

    return TRUE;
}

void CChildView::OnPaint()
{
    CPaintDC           dc(this); // device context for painting
    CRect              rcClient;
    GetClientRect(&rcClient);


    if (!m_pMainFrame )
    {
        return;
    }

    CDC                memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap            memBmp;
    memBmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    memDC.SelectObject(&memBmp);


    // DRAW_HEIGHTMAP,
    // DRAW_REGION,
    // DRAW_DIJKSTRA,
    // DRAW_WCOLLISION,
    // DRAW_LIGHTMAP,
    // DRAW_GRIDINFO,
    // DRAW_NAV,
    // DRAW_TERRAIN,
    // DRAW_SCENE
    int                drawType = m_pMainFrame->GetDrawType();
    FileInfoInterface* fileInfo = m_pMainFrame->GetFileinfo();
    if (fileInfo)
    {
        ASSERT(fileInfo);
        fileInfo->Draw(memDC, rcClient.Width(), rcClient.Height());
    }

    // switch(drawType )
    // {

    // case DRAW_HEIGHTMAP:
    //
    //	break;
    // }
    // FileInfoInterface* fileInfo = NULL;
    // fileInfo

    // memDC.SetTextColor(RGB(255,255,0) );
    // memDC.SetBkMode(TRANSPARENT );
    // if (regions.size()<=0)
    // {
    //	memDC.DrawText("使用方法:在菜单中打开WX1的rengion文件即可" , rcClient , DT_LEFT );
    // }

    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.DeleteDC();
}

void CChildView::SetMainFrame(CMainFrame* pMainFrame)
{
    ASSERT(pMainFrame);
    m_pMainFrame = pMainFrame;

    SetTimer(1, 30, NULL);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    // srand( timeGetTime() );
    // TODO:  Add your specialized creation code here


    return 0;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return 1;
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    // Invalidate(FALSE );

    CWnd::OnTimer(nIDEvent);
}


void CChildView::ScreenShot()
{
    CWXImage image(m_hWnd);
    CBitmap  bmp;
    CRect    rcClient;
    GetClientRect(&rcClient);
    bmp.CreateCompatibleBitmap(GetDC(), rcClient.Width(), rcClient.Height());

    image.ScreehShots(bmp, GetDC()->GetSafeHdc());
}

void CChildView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

LRESULT CChildView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    if (message == VK_SNAPSHOT )
    {
        // if (wParam == VK_PRINTSCREEN )
        // {
        // }
        ScreenShot();
    }

    return CWnd::DefWindowProc(message, wParam, lParam);
}
