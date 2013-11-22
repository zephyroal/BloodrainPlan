#include "HydraxListener.h"

// Hydrax pointer
Hydrax::Hydrax*               mHydrax = 0;

std::string                  mSkyBoxes[_def_SkyBoxNum] =
{"Sky/ClubTropicana",
 "Sky/EarlyMorning",
 "Sky/Clouds"};

Ogre::Vector3                 mSunPosition[_def_SkyBoxNum] =
{ Ogre::Vector3(0.0f, 10000.0f, 0.0f),
  Ogre::Vector3(0.0f, 10000.0f, 90000),
  Ogre::Vector3(0.0f, 10000.0f, 0.0f) };

Ogre::Vector3                 mSunColor[_def_SkyBoxNum] =
{ Ogre::Vector3(1.0f, 0.9f, 0.6f),
  Ogre::Vector3(1.0f, 0.6f, 0.4f),
  Ogre::Vector3(0.45f, 0.45f, 0.45f)};

int                           mCurrentSkyBox = 0;

// Just to wshow skyboxes information
Ogre::TextAreaOverlayElement* mTextArea = 0;



CHydraxListener::CHydraxListener(RenderWindow* win, Camera* cam, SceneManager* sm)
    : MyFrameListener(win, cam, true, true)
    , mSceneMgr(sm)
    , mKeyBuffer(-1)
{
    mSceneMgr = sm;
    mCamera   = cam;
    mWindow   = win;
    m_bPause  = true;
    createScene();
    mHydrax->setVisible(!m_bPause);
    // 要响应Buffered输入 1，继承OIS：Key 2，FrameRenderQue 里·capturekeyboard 3，CallBackThis
    mKeyboard->setEventCallback(this);
}
// Just override the mandatory create scene method
void CHydraxListener::createScene(void)
{
    // Set default ambient light
    mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));

    // Create the SkyBox
    mSceneMgr->setSkyBox(true, mSkyBoxes[mCurrentSkyBox], 99999 * 3, true);

    // Set some camera params
    mCamera->setFarClipDistance(99999 * 6);
    mCamera->setPosition(312.902f, 206.419f, 1524.02f);
    mCamera->setOrientation(Ogre::Quaternion(0.998f, -0.0121f, -0.0608f, -0.00074f));

    // Light
    Ogre::Light*                   mLight = mSceneMgr->createLight("Light0");
    mLight->setPosition(mSunPosition[mCurrentSkyBox]);
    mLight->setDiffuseColour(1, 1, 1);
    mLight->setSpecularColour(mSunColor[mCurrentSkyBox].x,
                              mSunColor[mCurrentSkyBox].y,
                              mSunColor[mCurrentSkyBox].z);

    // Hydrax initialization code ---------------------------------------------
    // ------------------------------------------------------------------------

    // Create Hydrax object
    mHydrax = new Hydrax::Hydrax(mSceneMgr, mCamera, mWindow->getViewport(0));

    // Create our projected grid module
    Hydrax::Module::ProjectedGrid* mModule
        = new Hydrax::Module::ProjectedGrid(        // Hydrax parent pointer
        mHydrax,
        // Noise module
        new Hydrax::Noise::Perlin(/*Generic one*/),
        // Base plane
        Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
        // Normal mode
        Hydrax::MaterialManager::NM_VERTEX,
        // Projected grid options
        Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

    // Set our module
    mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

    // Load all parameters from config file
    // Remarks: The config file must be in Hydrax resource group.
    // All parameters can be set/updated directly by code(Like previous versions),
    // but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
    mHydrax->loadCfg("HydraxDemo.hdx");

    // Create water
    mHydrax->create();

    // Hydrax initialization code end -----------------------------------------
    // ------------------------------------------------------------------------

    // Load island
    // mSceneMgr->setWorldGeometry("Island.cfg");

    mHydrax->getMaterialManager()->addDepthTechnique(
        static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"))
        ->createTechnique());

    // Create text area to show skyboxes information
    createTextArea();
}

// Create text area to show skyboxes information
void CHydraxListener::createTextArea()
{
    // Create a panel
    Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(
        OverlayManager::getSingleton().createOverlayElement("Panel", "HydraxDemoInformationPanel"));
    panel->setMetricsMode(Ogre::GMM_PIXELS);
    panel->setPosition(10, 10);
    panel->setDimensions(400, 400);
    // Create a text area
    mTextArea = static_cast<Ogre::TextAreaOverlayElement*>(
        OverlayManager::getSingleton().createOverlayElement("TextArea", "HydraxDemoInformationTextArea"));
    mTextArea->setMetricsMode(Ogre::GMM_PIXELS);
    mTextArea->setPosition(0, 0);
    mTextArea->setDimensions(100, 100);
    mTextArea->setCharHeight(16);
    mTextArea->setCaption("Hydrax 0.5.1 demo application\nCurrent water preset: "  + Ogre::StringUtil::split(mSkyBoxes[mCurrentSkyBox],
                                                                                                             "/")[1] + " (" +
                          Ogre::StringConverter::toString(mCurrentSkyBox + 1) + "/3). Press 'm' to switch water presets.");
    mTextArea->setFontName("BlueHighway");
    mTextArea->setColourBottom(ColourValue(0.3f, 0.5f, 0.3f));
    mTextArea->setColourTop(ColourValue(0.5f, 0.7f, 0.5f));
    // Create an overlay, and add the panel

    Ogre::Overlay* overlay = OverlayManager::getSingleton().getByName("BloodRainTrayMgr/WidgetsLayer");
    overlay->add2D(panel);
    // Add the text area to the panel
    panel->addChild(mTextArea);
    // Show the overlay
    // overlay->show();
    // showDebugOverlay(false);
}
bool CHydraxListener::mouseMoved(const OIS::MouseEvent& arg)
{
    return true;
}
/*
bool CHydraxListener::frameRenderingQueued(const FrameEvent& evt)
{

        return true;
}*/

bool CHydraxListener::keyPressed(const OIS::KeyEvent& arg)
{
    switch (arg.key)
    {
    case OIS::KC_F12:
        m_bPause = !m_bPause;
        mHydrax->setVisible(!m_bPause);
        break;
    case OIS::KC_M:
        {
            mCurrentSkyBox++;
            if (mCurrentSkyBox > (_def_SkyBoxNum - 1))
            {
                mCurrentSkyBox = 0;
            }
            changeSkyBox();
            break;
        }
    }
    return true;
}
bool CHydraxListener::keyReleased(const OIS::KeyEvent& arg)
{
    return true;
}

bool CHydraxListener::frameStarted(const FrameEvent& e)
{
    if (m_bPause)
    {
        return true;
    }
    // Check camera height
    Ogre::RaySceneQuery*                raySceneQuery =
        mSceneMgr->
        createRayQuery(Ogre::Ray(mCamera->getPosition() + Ogre::Vector3(0, 1000000, 0),
                                 Vector3::NEGATIVE_UNIT_Y));
    Ogre::RaySceneQueryResult&          qryResult = raySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator i         = qryResult.begin();
    if (i != qryResult.end() && i->worldFragment)
    {
        if (mCamera->getPosition().y < i->worldFragment->singleIntersection.y + 30)
        {
            mCamera->
            setPosition(mCamera->getPosition().x,
                        i->worldFragment->singleIntersection.y + 30,
                        mCamera->getPosition().z);
        }
    }
    delete raySceneQuery;
    // Update Hydrax
    mHydrax->update(e.timeSinceLastFrame);
    // Check for skyboxes switch
    mKeyBuffer -= e.timeSinceLastFrame;
    return true;
}

void CHydraxListener::changeSkyBox()
{
    // Change skybox
    mSceneMgr->setSkyBox(true, mSkyBoxes[mCurrentSkyBox], 99999 * 3, true);

    // Update Hydrax sun position and colour
    mHydrax->setSunPosition(mSunPosition[mCurrentSkyBox]);
    mHydrax->setSunColor(mSunColor[mCurrentSkyBox]);

    // Update light 0 light position and colour
    mSceneMgr->getLight("Light0")->setPosition(mSunPosition[mCurrentSkyBox]);
    mSceneMgr->getLight("Light0")->setSpecularColour(mSunColor[mCurrentSkyBox].x, mSunColor[mCurrentSkyBox].y, mSunColor[mCurrentSkyBox].z);

    // Update text area
    mTextArea->setCaption("Hydrax 0.5.1 demo application\nCurrent water preset: "  + Ogre::StringUtil::split(mSkyBoxes[mCurrentSkyBox],
                                                                                                             "/")[1] + " (" +
                          Ogre::StringConverter::toString(mCurrentSkyBox + 1) + "/3). Press 'm' to switch water presets.");

    // Log
    LogManager::getSingleton().logMessage("Skybox " + mSkyBoxes[mCurrentSkyBox] + " selected. (" + Ogre::StringConverter::toString(
                                              mCurrentSkyBox + 1) + "/" + Ogre::StringConverter::toString(_def_SkyBoxNum) + ")");
}
