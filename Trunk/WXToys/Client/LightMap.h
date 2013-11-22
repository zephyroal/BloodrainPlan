#pragma once



#define string string

class LightMap : public FileInfoInterface
{
public:
    LightMap(void);
    ~LightMap(void);

    void _loadLightmap(const string& filename);
    BOOL LoadFile(CONST CString& strFile)
    {
        _loadLightmap((LPCTSTR)strFile);
        return TRUE;
    }
    void Draw(CDC& dc, int width, int height);


    auto_ptr<Image > m_image;
    // Image* m_image;
};
