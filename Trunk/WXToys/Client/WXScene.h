#pragma once

class WXScene : public FileInfoInterface
{
    static bool IsStrEqual(const char* str1, const char* str2)
    {
        return (strcmp(str1, str2) == 0) ? true : false;
    }

public:
    WXScene(void);
    ~WXScene(void);

    BOOL LoadFile(CONST CString& strFile)
    {
        LoadTTLBScene((LPCTSTR)strFile);
        return TRUE;
    }

    void LoadTTLBScene(const char* sceneName);



    void Draw(CDC& dc, int width, int height);
};
