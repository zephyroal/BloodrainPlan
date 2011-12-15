#ifndef AppFrameHead
#define AppFrameHead
#pragma  once
//************************************
// 函数名称: BloodRain3D
// 函数说明：千里之行始于足下
// 作 成 者：何牧
// 加入日期：2011/05/07
//************************************

//20110516 饶了好大一圈

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
