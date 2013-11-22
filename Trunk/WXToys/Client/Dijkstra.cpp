#include "StdAfx.h"
#include "Dijkstra.h"

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)    if ((x) != NULL ) { delete[] (x); (x) = NULL; }
#endif

Dijkstra::Dijkstra(void)
    : m_Weights(0)
    , m_Dist(0)
{
}

Dijkstra::~Dijkstra(void)
{
}


VOID Dijkstra::LoadDijk(const CString strFile)
{
    m_strInfomaiton = strFile;

    string   strFileName = strFile;
    FILE*    pFile       = fopen(strFileName.c_str(), "rb");

    if (NULL == pFile)
    {
        // 不能打开文件.
        return;
    }
    // 的版本信息
    int      iVerLen = (int)sizeof("DIJKSTRA_EDIT_01");
    char*    pVer    = new char[iVerLen];
    ::fread(pVer, 1, iVerLen, pFile);
    delete[] pVer;
    INT      dwPoint = 0;
    fVector3 position;
    ::fread(&dwPoint, sizeof(dwPoint), 1, pFile);
    fVector2 v;
    // 1. 读出所有的点
    m_Points.clear();
    for ( INT dw = 0; dw < dwPoint; dw++ )
    {
        ::fread(&position.x, sizeof(position.x), 1, pFile);
        ::fread(&position.z, sizeof(position.z), 1, pFile);
        ::fread(&position.y, sizeof(position.y), 1, pFile);
        v.x = position.x;
        v.y = position.z;
        m_Points.push_back(v);
    }
    if ( m_Points.size() == 0 )
    {
        return;
    }

    // 2. 建立表
    SAFE_DELETE_ARRAY(m_Weights);
    m_Weights = new FLOAT[(m_Points.size() + 2) * (m_Points.size() + 2)];
    int    cntWeights = (m_Points.size() + 2) * (m_Points.size() + 2);
    ZeroMemory(m_Weights, sizeof(float) * cntWeights);


    int    cnt1 = sizeof(m_Weights);
    int    cnt2 = sizeof(FLOAT);


    // memset( m_Weights, 0, (m_Points.size()+2)*(m_Points.size()+2)*sizeof(FLOAT) );
    float* JuLi = new float[dwPoint * dwPoint];
    ZeroMemory(JuLi, sizeof(float) * dwPoint * dwPoint);

    ::fread(JuLi, sizeof(float), dwPoint * dwPoint, pFile);

    // 3. 计算路径权重
    size_t s = 0;
    for ( s = 0; s < m_Points.size() - 1; s++ )
    {
        for ( size_t t = s + 1; t < m_Points.size(); t++ )
        {
            // 填入表中
            float w = JuLi[s + t * dwPoint];

            m_Weights[(s + 1) + (t + 1) * (m_Points.size() + 2)] = w;
            m_Weights[(t + 1) + (s + 1) * (m_Points.size() + 2)] = w;
        }
        m_Weights[(s + 1) + (s + 1) * (m_Points.size() + 2)] = 0.0f; // 自己到自己权重是 0
    }
    delete[] JuLi;
    for (int i = 0; i < dwPoint + 2; ++i)
    {
        for (int j = 0; j < dwPoint + 2; ++j)
        {
            if (i == 0 || i == dwPoint + 1 || j == 0 || j == dwPoint + 1)
            {
                m_Weights[j + i * (dwPoint + 2)] = FLT_MAX;
            }
        }
    }
    m_Weights[(s + 1) + (s + 1) * (m_Points.size() + 2)] = 0.0f;
    m_Weights[(s + 2) + (s + 2) * (m_Points.size() + 2)] = 0.0f; // 自己到自己权重是 0
    m_Weights[0]                                         = 0.0f;

    // 4. 初始化最短路径
    SAFE_DELETE_ARRAY(m_Dist);
    m_Dist = new FLOAT[m_Points.size() + 2];   // 最短路径保存处


    // ! debug info
    int cntPoints = m_Points.size();

    m_strInfomaiton.AppendFormat("\n---------- points count :%d ----------\n", cntPoints);
    for (int i = 0; i < cntPoints; ++i )
    {
        const fVector2& vec = m_Points[i];
        m_strInfomaiton.AppendFormat("(%f,%f)", vec.x, vec.y);
    }

    // int cntWeights = sizeof(m_Weights)/sizeof(FLOAT);

    m_strInfomaiton.AppendFormat("\n---------- weights count :%d ----------\n(", cntWeights);
    for (int i = 0; i < cntWeights; ++i )
    {
        const FLOAT& w = m_Weights[i];
        m_strInfomaiton.AppendFormat("%3.3g,", w);
        if (i % 8 == 7)
        {
            m_strInfomaiton.AppendFormat("\n");
        }
    }
    m_strInfomaiton.AppendFormat(")\n-----------------------\n ");
}





void Dijkstra::Draw(CDC& dc, int width, int height)
{
    FileInfoInterface::Draw(dc, width, height);
}
