#pragma once


class WXTerrain : public FileInfoInterface
{
    typedef struct
    {
        float left;
        float right;
        float top;
        float bottom;
        int   id;
    } PixMap;


    static void fixFloat(float& f)
    {
        if ( f < 0.01f )
        {
            f = 0.005f;
        }
        else if ( f > 0.24f && f < 0.25f )
        {
            f = 0.245f;
        }
        else if ( f > 0.25f && f < 0.26f )
        {
            f = 0.255f;
        }
        else if ( f > 0.49f && f < 0.50f )
        {
            f = 0.495f;
        }
        else if ( f > 0.5f && f < 0.51f )
        {
            f = 0.505f;
        }
        else if ( f > 0.74f && f < 0.75f )
        {
            f = 0.745f;
        }
        else if ( f > 0.75f && f < 0.76f )
        {
            f = 0.755f;
        }
        else if ( f > 0.99f )
        {
            f = 0.995f;
        }
    }


public:
    WXTerrain(void);
    virtual ~WXTerrain(void);


    BOOL LoadFile(CONST CString& strFile)
    {
        LoadTerrain(strFile);
        return TRUE;
    }

    void LoadTerrain(CONST CString& strFile);


    void Draw(CDC& dc, int width, int height);



    static bool IsStrEqual(const char* str1, const char* str2)
    {
        return (strcmp(str1, str2) == 0) ? true : false;
    }


    char* UnicodeToANSI(const wchar_t* str)
    {
        char* result;
        int   textlen;
        textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
        result  = (char*)malloc((textlen + 1) * sizeof(char));
        memset(result, 0, sizeof(char) * (textlen + 1));
        WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
        return result;
    }


    wchar_t* UTF8ToUnicode(const char* str)
    {
        int      textlen;
        wchar_t* result;
        textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
        result  = (wchar_t*)malloc((textlen + 1) * sizeof(wchar_t));
        memset(result, 0, (textlen + 1) * sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
        return result;
    }


    char* UTF8ToANSI(const char* str)
    {
        wchar_t* temp = UTF8ToUnicode(str);
        char*    res  = UnicodeToANSI(temp);
        delete[]temp;
        return res;
    }


    // !
    // !<texture filename="08大理/大理方砖地.tga" type="image"/>
    // typedef std::pair< CString , CString > ImgInfo;
    vector< CString >   m_imgs;


    std::vector<PixMap> pixMapArray;

public:
    int                 width;
    int                 height;
    int                 tileSize;
    int                 scale_x, scale_y, scale_z;
};
