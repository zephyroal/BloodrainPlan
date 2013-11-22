#include "StdAfx.h"
#include "Heightmap.h"

#include <limits>

// #include <GdiplusHeaders.h>

Heightmap::Heightmap(void)
{
    mXSize    = m_width;
    mZSize    = m_height;
    mTileSize = m_titlSize;


    m_nTop   = 0;
    m_bottom = 0;
}

Heightmap::~Heightmap(void)
{
}

void Heightmap::_loadHeightmap(const string& filename)
{
    mHeightmap.resize((mXSize + 1) * (mZSize + 1));
    // mHeightmap.capacity();
    // mHeightmap.reserve((mXSize + 1) * (mZSize + 1));
    std::fill(mHeightmap.begin(), mHeightmap.end(), (Real)0);
    m_strInfomaiton = filename.c_str();

    // Image img((CStringW)(CStringA)filename.c_str() );

    // img.GetType();
    ////img.FromFile(filename);

    // UINT width = img.GetWidth();
    // UINT height = img.GetHeight();

    // ! read floats
    CFile        file;
    BOOL         bOpened = file.Open(filename.c_str(), CFile::modeRead | CFile::typeBinary);
    if (!bOpened)
    {
        AfxMessageBox("can't create file");
        return;
    }
    int          fileLength = file.GetLength();



    unsigned int header[4];
    file.Read(header, 4 * sizeof(unsigned int));
    if (header[0] != TERRAIN_HEIGHTMAP_MAGIC)
    {
        throw("TERRAIN_HEIGHTMAP_MAGIC");
    }
    if (header[1] != TERRAIN_HEIGHTMAP_VERSION)
    {
        throw("TERRAIN_HEIGHTMAP_VERSION");
    }
    if (header[2] != mXSize + 1 || header[3] != mZSize + 1)
    {
        throw("Dimensions of heightmap file doesn't match the dimensions of the terrain");
    }

    //////////////////////////////////////////////////////////////////////////


    int nRead = file.Read(&mHeightmap[0], mHeightmap.size() * sizeof(float));


    int ccc = (m_height + 1) * (m_width + 1);

    ASSERT((m_height + 1) * (m_width + 1) <= mHeightmap.size());

    int cntHeightmap = mHeightmap.size();
    m_strInfomaiton.AppendFormat("\n-------------- height maps points count:%d --------------\n", cntHeightmap);
    for (int i = 0; i < cntHeightmap; ++i )
    {
        if (i < 100)
        {
            m_strInfomaiton.AppendFormat("%f ", mHeightmap[i]);
        }

        if (mHeightmap[i] > m_nTop)
        {
            m_nTop = mHeightmap[i];
        }
        if (mHeightmap[i] < m_bottom )
        {
            m_bottom = mHeightmap[i];
        }

        // if (i%8==7)
        // {
        //	m_strInfomaiton.AppendFormat("\n");
        // }
    }

    m_strInfomaiton.AppendFormat(TEXT("\ntop:%f,bottom:%f"), m_nTop, m_bottom);

    m_strInfomaiton.AppendFormat("\n----------\n");
}



void Heightmap::Draw(CDC& dc, int width, int height)
{
    // m_strInfomaiton.Empty();
    // __super::Draw(dc, width, height);

    CRect    rcDraw(0, 0, width, height);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(0, 255, 0));
    dc.FillSolidRect(rcDraw, RGB(0, 0, 0));

    // m_strInfomaiton.AppendFormat(TEXT("\n") );
#undef max

    // int max_int = numeric_limits< INT >::max();
    int      max_byte = numeric_limits< BYTE >::max();


    float    sacle      =  max_byte / m_nTop;
    float    minusScale =  max_byte / m_bottom;

    BYTE     cl    = 0;
    COLORREF color = RGB(0, 0, 0);

    int      w = m_width + 1;
    int      h = m_height + 1;


    for (INT j = 0; j < h; j++)
    {
        for (INT i = 0; i < w; i++)
        {
            int   index = j * w + i;

            float fH = mHeightmap[index];
            if (index >= w * h - w * 2)
            {
                int i = 0;
            }

            if ( fH >= 0 )
            {
                // ! read
                cl    = fH * sacle;
                color = RGB(cl, 0, 0);
            }
            else
            {
                // ! blue
                cl    = -fH * minusScale;
                color = RGB(0, 0, cl);
            }

            if (j == h - 1 || i == w - 1)
            {
                color = RGB(0, 255, 0);
            }


            dc.SetPixel(i, j, color);
        }
        // m_strInfomaiton.AppendFormat(TEXT("\n") );
    }


    dc.SetTextColor(RGB(255, 255, 0));

    m_strInfomaiton.Format(TEXT("W:%d,H:%d,bottom:%f,top:%f"), m_width + 1, m_height + 1, m_bottom, m_nTop);
    dc.DrawText(m_strInfomaiton, rcDraw, DT_TOP | DT_LEFT);
}
