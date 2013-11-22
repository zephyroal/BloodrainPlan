
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ChildView.h"

#include "Region.h"

#include "XCollision.h"

#include <vector>
using namespace std;

class CMainFrame : public CFrameWndEx
{
    _DRAWTYPE drawtype;

public:
    CMainFrame();
protected:
    DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, UINT nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL LoadFrame(UINT            nIDResource,
                           DWORD           dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
                           CWnd*           pParentWnd = NULL,
                           CCreateContext* pContext = NULL);

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CMFCMenuBar       m_wndMenuBar;
    CMFCToolBar       m_wndToolBar;
    CMFCStatusBar     m_wndStatusBar;
    CMFCToolBarImages m_UserImages;
    CChildView        m_wndView;

// Generated message map functions
protected:
    afx_msg UINT OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnViewCustomize();
    afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()


    //
    // VOID			LoadRegionBinary(LPCTSTR szRegionFile);

    ////Region链表
    // std::vector< CRegion >		m_theRegion;
    // fVector2 m_maxVec;

public:
    // const vector< CRegion >& GetRegion() const;
    // const fVector2& GetMaxVec();

public:
    afx_msg void OnFileOpen();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
    int GetDrawType()
    {
        return drawtype;
    }

    // FileInfoInterface * m_fileinfo;
    std::auto_ptr<FileInfoInterface> m_fileinfo;


    FileInfoInterface* GetFileinfo()
    {
        return m_fileinfo.get();
    }

    // ! 标志当前城市名是否解析
    BOOL                             m_bTerrainValid;

    // !  标志成功解析了的城市
    CString                          m_strCity;
};
