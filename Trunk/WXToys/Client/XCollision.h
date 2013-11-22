#pragma once



typedef struct _POINT_2D
{
    INT iX;
    INT iY;
}POINT_2D, * PPOINT_2D;

class Point_2D_less
{
public:
    bool operator()(const POINT_2D& point1, const POINT_2D& point2) const
    {
        if (point1.iX < point2.iX)
        {
            return true;
        }
        else if (point1.iX == point2.iX)
        {
            if (point1.iY < point2.iY)
            {
                return true;
            }
        }

        return false;
    }
};

typedef struct _POINT_3D
{
    _POINT_3D()
    {
        fx = 0;
        fy = 0;
        fz = 0;
    }
    FLOAT fx;
    FLOAT fy;
    FLOAT fz;
}POINT_3D, * PPOINT_3D;


typedef struct _TRI_INFO
{
    POINT_3D v1;
    POINT_3D v2;
    POINT_3D v3;
}TRI_INFO, * PTRI_INFO;

// �������б���Ϣ��
typedef std::vector<TRI_INFO> TRI_INFO_VECTOR;

// ��ͼ��ע����������б���λ��ӳ���
typedef std::map<POINT_2D, TRI_INFO_VECTOR, Point_2D_less> TRI_INFO_MAP;


class XCollision : public FileInfoInterface
{
public:
    XCollision(void);
    ~XCollision(void);
    UINT LoadCollisionTriInfoFromFile(const char* szFileName);

    // �ڵ�ͼ��ע���������ӳ���
    TRI_INFO_MAP m_triInfoMap;

    BOOL LoadFile(CONST CString& strFile)
    {
        LoadCollisionTriInfoFromFile((LPCTSTR)strFile);
        return TRUE;
    }


    void Draw(CDC& dc, int width, int height);
};
