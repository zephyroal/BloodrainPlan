// Deferred.h : Deferred Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CDeferredApp:
// �йش����ʵ�֣������ Deferred.cpp
//

class CDeferredApp : public CWinApp
{
public:
	CDeferredApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDeferredApp theApp;