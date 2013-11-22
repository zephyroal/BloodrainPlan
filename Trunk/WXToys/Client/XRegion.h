#pragma once

class XRegion : public FileInfoInterface
{
public:
    XRegion(void);
    ~XRegion(void);

    virtual BOOL LoadFile(CONST CString& strFile)
    {
        LoadRegionBinary(strFile);
        return TRUE;
    }


    VOID                    LoadRegionBinary(LPCTSTR szRegionFile);

    // Region¡¥±Ì
    std::vector< CRegion > m_theRegion;
    fVector2               m_maxVec;

    const vector< CRegion >& GetRegion() const;
    const fVector2&        GetMaxVec();

    void Draw(CDC& dc, int width, int height);
};
