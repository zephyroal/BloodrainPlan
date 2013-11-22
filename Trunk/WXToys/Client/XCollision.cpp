#include "StdAfx.h"
#include "XCollision.h"

#define TRACE void()

XCollision::XCollision(void)
{
}

XCollision::~XCollision(void)
{
}

UINT XCollision::LoadCollisionTriInfoFromFile(const char* szFileName)
{
    // ostringstream oss;
    // oss<<"open "<<szFileName<<endl;
    m_strInfomaiton = szFileName;

    // 读取新的数据之前， 要清空数据.
    m_triInfoMap.clear();

    if (!szFileName || szFileName[0] == '\0')
    {
        return 1;
    }

    CFile           file;
    BOOL            bOpened = file.Open(szFileName, CFile::modeRead | CFile::typeBinary);
    if (!bOpened)
    {
        // AfxMessageBox("can't create file");
        return -1;
    }


    DWORD           dwSize    = file.GetLength();
    char*           lpAddress = new char[dwSize + 1];
    lpAddress[dwSize] = 0;
    UINT            nReaded = file.Read(lpAddress, dwSize);
    ASSERT(nReaded == dwSize);

    if (dwSize > 50 * 1024 * 1024)
    {
        throw("文件过大，不予解析");
    }

    if (dwSize <= 0)
    {
        AfxMessageBox("文件过小，不予解析");

        return -1;
    }


    char*           pPoint = lpAddress;

    TRI_INFO        triInfo;
    POINT_2D        pos;
    UINT            dwVersion = 0;
    INT             iPosCount = 0;
    INT             iTriCount = 0;

    TRI_INFO_VECTOR triInfoVector;

    // 读取版本号
    // ::fread(&dwVersion, sizeof(dwVersion), 1, pFile);
    memcpy(&dwVersion, pPoint, sizeof(dwVersion)); pPoint += sizeof(dwVersion);

    // 读取注册过的三角形的位置的个数
    // ::fread(&iPosCount, sizeof(iPosCount), 1, pFile);
    memcpy(&iPosCount, pPoint, sizeof(iPosCount)); pPoint += sizeof(iPosCount);




    TRACE("\nversion:%d , count:%d\n", dwVersion, iPosCount);
    m_strInfomaiton.AppendFormat("\nversion:%d , count:%d\n", dwVersion, iPosCount);
    for (INT i = 0; i < iPosCount; i++)
    {
        // 清空三角形信息数组。
        triInfoVector.clear();

        // 读取位置
        // ::fread(&pos, sizeof(pos), 1, pFile);
        memcpy(&pos, pPoint, sizeof(pos)); pPoint += sizeof(pos);

        // 读取注册这个位置的三角形的个数
        // ::fread(&iTriCount, sizeof(iTriCount), 1, pFile);
        memcpy(&iTriCount, pPoint, sizeof(iTriCount)); pPoint += sizeof(iTriCount);

        TRACE("\n[i:%2d] , pos:%d , count:%d\n", i, pos, iTriCount);
        m_strInfomaiton.AppendFormat("\n[i:%2d] , pos:%d , count:%d\n", i, pos, iTriCount);

        for (INT j = 0; j < iTriCount; j++)
        {
            // ::fread(&triInfo, sizeof(triInfo), 1, pFile);
            memcpy(&triInfo, pPoint, sizeof(triInfo)); pPoint += sizeof(triInfo);
            triInfoVector.push_back(triInfo);
            TRACE("(%f,%f,%f) ", triInfo.v1.fx, triInfo.v1.fy, triInfo.v1.fz);
            m_strInfomaiton.AppendFormat("(%f,%f,%f) ", triInfo.v1.fx, triInfo.v1.fy, triInfo.v1.fz);
        }

        TRACE("\n--------------\n");
        m_strInfomaiton.AppendFormat("\n--------------\n");
        if (triInfoVector.size())
        {
            m_triInfoMap[pos] = triInfoVector;
        }

        // ! debug info

        // ostringstream oss;
        // oss<<"i:"<<i;
        // oss<<" count:"<<region.m_nCount;


        // string strMsg(oss.str());
        // TRACE(strMsg.c_str() );
    }

    // string str = oss.str();
    // m_strInfomaiton = str.c_str();



    return 0;
}


void XCollision::Draw(CDC& dc, int width, int height)
{
    // CRect rcDraw( 0 , 0 , width , height);
    // dc.SetBkMode(TRANSPARENT );
    // dc.SetTextColor(RGB(0,0,255) );
    // dc.DrawText(m_strInfomaiton  ,rcDraw , DT_TOP|DT_LEFT );
    __super::Draw(dc, width, height);
}
