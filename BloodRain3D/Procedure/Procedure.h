#ifndef ProcedureHead
#define ProcedureHead
// ************************************
// 函数名称: BloodRain3D
// 函数说明：千里之行始于足下
// 作 成 者：何牧
// 加入日期：2011/03/23
// ************************************
/*
为所有Procedure的基类

*/
// 20110511 放弃CeGui，重构，Dare to do！
// 自信 从容 狠硬

#include "CommonInclude.h"
#include "SelectionRectangle.h"
#include "RaySelect.h"

#include <MyGUI.h>
#include "MyGUI_OgrePlatform.h"
#include "InputManager.h"
#include "PointerManager.h"
#include "Base/StatisticInfo.h"
#include "Base/InputFocusInfo.h"
#include "MyGUI_LastHeader.h"
#include "MainPanel.h"
#include "MyInputContext.h"


// 用于监听输入法状态
#pragma  comment(lib,"Imm32.lib")
enum ETQueryFlags
{
    Terrain_MASK     = 1 << 0,
        Objiect_MASK = 1 << 1,
};

enum EditType {ET_Deform = 0, ET_Paint, ET_SceneAdd, ET_SceneEdit};

class CProcedure : public MyFrameListener, public OIS::MouseListener, public OIS::KeyListener, public input::PointerManager
{
public:
    CProcedure(RenderWindow*                  win,
               Camera*                        cam,
               SceneManager*                  sceneManager,
               ET::TerrainManager*            terrainMgr,
               ET::SplattingManager*          splatMgr,
               BloodRainBites::MyTrayManager* trayMgr);
    ~CProcedure( );
    void CreateTray();
    void createEditBrush();
    void RenderQuad(const FrameEvent& evt);
    bool frameStarted(const FrameEvent& evt);
    virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
    virtual bool mouseMoved(const OIS::MouseEvent& arg);
    void performSelection(const Vector2& first, const Vector2& second);
    void deselectObjects();
    void selectObject(MovableObject* obj);
    void SwithMode(EditType newType, bool bChangedFromKeyBoard = false);
    void OnGUITreeItemSelected(MyGUI::TreeControl* pTreeControl, MyGUI::TreeControl::Node* pNode);
    void SetSceneAddType( std::string ObjectName);
    void OnGUISwitchMode();
    virtual bool keyPressed(const OIS::KeyEvent& arg);
    virtual bool keyReleased(const OIS::KeyEvent& arg);
    void saveTerrain();
    void loadTerrain();
    void updateLightmap();
    void shutdown();
    void checkPostion();
    void moveCamera();
    virtual void showDebugOverlay(bool show);
    virtual bool processUnbufferedKeyInput(const FrameEvent& evt);
    virtual bool processUnbufferedMouseInput(const FrameEvent& evt);
    virtual bool frameRenderingQueued(const FrameEvent& evt);
    virtual bool frameEnded(const FrameEvent& evt);
protected:
    std::vector<std::string>                    m_OldMaterialVec;
    std::vector<MaterialPtr>                     m_NewMaterialVec;
    int                                          m_iMtrNum;
    Entity*                                      m_pSelectedEntity;
    SceneNode*                                   m_pSelectedSceneNode;
    int                                          mCursorX;
    int                                          mCursorY;
    std::vector<Ogre::AnimationState*>           m_AnimationStateVec;
    std::vector<Ogre::AnimationState*>::iterator iter;
    int                                          m_iLayer;
    ET::TileBrush                                mBrush;
    std::string                                 mNewEntityName;
    BloodRainBites::Button*                      mButton;
    BloodRainBites::TextBox*                     mHelpText;
    BloodRainBites::ParamsPanel*                 mDetailsPanel;
    BloodRainBites::MyTrayManager*               mTrayMgr;
    bool                                         m_bShowHelp;
    int                                          m_iCount;
    AnimationState*                              mAnimationState;
    bool                                         m_bQuit;
    SceneNode*                                   mCurrentObject;
    int                                          m_iFrame, m_iLastFrame, m_iUpadteCounts;
    RaySceneQuery*                               mRaySceneQuery; // The ray scene query pointer
    bool                                         mLMouseDown, mRMouseDown, mLShiftDown, mLCtrlDown, mLAltDown, mLCaptial; // True if the mouse buttons are down
    bool                                         mMMouseDown;
    SceneManager*                                mSceneMgr; // A pointer to the scene manager
    SceneNode*                                   mPointer; // Our "pointer" on the terrain

    ET::Brush                                    mEditBrush; // Brush for terrain editing
    EditType                                     m_EditType, m_OldEditType;
    uint                                         mChosenTexture; // which of the four splatting textures is to be used?
    // movement
    Vector3                                      mDirection;
    bool                                         mMove;
    SceneNode*                                   mCamNode;
    ET::TerrainManager*                          mTerrainMgr;
    const ET::TerrainInfo*                       mTerrainInfo;
    ET::SplattingManager*                        mSplatMgr;
    Vector2                                      mStart, mStop;
    PlaneBoundedVolumeListSceneQuery*            mVolQuery;
    std::list<MovableObject*>                    mSelected;
    SelectionRectangle*                          mRect;
    bool                                         mSelecting;
    static void swap(float& x, float& y)
    {
        float tmp = x;
        x = y;
        y = tmp;
    }
public:
    void CreateMyGUI();
    const std::string& getRootMedia();
    void setupMyGUIResources();
    void addResourceLocation(const std::string& _name, const std::string& _group, const std::string& _type, bool _recursive);

    void addResourceLocation(const std::string& _name, bool _recursive = false);
    void destroyGui();
    void shutdownMyGUI();
    void createMyGUIScene();
    void destroyMyGUIScene();
    bool injectMouseMove(int _absx, int _absy, int _absz);
    bool injectMousePress(int _absx, int _absy, MyGUI::MouseButton _id);
    bool injectMouseRelease(int _absx, int _absy, MyGUI::MouseButton _id);
    bool injectKeyPress(MyGUI::KeyCode _key, MyGUI::Char _text);
    bool injectKeyRelease(MyGUI::KeyCode _key);
    void notifyEventAction(MainPanel::TypeEvents _action, size_t _index);
private:
    MainPanel*                  mMainPanel;
    bool                        mExit;
    MyGUI::Gui*                 mGUI;
    MyGUI::OgrePlatform*        mPlatform;
    diagnostic::StatisticInfo*  mInfo;
    diagnostic::InputFocusInfo* mFocusInfo;
    MyGUI::Widget*              mMainWidget;
    std::string                mPrefix;
    std::string                mLayoutName;
    typedef std::vector<wraps::BaseLayout*> VectorBasePtr;
    MyGUI::VectorWidgetPtr      mListWindowRoot;
    VectorBasePtr               mListBase;
    std::string                mResourcePath;
    std::string                mPluginCfgName;
    std::string                mResourceXMLName;
    std::string                mResourceFileName;
    std::string                mRootMedia;
    Ogre::SceneNode*            mNode;
protected:
    virtual void updateStats(void)
    {
        static std::string currFps  = "Current FPS: ";
        static std::string avgFps   = "Average FPS: ";
        static std::string bestFps  = "Best FPS: ";
        static std::string worstFps = "Worst FPS: ";
        static std::string tris     = "Triangle Count: ";
        static std::string batches  = "Batch Count: ";

        // update stats when necessary
        try {
            OverlayElement*                 guiAvg   = OverlayManager::getSingleton().getOverlayElement("BloodRain/AverageFps");
            OverlayElement*                 guiCurr  = OverlayManager::getSingleton().getOverlayElement("BloodRain/CurrFps");
            OverlayElement*                 guiBest  = OverlayManager::getSingleton().getOverlayElement("BloodRain/BestFps");
            OverlayElement*                 guiWorst = OverlayManager::getSingleton().getOverlayElement("BloodRain/WorstFps");

            const RenderTarget::FrameStats& stats = mWindow->getStatistics();
            guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
            guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
            guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
                                + " " + StringConverter::toString(stats.bestFrameTime) + " ms");
            guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
                                 + " " + StringConverter::toString(stats.worstFrameTime) + " ms");

            OverlayElement*                 guiTris = OverlayManager::getSingleton().getOverlayElement("BloodRain/NumTris");
            guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

            OverlayElement*                 guiBatches = OverlayManager::getSingleton().getOverlayElement("BloodRain/NumBatches");
            guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

            OverlayElement*                 guiDbg = OverlayManager::getSingleton().getOverlayElement("BloodRain/DebugText");
            guiDbg->setCaption(mDebugText);
        }
        catch (...)
        {
        }
    }
};
#endif
