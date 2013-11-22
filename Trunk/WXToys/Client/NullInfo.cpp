#include "StdAfx.h"
#include "NullInfo.h"

NullInfo::NullInfo(void)
{
}

NullInfo::~NullInfo(void)
{
    int i = 0;
}

BOOL NullInfo::LoadFile(CONST CString& strFile)
{
    return TRUE;
}

void NullInfo::Draw(CDC& dc, int width, int height)
{
    CRect rcDraw(0, 0, width, height);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(255, 255, 0));
    m_strInfomaiton = (TEXT("���Խ������ļ�����:\n"));
    m_strInfomaiton.AppendFormat(TEXT("scene;terrain;nav;gridinfo;heightmap;lightmap;region;wcollsion;dijkstra\n"));
    m_strInfomaiton.AppendFormat(TEXT("ʹ�÷�ʽ:�ڲ˵��д��ļ�����\n"));
    m_strInfomaiton.AppendFormat(TEXT("�ȼ��ض�Ӧ���е�terain�ļ����ٽ���������������ļ�"));

    dc.DrawText(m_strInfomaiton, rcDraw, DT_CENTER | DT_VCENTER | DT_WORDBREAK);

    // __super::Draw(dc , width , height );
}
