#include "StdAfx.h"
#include "XRegion.h"

XRegion::XRegion(void)
{
}

XRegion::~XRegion(void)
{
}

VOID XRegion::LoadRegionBinary(LPCTSTR szRegionFile)
{
    if (!szRegionFile || szRegionFile[0] == '\0')
    {
        return;
    }
    m_theRegion.clear();

    CFile file;
    BOOL  bOpened = file.Open(szRegionFile, CFile::modeRead | CFile::typeBinary);
    if (!bOpened)
    {
        // AfxMessageBox("can't create file");
        return;
    }


    DWORD dwSize    = file.GetLength();
    char* lpAddress = new char[dwSize + 1];
    lpAddress[dwSize] = 0;
    UINT  nReaded = file.Read(lpAddress, dwSize);
    ASSERT(nReaded == dwSize);

    if (dwSize > 1024 * 1024)
    {
        throw("文件过大，不予解析");
    }


    if (dwSize > 0)
    {
        INT      iRegionCount = 0;
        INT      iPointCount  = 0;
        INT      iId          = 0;
        INT      FlyLevel     = 0;
        bool     NewVer       = false;
        FLOAT    fTemp        = 0;
        fVector2 fvp;
        char*    pPoint = lpAddress;
        // 判断版本
        int      iVerLen = (int)sizeof("REGION_EDIT_01");
        char*    pVer    = new char[iVerLen];
        memcpy(pVer, pPoint, iVerLen - 1);
        pVer[iVerLen - 1] = 0;

        if ( strcmp(pVer, "REGION_EDIT_01") == 0 )
        {
            pPoint += (iVerLen - 1);
            NewVer  = true;
        }

        delete[] pVer;

        // 读取区域的个数
        memcpy(&iRegionCount, pPoint, sizeof(iRegionCount));
        pPoint += sizeof(iRegionCount);

        if (iPointCount > 10240)
        {
            AfxMessageBox("当前区块过多，不予解析");
            return;
        }


        m_theRegion.reserve(iRegionCount);
        for (INT i = 0; i < iRegionCount; i++)
        {
            // 读取区域的id
            // memcpy(&(iId), pPoint, sizeof(iId));
            pPoint += sizeof(INT);             // skip regionid
            CRegion region;
            region.m_RegionID = i;

            if (NewVer)
            {
                memcpy(&FlyLevel, pPoint, sizeof(FlyLevel));
                pPoint                   += sizeof(FlyLevel);
                region.m_nPassRegionLevel = FlyLevel;
            }

            // 读取点的个数.
            memcpy(&iPointCount, pPoint, sizeof(iPointCount));
            pPoint += sizeof(iPointCount);

            if (iPointCount > 10240)
            {
                AfxMessageBox("当前图形点数过多，不予解析");
                return;
            }

            for (INT k = 0; k < iPointCount; k++)
            {
                // 读取x， z 数据
                memcpy(&fvp.x, pPoint, sizeof(fvp.x)); pPoint += sizeof(fvp.x);
                memcpy(&fvp.y, pPoint, sizeof(fvp.y)); pPoint += sizeof(fvp.y);
                memcpy(&fTemp, pPoint, sizeof(fTemp)); pPoint += sizeof(fTemp);

                region.AddPoint(&fvp);
                if (fvp.x > m_maxVec.x)
                {
                    m_maxVec.x = fvp.x;
                }
                if (fvp.y > m_maxVec.y)
                {
                    m_maxVec.y = fvp.y;
                }
            }
            m_theRegion.push_back(region);

            // INT						m_RegionID;
            // INT						m_nCount;
            // INT						m_nPassRegionLevel;// 可通过区域的 行走等级 fujia 2007.12.27
            // std::vector<fVector2>	m_vfPoints;
            // std::vector<BOOL>		m_vbBulge;

            ostringstream oss;
            oss << "id:" << region.m_RegionID;
            oss << " count:" << region.m_nCount;
            oss << " level:" << region.m_nPassRegionLevel;
            oss << " points:" << region.m_vfPoints.size();
            oss << " bulge:" << region.m_vbBulge.size();
            oss << " [points:";

            for (int i = 0; i < region.m_vfPoints.size(); ++i )
            {
                const fVector2& vec = region.m_vfPoints[i];
                oss << "(" << vec.x << "," << vec.y << ")";
            }
            oss << "]";

            oss << " [bulge:";
            for (int i = 0; i < region.m_vbBulge.size(); ++i )
            {
                const BOOL& boolean = region.m_vbBulge[i];
                oss << boolean << ",";
            }
            oss << "]\n";


            string strMsg(oss.str());
            TRACE(strMsg.c_str());
        }
    }

    // !
    TRACE("\nMax x:%f,max y:%f", m_maxVec.x, m_maxVec.y);
    // 将Region注册到Zone
    // RegisteAllRegion();

    delete lpAddress;
}

void XRegion::Draw(CDC& dc, int width, int height)
{
    const vector< CRegion >& regions = m_theRegion;
    const fVector2&          maxVec  = m_maxVec;

    CRect                    rcDraw(0, 0, width, height);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0, 0, 255));
    dc.FillSolidRect(rcDraw, RGB(0, 0, 0));

    fVector2                 factor;
    factor.x = (width) / maxVec.x;
    factor.y = (height) / maxVec.y;

    COLORREF                 color;
    // rand();

    for (int i = 0; i < regions.size(); ++i )
    {
        const CRegion& reg = regions[i];
        int            r   = rand() % 256;
        int            g   = rand() % 256;
        int            b   = rand() % 256;
        if (r < 10 && b < 10 && b < 10)
        {
            r += 10;
            g += 10;
            b += 10;
        }
        color = RGB(r, g, b);
        DrawPoints(dc, reg.m_vfPoints, color, factor);
    }


    m_strInfomaiton.Format("regions:%d,max x:%f,max y:%f", regions.size(), maxVec.x, maxVec.y);
    dc.SetTextColor(RGB(255, 255, 0));
    dc.DrawText(m_strInfomaiton, rcDraw, DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
}
