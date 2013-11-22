#pragma once


class WXNavmap : public FileInfoInterface
{
#define MAX_ROUTES 1024 * 2
#define EIGHT_DIRECTIONS 8

    struct _D2XZLOOKUP
    {
        WORD  costmultiplier;
        short zx;
        BYTE  route;
    } DZX[EIGHT_DIRECTIONS];

    struct AIROUTE
    {
        WORD count;        // ������0ʱ,�������ݲ�������

        // ������ʼ��
        WORD walkpoint;
        UINT startzx;
        UINT endzx;

        BYTE route[MAX_ROUTES];
    };

    struct _WORLD
    {
        BYTE state : 4;
        BYTE route : 4;
    };

    struct _NODES
    {
        WORD f;
        WORD g;

        UINT zx;
    };

    struct _NAVMAP_HEAD
    {
        WORD width;
        WORD height;
    };


    enum
    {
        EMPTY_NODE     = 0,
        MIN_NODE       = 1,
        NO_ROUTE       = EIGHT_DIRECTIONS,
        MAX_HEAP_LEAFS = MAX_ROUTES,
    };

    enum
    {
        UNKNOWN    = 0,
        IMPASSABLE = 1,
        OPEN       = 20,
        CLOSED     = 30,
    };

    enum
    {
        FINDING    = 0,
        NO_PATH    = 1,
        PATH_FOUND = 2,
    };
    enum
    {
        ROOT_HEAP = 1
    };

    _WORLD* mWorld;
    _WORLD* mWorkWorld;
    _NODES* mNodes;


    // ����ͼ�ĺ��������ĳߴ�
    INT     mWidth;
    INT     mHeight;
    // һ�����ӵĳߴ�
    FLOAT   mGridSize;
    FLOAT   mInvGridSize;
    // ��׼��λ��
    FLOAT   mLeftTopx;
    FLOAT   mLeftTopz;

    WORD    mBestFNode;
    WORD    mFreeNode;
    INT     mMaxNode;


public:
    WXNavmap(void);
    virtual ~WXNavmap(void);


    BOOL LoadFile(CONST CString& strFile)
    {
        int mx = 0, mz = 0;
        ReadNavMap(strFile, mx, mz);

        return TRUE;
    }

    void ReadNavMap(const CHAR* filename, int& mx, int& mz);

    void Draw(CDC& dc, int width, int height);


    // ! INFO �ļ�ֵ
    int m_max;
};
