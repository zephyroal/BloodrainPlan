
// WXRegion.h : main header file for the WXRegion application
//
#pragma once

#ifndef __AFXWIN_H__
        #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CWXRegionApp:
// See WXRegion.cpp for the implementation of this class
//

class CWXRegionApp : public CWinAppEx
{
public:
    CWXRegionApp();


// Overrides
public:
    virtual BOOL InitInstance();

// Implementation

public:
    BOOL      m_bHiColorIcons;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    virtual int ExitInstance();
};

extern CWXRegionApp theApp;
