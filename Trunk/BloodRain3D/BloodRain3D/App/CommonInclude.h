//************************************
// ��������: BloodRain3D
// ����˵����ǧ��֮��ʼ������
// �� �� �ߣ�����
// �������ڣ�2011/03/23
//************************************
//�ڴ˷��ó��õ�ͷ�ļ�
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