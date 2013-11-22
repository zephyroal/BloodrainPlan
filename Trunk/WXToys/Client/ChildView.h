
// ChildView.h : interface of the CChildView class
//


#pragma once

class CMainFrame;

#include <vector>
#include "TDMath.h"

using namespace std;

// CChildView window

#include "wximage.h"


enum _DRAWTYPE
{
    DRAW_NULL,
    DRAW_HEIGHTMAP,
    DRAW_REGION,
    DRAW_DIJKSTRA,
    DRAW_WCOLLISION,
    DRAW_LIGHTMAP,
    DRAW_GRIDINFO,
    DRAW_NAV,
    DRAW_TERRAIN,
    DRAW_SCENE
};

class CChildView : public CWnd
{
// Construction
public:
    CChildView();

// Attributes
public:

// Operations
public:

// Overrides
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
    virtual ~CChildView();

    // Generated message map functions
protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()

    CMainFrame * m_pMainFrame;

public:
    void SetMainFrame(CMainFrame* pMainFrame);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    void ScreenShot();

    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
