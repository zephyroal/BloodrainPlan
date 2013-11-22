#include "CommonInclude.h"
/*
std::string& decode(const CEGUI::String& str)
{
        char sdes[2048];
        static std::string result;
        int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        wchar_t* wcstr = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wcstr, len);
        int wlen = WideCharToMultiByte(CP_ACP, 0, wcstr, -1, NULL, 0, NULL, NULL);
        char* nstr = new char[wlen];
        WideCharToMultiByte(CP_ACP, 0, wcstr, -1, nstr, wlen, NULL, NULL);
        strcpy(sdes, nstr);
        delete[] wcstr;
        delete[] nstr;
        result = sdes;
        return result;
}
CEGUI::utf8* mbcs_to_utf8(const char * pMbcs)
{
        static CEGUI::utf8 g_buf[1024]={0};
        static wchar_t g_Unicode[1024]={0};
        memset(g_Unicode,0,sizeof(g_Unicode));
        memset(g_buf,0,sizeof(g_buf));
        //首先将GBK的数据转化为Unicode数据
        MultiByteToWideChar(936,0,pMbcs,strlen(pMbcs),g_Unicode,1024);
        //然后将Unicode数据转化为utf8数据
        WideCharToMultiByte(CP_UTF8,0,g_Unicode,wcslen(g_Unicode),(char*)g_buf,1024,0,0);
        //最后返回utf8数据
        return g_buf;
}

CEGUI::utf8* Unicode_to_utf8(const TCHAR* pMbcs)
{
        static CEGUI::utf8 g_buf[1024]={0};
        memset(g_buf,0,sizeof(g_buf));
        //然后将Unicode数据转化为utf8数据
        WideCharToMultiByte(CP_UTF8,0,pMbcs,wcslen(pMbcs),(char*)g_buf,1024,0,0);
        //最后返回utf8数据
        return g_buf;
}

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
        switch (buttonID)
        {
        case OIS::MB_Left:
                return CEGUI::LeftButton;
                break;
        case OIS::MB_Right:
                return CEGUI::RightButton;
                break;
        case OIS::MB_Middle:
                return CEGUI::MiddleButton;
                break;
        default:
                return CEGUI::LeftButton;
                break;
        }
}
*/

bool IsStrEqual(const char* str1, const char* str2)
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
