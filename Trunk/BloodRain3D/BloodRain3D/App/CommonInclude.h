//************************************
// 函数名称: BloodRain3D
// 函数说明：千里之行始于足下
// 作 成 者：何牧
// 加入日期：2011/03/23
//************************************
//在此放置常用的头文件
#include <tchar.h>
#include "Windows.h"
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "ExampleFrameListener.h"

#include "ExampleApplication.h"
#include <SdkTrays.h>
#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "ETSplattingManager.h"
#include "tinyxml.h"
#include "tinystr.h"

// Sample sub-class for ListboxTextItem that auto-sets the selection brush
// image.  This saves doing it manually every time in the code.
using namespace std;
//using Ogre::uint;

/*
extern CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);
extern CEGUI::utf8* Unicode_to_utf8(const TCHAR* pMbcs);
extern CEGUI::utf8* mbcs_to_utf8(const char * pMbcs);
extern std::string& decode(const CEGUI::String& str);*/

extern char* UTF8ToANSI(const char* str);
extern wchar_t * UTF8ToUnicode( const char* str );
extern char * UnicodeToANSI( const wchar_t* str );
extern bool IsStrEqual( const char* str1, const char* str2 );