#include "StdAfx.h"
#include "WXNavmap.h"

#include <limits>




WXNavmap::WXNavmap(void)
    : m_max(1)
{
}

WXNavmap::~WXNavmap(void)
{
}


// 读取导航信息，文件由编辑器生成
VOID WXNavmap::ReadNavMap(const CHAR* filename, int& mx, int& mz)
{
    FILE*        fp = fopen(filename, "rb");
    if (NULL == fp)
    {
        ASSERT(fp);
        return;
    }

    _NAVMAP_HEAD head;
    fread(&head, sizeof(_NAVMAP_HEAD), 1, fp);

    mWidth       = head.width;
    mHeight      = head.height;
    mGridSize    = 0.5;
    mInvGridSize = 1 / mGridSize;

    mx = (int)(mWidth * mGridSize);
    mz = (int)(mHeight * mGridSize);

    mLeftTopx = 0;
    mLeftTopz = 0;

    mMaxNode = mWidth * mHeight;

    mWorld     = new _WORLD[mMaxNode];
    mWorkWorld = new _WORLD[mMaxNode];
    mNodes     = new _NODES[mMaxNode + 1];

    INT size = sizeof(struct _NAVMAP_HEAD);
    for (INT j = 0; j < mHeight; j++)
    {
        for (INT i = 0; i < mWidth; i++)
        {
            INT     info;
            fread(&info, size, 1, fp);

            _WORLD* pWorld = mWorld + j * mWidth + i;
            pWorld->state = info;
            if (pWorld->state > m_max)
            {
                m_max = pWorld->state;
            }
        }
    }

    mNodes[0].zx = 0;
    mNodes[0].f  = 0;
    mNodes[0].g  = 0;

    // curStep=0;
}


void WXNavmap::Draw(CDC& dc, int width, int height)
{
    m_strInfomaiton.Empty();
    // __super::Draw(dc, width, height);

    CRect rcDraw(0, 0, width, height);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0, 255, 0));
    dc.FillSolidRect(rcDraw, RGB(0, 0, 0));

    m_strInfomaiton.AppendFormat(TEXT("\n"));
#undef max

    int   max_int  = numeric_limits< INT >::max();
    int   max_byte = numeric_limits< BYTE >::max();
    float sacle    =  max_byte / m_max;

    for (INT j = 0; j < mHeight; j++)
    {
        for (INT i = 0; i < mWidth; i++)
        {
            _WORLD* pWorld = mWorld + j * mWidth + i;
            BYTE    b      = pWorld->state;
            b *= sacle;

            dc.SetPixel(i, j, RGB(b, 0, 0));
        }
        // m_strInfomaiton.AppendFormat(TEXT("\n") );
    }


    dc.SetTextColor(RGB(255, 255, 0));

    m_strInfomaiton.Format(TEXT("W:%d,H:%d ,grid size:%f "), mWidth, mHeight, mGridSize);
    dc.DrawText(m_strInfomaiton, rcDraw, DT_BOTTOM | DT_CENTER);

    // mWidth * mHeight;
    // int points = sizeof(mWorld)/sizeof(_WORLD );


    // void  DrawPoints(CDC& dc , const vector<fVector2>	points , COLORREF color , fVector2 factor)
}
