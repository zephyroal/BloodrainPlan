#pragma once

class Dijkstra : public FileInfoInterface
{
public:
    Dijkstra(void);
    ~Dijkstra(void);

    BOOL LoadFile(CONST CString& strFile)
    {
        LoadDijk(strFile);

        return TRUE;
    }
    VOID LoadDijk(const CString strFile);
    std::vector<fVector2> m_Points;

    FLOAT*                m_Weights;
    FLOAT*                m_Dist;

    void Draw(CDC& dc, int width, int height);
};
