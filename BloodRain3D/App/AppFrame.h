#ifndef AppFrameHead
#define AppFrameHead
#pragma  once
// ************************************
// ��������: BloodRain3D
// ����˵����ǧ��֮��ʼ������
// �� �� �ߣ�����
// �������ڣ�2011/05/07
// ************************************

// 20110516 ���˺ô�һȦ

#include "MyAppFrame.h"
#include "TerrainLiquid.h"
#include "Procedure.h"
#include "HydraxListener.h"

#define   SCENE_NAME  yangliucun

class CBloodRain3DApp : public CMyApplication
{
private:
    ET::TerrainManager*   mTerrainMgr;
    ET::SplattingManager* mSplatMgr;
    std::string           mSceneName;
    std::string           mSceneDir;

    bool                  mHydraxEnable;
public:
    CBloodRain3DApp();
    ~CBloodRain3DApp();
protected:
    bool configure(void);
    void chooseSceneManager(void);
    void createScene(void);
    void createTTLBScene(const std::string& sceneName);
    void createFrameListener(void);
    // method to perform any required cleanup operations.
    void cleanupSample ();
};
#endif
