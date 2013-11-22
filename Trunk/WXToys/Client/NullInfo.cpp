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
    m_strInfomaiton = (TEXT("可以解析的文件如下:\n"));
    m_strInfomaiton.AppendFormat(TEXT("scene;terrain;nav;gridinfo;heightmap;lightmap;region;wcollsion;dijkstra\n"));
    m_strInfomaiton.AppendFormat(TEXT("使用方式:在菜单中打开文件即可\n"));
    m_strInfomaiton.AppendFormat(TEXT("先加载对应城市的terain文件，再解析这个城市其他文件"));

    dc.DrawText(m_strInfomaiton, rcDraw, DT_CENTER | DT_VCENTER | DT_WORDBREAK);

    // __super::Draw(dc , width , height );
}
