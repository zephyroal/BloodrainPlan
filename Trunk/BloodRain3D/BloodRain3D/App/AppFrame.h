#ifndef AppFrameHead
#define AppFrameHead
#pragma  once
//************************************
// ��������: BloodRain3D
// ����˵����ǧ��֮��ʼ������
// �� �� �ߣ�����
// �������ڣ�2011/05/07
//************************************

//20110516 ���˺ô�һȦ

#include "BaseAppFrame.h"
#include "TerrainLiquid.h"
#include "Procedure.h"
#include "HydraxListener.h"

#define   SCENE_NAME  emei
class CBloodRain3DApp : public CExampleApplication
{
private:
	ET::TerrainManager* mTerrainMgr;
	ET::SplattingManager* mSplatMgr;
	String mSeneName;
public:
	CBloodRain3DApp();
	~CBloodRain3DApp();
protected:
	bool configure(void);
	void chooseSceneManager(void);
	void createScene(void);
	void createTTLBScene(const Ogre::String& sceneName );
	void createFrameListener(void);
	// method to perform any required cleanup operations.
	void cleanupSample ();
};
#endif
