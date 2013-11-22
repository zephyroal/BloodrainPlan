// ---------------------------------------------------------------
// modified or deveoloped by Shen Yuqing
// HUST CS 06
// syq.myth@gmail.com
// 2009
// ---------------------------------------------------------------
// #include <OgreNoMemoryMacros.h>
#include <Windows.h>
#include <CEGUI.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <tchar.h>

#include "ExampleApplication.h"
#include "ExampleFrameListener.h"

#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "ETSplattingManager.h"
#include "TerrainLiquid.h"
using namespace std;
using Ogre::uint;

#include "TinyXml/tinyxml.h"
#include "tinyxml/tinystr.h"
#define   SCENE_NAME  emei

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


class DemoListener : public ExampleFrameListener, public OIS::MouseListener, public OIS::KeyListener
{
public:

    DemoListener(RenderWindow*         win,
                 Camera*               cam,
                 SceneManager*         sceneManager,
                 CEGUI::Renderer*      renderer,
                 ET::TerrainManager*   terrainMgr,
                 ET::SplattingManager* splatMgr)
        : ExampleFrameListener(win, cam, true,
                               true), mGUIRenderer(renderer), mTerrainMgr(terrainMgr), mTerrainInfo(&terrainMgr->getTerrainInfo()),
        mSplatMgr(splatMgr)
    {
        // Setup default variables
        mPointer    = NULL;
        mLMouseDown = false;
        mRMouseDown = false;
        mMMouseDown = false;
        mSceneMgr   = sceneManager;

        // Reduce move speed
        mMoveSpeed    = 320;
        mRotateSpeed *= 0.008;

        // Register this so that we get events.
        mMouse->setEventCallback(this);
        mKeyboard->setEventCallback(this);

        // Create RaySceneQuery
        mRaySceneQuery = mSceneMgr->createRayQuery(Ray());

        // Create a "pointer" for use on the terrain
        Entity* pointer = mSceneMgr->createEntity("Pointer", "ogrehead.mesh");
        mPointer = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mPointer->attachObject(pointer);

        // initialise CEGUI mouse cursor position
        CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(CEGUI::Vector2(0, 0)));

        createEditBrush();
        mDeform        = true;
        mChosenTexture = 0;

        mDirection = Vector3::ZERO;

        mContinue = true;

        mCamNode = mCamera->getParentSceneNode();
    }

    ~DemoListener( )
    {
        delete mRaySceneQuery;
    }


    void createEditBrush()
    {
        // load the edit brush for terrain editing
        Image image;
        image.load("brush.png", "ET");
        image.resize(16, 16);
        mEditBrush = ET::loadBrushFromImage(image);

        mBrush.brushType = ET::TileBrush::POINT;
        mBrush.op        = 0;
        mBrush.imageID   = 0;
        mBrush.layer     = 0;
    }


    bool frameStarted(const FrameEvent& evt)
    {
        if (!ExampleFrameListener::frameStarted(evt))
        {
            return false;
        }

        // handle movement
        mCamNode->translate(mCamera->getOrientation() * mDirection * mMoveSpeed * evt.timeSinceLastFrame);

        // Ensure we stay above terrain (somehow buggy still, doesn't work reliably)
        Vector3 camPos = mCamNode->getPosition( );
        // get terrain height at camPos
        float   terrainHeight = mTerrainInfo->getHeightAt(camPos.x, camPos.z);
        if ((terrainHeight + 30.0f) > camPos.y)
        {
            mCamNode->setPosition(camPos.x, terrainHeight + 30.0f, camPos.z);
        }

        if (mLMouseDown || mRMouseDown)
        {
            // deform or paint terrain on mouse down
            // left button raises, right button lowers
            if (mDeform)
            {
                // choose a brush intensity, this determines
                // how extreme our brush works on the terrain
                float   brushIntensity = evt.timeSinceLastFrame * 0.4 * (mLMouseDown ? 1 : -1);
                // translate our cursor position to vertex indexes
                Vector3 deformPos = mPointer->getPosition();
                // Vector3 offset = mTerrainInfo->getOffset();
                // deformPos+=offset;
                int     x = mTerrainInfo->posToVertexX(deformPos.x);
                int     z = mTerrainInfo->posToVertexZ(deformPos.z);
                // now tell the ETM to deform the terrain
                mTerrainMgr->deform(x, z, mEditBrush, brushIntensity);
            }
            else
            {
                // need to set our brush intensity larger for painting.
                // for painting, all texture channels are only 1 byte
                // large, so with a small intensity you won't get any
                // effect at all.
                float   brushIntensity = evt.timeSinceLastFrame * 5.0 * (mLMouseDown ? 1 : -1);
                // retrieve edit points
                Vector3 paintPos = mPointer->getPosition();
                int     x        = mTerrainInfo->posToVertexX(paintPos.x);
                int     z        = mTerrainInfo->posToVertexZ(paintPos.z);
                // now use the splatting manager to update the coverage maps
                // mSplatMgr->paint(mChosenTexture, x, z, mEditBrush, brushIntensity);

                mBrush.brushType = ET::TileBrush::POINT;
                mBrush.op        = 0;
                mBrush.imageID   = mChosenTexture;
                mBrush.layer     = 0;
                mTerrainMgr->tileSplat(x, z, mBrush);
            }
        }
        return mContinue;
    }


    virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
    {
        // Left mouse button down
        if ( id == OIS::MB_Left )
        {
            mLMouseDown = true;
        }
        // Middle mouse button down
        else if ( id == OIS::MB_Middle )
        {
            CEGUI::MouseCursor::getSingleton().hide( );
            mMMouseDown = true;
        }

        else if (id == OIS::MB_Right)
        {
            mRMouseDown = true;
        }

        return true;
    }

    virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
    {
        // Left mouse button up
        if ( id == OIS::MB_Left )
        {
            mLMouseDown = false;
            // after completed deformation steps, recalculate the lightmap
            if (mDeform)
            {
                updateLightmap();
            }
        }

        // Middle mouse button up
        else if ( id == OIS::MB_Middle )
        {
            CEGUI::MouseCursor::getSingleton().show( );
            mMMouseDown = false;
        }

        else if (id == OIS::MB_Right)
        {
            mRMouseDown = false;
            // after completed deformation steps, recalculate the lightmap
            if (mDeform)
            {
                updateLightmap();
            }
        }

        return true;
    }

    virtual bool mouseMoved(const OIS::MouseEvent& arg)
    {
        // Update CEGUI with the mouse motion
        CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);

        // whenever the mouse is moved, we update the position on the
        // terrain to which the mouse is currently pointing
        Ray                      mouseRay =
            mCamera->getCameraToViewportRay(arg.state.X.abs / float(arg.state.width), arg.state.Y.abs / float(arg.state.height));
        // since ETM is no longer a scene manager, we don't use a ray scene query,
        // but instead query the terrain info directly
        std::pair<bool, Vector3> result = mTerrainInfo->rayIntersects(mouseRay);
        if (result.first)
        {
            // update pointer's position
            mPointer->setPosition(result.second);
        }

        // If we are dragging the middle mouse button.
        if ( mMMouseDown )
        {
            mCamera->yaw(Degree(-arg.state.X.rel * mRotateSpeed));
            mCamera->pitch(Degree(-arg.state.Y.rel * mRotateSpeed));
        }

        return true;
    }


    virtual bool keyPressed(const OIS::KeyEvent& arg)
    {
        // we'll use the keys 1, 2, 3, 4 and E to switch between
        // edit modes and select a paint texture
        // WSAD is used for movement
        // O will save the changes to disk
        switch (arg.key)
        {
        case OIS::KC_1:
            mDeform        = false;
            mChosenTexture = 0;
            return true;
        case OIS::KC_2:
            mDeform        = false;
            mChosenTexture = 1;
            return true;
        case OIS::KC_3:
            mDeform        = false;
            mChosenTexture = 2;
            return true;
        case OIS::KC_4:
            mDeform        = false;
            mChosenTexture = 3;
            return true;
        case OIS::KC_5:
            mDeform        = false;
            mChosenTexture = 4;
            return true;
        case OIS::KC_6:
            mDeform        = false;
            mChosenTexture = 5;
            return true;
        case OIS::KC_E:
            mDeform = true;
            return true;

        case OIS::KC_W:
            mDirection.z += -1;
            return true;
        case OIS::KC_S:
            mDirection.z += 1;
            return true;
        case OIS::KC_A:
            mDirection.x += -1;
            return true;
        case OIS::KC_D:
            mDirection.x += 1;
            return true;

        case OIS::KC_O:
            saveTerrain();
            return true;

        case OIS::KC_P:
            loadTerrain();
            return true;

        case OIS::KC_ESCAPE:
            mContinue = false;
            return true;

        default:
            return false;
        }
    }

    virtual bool keyReleased(const OIS::KeyEvent& arg)
    {
        // not interested in this
        switch (arg.key)
        {
        case OIS::KC_W:
            mDirection.z -= -1;
            return true;
        case OIS::KC_S:
            mDirection.z -= 1;
            return true;
        case OIS::KC_A:
            mDirection.x -= -1;
            return true;
        case OIS::KC_D:
            mDirection.x -= 1;
            return true;

        default:
            return false;
        }
    }


    void saveTerrain()
    {
        // just for demonstration, we'll save anything we can
        // start with the terrain
        Image         image;
        ET::saveHeightmapToImage(*mTerrainInfo, image);
        image.save("../../Media/ET/ETterrain.png");

        // now save the splatting maps
        for (uint i = 0; i < mSplatMgr->getNumMaps(); ++i)
        {
            mSplatMgr->saveMapToImage(i, image);
            image.save("../../Media/ET/ETcoverage." + StringConverter::toString(i) + ".png");
        }

        // now the lightmap
        Image         lightmap;
        ET::createTerrainLightmap(*mTerrainInfo, lightmap, 512, 512, Vector3(1, -1, 1), ColourValue(1, 1, 1),
                                  ColourValue(0.3, 0.3,  0.3));
        lightmap.save("../../Media/ET/ETlightmap.png");

        // generate a base texture for this terrain (could be used for older hardware instead of splatting)
        // create an ImageList of our splatting textures.
        ET::ImageList textures(6);
        for (int i = 0; i < 6; ++i)
        {
            textures[i].load("splatting" + StringConverter::toString(i) + ".png", "ET");
        }
        // create the base texture
        Image         baseTexture;
        mSplatMgr->createBaseTexture(baseTexture, 512, 512, textures, 20, 20);
        baseTexture.save("../../Media/ET/ETbase.png");

        // finally create a minimap using the lightmap and the generated base texture
        Image         minimap = ET::createMinimap(baseTexture, lightmap);
        minimap.save("../../Media/ET/ETminimap.png");
    }


    void loadTerrain()
    {
        // now we revert the process and load the data back from disk
        // recreate terrain...
        mTerrainMgr->destroyTerrain();
        Image               image;
        image.load("ETterrain.png", "ET");
        ET::TerrainInfo     info;
        ET::loadHeightmapFromImage(info, image);

        // --------------------------------------------------------------------
        // todo
        ET::TileTerrainInfo tileTerrainInfo;

        info.setExtents(AxisAlignedBox(0, 0, 0, 1500, 300, 1500));
        mTerrainMgr->createTerrain(info, tileTerrainInfo);
        mTerrainInfo = &mTerrainMgr->getTerrainInfo();

        // now load the splatting maps
        for (uint i = 0; i < mSplatMgr->getNumMaps(); ++i)
        {
            image.load("ETcoverage." + StringConverter::toString(i) + ".png", "ET");
            mSplatMgr->loadMapFromImage(i, image);
        }

        // update the lightmap
        updateLightmap();
    }


    void updateLightmap()
    {
        Image      lightmap;
        ET::createTerrainLightmap(*mTerrainInfo, lightmap, 192, 192, Vector3(1, -1, 1), ColourValue(1, 1, 1),
                                  ColourValue(0.3, 0.3,  0.3));

        // get our dynamic texture and update its contents
        TexturePtr tex = TextureManager::getSingleton().getByName("ETLightmap");
        tex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
    }


protected:
    ET::TileBrush          mBrush;
    RaySceneQuery*         mRaySceneQuery; // The ray scene query pointer
    bool                   mLMouseDown, mRMouseDown; // True if the mouse buttons are down
    bool                   mMMouseDown;
    SceneManager*          mSceneMgr;  // A pointer to the scene manager
    SceneNode*             mPointer;   // Our "pointer" on the terrain
    CEGUI::Renderer*       mGUIRenderer; // cegui renderer
    ET::Brush              mEditBrush; // Brush for terrain editing
    bool                   mDeform;    // stores which mode we are in (deform or paint)
    uint                   mChosenTexture; // which of the four splatting textures is to be used?

    // movement
    Vector3                mDirection;
    bool                   mMove;

    SceneNode*             mCamNode;

    bool                   mContinue;

    ET::TerrainManager*    mTerrainMgr;
    const ET::TerrainInfo* mTerrainInfo;
    ET::SplattingManager*  mSplatMgr;
};


class DemoETSM : public ExampleApplication
{
private:
    String                mSeneName;
    ET::TerrainManager*   mTerrainMgr;
    ET::SplattingManager* mSplatMgr;
protected:
    CEGUI::OgreRenderer*  mGUIRenderer;
    CEGUI::System*        mGUISystem;      // cegui system
public:
    DemoETSM()
    {
        mTerrainMgr = 0;
        mSplatMgr   = 0;
    }

    ~DemoETSM()
    {
        delete mTerrainMgr;
        delete mSplatMgr;
    }
protected:
    void chooseSceneManager(void)
    {
        // Create instance of the Editable Terrain Scene Manager
        mSceneMgr = mRoot->createSceneManager("OctreeSceneManager", "ETInstance");
    }

    void createScene(void)
    {
        // 支持中文路径
        mSeneName = String("emei");
        setlocale(LC_CTYPE, "");

        int                 layerTextureWidth  = ET::SPLICE_COL * ET::SPLICE_IMAGE_SIZE;
        int                 layerTextureHeight = ET::SPLICE_ROW * ET::SPLICE_IMAGE_SIZE;

        TexturePtr          layer0 = TextureManager::getSingletonPtr()->createManual(
            "<layer0>", "General", TEX_TYPE_2D,
            layerTextureWidth, layerTextureHeight, 1, 3, PF_BYTE_RGBA, TU_WRITE_ONLY);

        TexturePtr          ui = TextureManager::getSingletonPtr()->createManual(
            "<TileImagesetTexture>", "General", TEX_TYPE_2D,
            layerTextureWidth, layerTextureHeight, 1, 3, PF_BYTE_RGBA, TU_WRITE_ONLY);
        // layer0和layer1用的相同的纹理。
        MaterialPtr         material(MaterialManager::getSingleton().getByName("Terrain/TwoLayerLightmap"));
        material->getTechnique(0)->getPass(0)->getTextureUnitState(1)->setTextureName("<layer0>");

        // create lightmap
        TexturePtr          layerlightmap = TextureManager::getSingletonPtr()->createManual(
            "<lightmap>", "General", TEX_TYPE_2D, 512, 512, 1, PF_BYTE_RGBA);

        Image               ligthMapImage;
        ligthMapImage.load(mSeneName + ".lightmap.png", "General");
        layerlightmap->getBuffer()->blitFromMemory(ligthMapImage.getPixelBox());

        // create terrain manager
        mTerrainMgr = new ET::TerrainManager(mSceneMgr);
        mTerrainMgr->setLODErrorMargin(2, mCamera->getViewport()->getActualHeight());
        mTerrainMgr->setUseLODMorphing(true, 0.2, "morphFactor");

        // set position and size of the terrain
        ET::TileTerrainInfo tileTerrainInfo;
        // tileTerrainInfo.LoadTerrain( "../../media/TTLB/scene/","shaolin.Terrain" );
        tileTerrainInfo.LoadTerrain("../../media/TTLB/scene/", mSeneName + ".Terrain");

        int                 width  = tileTerrainInfo.width;
        int                 height = tileTerrainInfo.height;

        ET::TerrainInfo     terrainInfo(width + 1, height + 1, tileTerrainInfo.heightMapData);
        int                 x = width * tileTerrainInfo.scale_x;
        int                 z = height * tileTerrainInfo.scale_z;

        // 有的地形需要有个偏移才行,<center>值
        // terrainInfo.setExtents(AxisAlignedBox( -x/2-400, 0, -z/2-200, x/2-400, tileTerrainInfo.scale_y, z/2-200 ) );
        // 少林正常
        terrainInfo.setExtents(AxisAlignedBox(-x / 2, 0, -z / 2, x / 2, tileTerrainInfo.scale_y, z / 2));
        // 由于ETM中只支持高度0.0f-1.0f,所以必须再弄一个包围盒，不然光线追踪出问题，鼠标位置不对。
        terrainInfo.setBounding(AxisAlignedBox(-x / 2, 0, -z / 2, x / 2, 4000, z / 2));

        // now render it
        mTerrainMgr->createTerrain(terrainInfo, tileTerrainInfo, tileTerrainInfo.tileSize + 1, 255, true, false);

        //  createTTLBScene( "../../media/TTLB/scene/shaolin.Scene");
        String              tempsene = String("../../media/TTLB/scene/") + mSeneName;

        createTTLBScene(tempsene + ".Scene"); // 载入景物

        // create the splatting manager
        mSplatMgr = new ET::SplattingManager("ETSplatting", "ET", 192, 192, 3);
        // specify number of splatting textures we need to handle
        mSplatMgr->setNumTextures(6);

        // create a manual lightmap texture
        TexturePtr          lightmapTex = TextureManager::getSingleton().createManual(
            "ETLightmap", "ET", TEX_TYPE_2D, 192, 192, 1, PF_BYTE_RGB);

        Image               lightmap;
        ET::createTerrainLightmap(terrainInfo, lightmap, 192, 192, Vector3(1, -1, 1), ColourValue::White,
                                  ColourValue(0.3, 0.3, 0.3));
        lightmapTex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
        mTerrainMgr->setMaterial(MaterialManager::getSingleton().getByName("ETTerrainMaterial"));
        mTerrainMgr->setTileTerrainMaterial(MaterialManager::getSingleton().getByName("Terrain/TwoLayerLightmap"));

        SceneNode*          camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        camNode->setPosition(750, 800, 800);
        camNode->attachObject(mCamera);
        mCamera->lookAt(750, 0, 750);
        mCamera->yaw(Degree(-45));

        // CEGUI setup
        IniCeGUI();
    }
    void IniCeGUI()
    {
        mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

        CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
        CEGUI::Font::setDefaultResourceGroup("Fonts");
        CEGUI::Scheme::setDefaultResourceGroup("Schemes");
        CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
        CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

        CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");

        CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

        CEGUI::WindowManager& wmgr  = CEGUI::WindowManager::getSingleton();
        CEGUI::Window*        sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

        CEGUI::Window*        quit = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
        quit->setText("Quit");
        quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));

        sheet->addChildWindow(quit);
        CEGUI::System::getSingleton().setGUISheet(sheet);

        quit->subscribeEvent(CEGUI::PushButton::EventClicked,
                             CEGUI::Event::Subscriber(&DemoETSM::quit, this));
    }
    bool quit(const CEGUI::EventArgs& e)
    {
        mRoot->shutdown();
        return true;
    }

    void createFrameListener(void)
    {
        mFrameListener = new DemoListener(mWindow, mCamera, mSceneMgr, mGUIRenderer, mTerrainMgr, mSplatMgr);
        mFrameListener->showDebugOverlay(true);
        mRoot->addFrameListener(mFrameListener);
    }


    void createTTLBScene(const Ogre::String& sceneName)
    {
        int           iLight = 0;
        TiXmlDocument sceneDoc;

        bool          bLoadSuccess = sceneDoc.LoadFile(sceneName);

        if ( !bLoadSuccess )
        {
            OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "load scene file failed", "");
            return;
        }

        const char*   strTemp;
        const char*   sValue;
        // Scene
        TiXmlElement* root = sceneDoc.FirstChildElement();

        // terrain
        TiXmlElement* element = root->FirstChildElement("Terrain");

        strTemp = element->Attribute("filename");

        // load the terrain and insert into scene.
        // ETM::TerrainManager will do the insertion

        while ( 1 )
        {
            element = element->NextSiblingElement();
            strTemp = element->Attribute("type");
            if ( IsStrEqual(strTemp, "StaticEntity"))
            {
                break;
            }

            TiXmlElement* propriety = element->FirstChildElement("Property");
            if ( IsStrEqual(strTemp, "Light"))
            {
                const char*      lightName  = element->Attribute("name");
                Ogre::Light*     pLight     = mSceneMgr->createLight(lightName);
                Ogre::SceneNode* pLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(lightName);

                // add light to scene
                float            r, g, b, x, y, z;
                while ( propriety )
                {
                    strTemp = propriety->Attribute("name");
                    sValue  = propriety->Attribute("value");
                    if ( IsStrEqual("type", strTemp))
                    {
                        if ( IsStrEqual(sValue, "directional"))
                        {
                            pLight->setType(Ogre::Light::LT_DIRECTIONAL);
                        }
                        else if ( IsStrEqual(sValue, "point"))
                        {
                            pLight->setType(Ogre::Light::LT_POINT);
                        }
                        else if ( IsStrEqual(sValue, "spotlight"))
                        {
                            pLight->setType(Ogre::Light::LT_SPOTLIGHT);
                        }
                        else
                        {
                            assert(0);                                  // error
                        }
                    }
                    else if ( IsStrEqual("diffuse", strTemp))
                    {
                        sscanf(sValue, "%f %f %f", &r, &g, &b);
                        pLight->setDiffuseColour(r, g, b);
                    }
                    else if ( IsStrEqual("specular", strTemp))
                    {
                        sscanf(sValue, "%f %f %f", &r, &g, &b);
                        pLight->setSpecularColour(r, g, b);
                    }
                    else if ( IsStrEqual("position", strTemp))
                    {
                        sscanf(sValue, "%f %f %f", &x, &y, &z);
                        pLight->setPosition(x, y, z);
                    }
                    else if ( IsStrEqual("direction", strTemp))
                    {
                        sscanf(sValue, "%f %f %f", &x, &y, &z);
                        pLight->setDirection(x, y, z);
                    }
                    else if ( IsStrEqual("cast shadows", strTemp))
                    {
                        if ( IsStrEqual(sValue, "true"))
                        {
                            pLight->setCastShadows(true);
                        }
                        else
                        {
                            pLight->setCastShadows(false);
                        }
                    }
                    else
                    {
                        assert(0);
                    }

                    propriety = propriety->NextSiblingElement();
                }

                // pLightNode->attachObject( pLight );
                // pLight->setVisible( false );
            }
            else if ( IsStrEqual(strTemp, "Enviroment"))
            {
                // add fog to scene
                Ogre::ColourValue ambient;
                Ogre::ColourValue color;
                Ogre::FogMode     mode;
                int               linerstart = 0, linerend = 0;
                while ( propriety )
                {
                    strTemp = propriety->Attribute("name");
                    sValue  = propriety->Attribute("value");


                    if ( IsStrEqual("ambient", strTemp))
                    {
                        sscanf(sValue, "%f %f %f", &ambient.r, &ambient.g, &ambient.b);
                    }
                    else if ( IsStrEqual("fog.mode", strTemp))
                    {
                        if ( IsStrEqual(sValue, "linear"))
                        {
                            mode = FOG_LINEAR;
                        }
                        else if ( IsStrEqual(sValue, "exp"))
                        {
                            mode = FOG_EXP;
                        }
                        else if ( IsStrEqual(sValue, "exp2"))
                        {
                            mode = FOG_EXP2;
                        }
                        else
                        {
                            mode = FOG_NONE;
                        }
                    }
                    else if ( IsStrEqual("fog.colour", strTemp))
                    {
                        sscanf(sValue, "%f %f %f", &color.r, &color.g, &color.b);
                    }
                    else if ( IsStrEqual("fog.linear start", strTemp))
                    {
                        linerstart = _ttoi(sValue);
                    }
                    else if ( IsStrEqual("fog.linear end", strTemp))
                    {
                        linerend = _ttoi(sValue);
                    }
                    else
                    {
                        assert(0);
                    }

                    propriety = propriety->NextSiblingElement();
                }
                mSceneMgr->setAmbientLight(ambient);
                mSceneMgr->setFog(mode, color, 0.001, linerstart, linerend);
            }

            else if ( IsStrEqual(strTemp, "SkyDome"))
            {
                // add skydome
                while ( propriety )
                {
                    strTemp = propriety->Attribute("name");
                    sValue  = propriety->Attribute("value");
                    if ( IsStrEqual("material", strTemp))
                    {
                        mSceneMgr->setSkyDome(true, sValue);
                    }
                    else
                    {
                        assert(0);
                    }

                    propriety = propriety->NextSiblingElement();
                }
            }

            else
            {
                // const char* Name1 = element->Attribute( "model name");
                // const char* Name2 = element->Attribute( "position");
                assert(0);                 // error!
            }
        }

        static int StaticEntityIndex = 0;
        while ( element )
        {
            strTemp = element->Attribute("type");
            {
                if ( IsStrEqual("StaticEntity", strTemp))
                {
                    SceneNode*    pSsceneNode = NULL;
                    Entity*       pEntity     = NULL;
                    float         w, x, y, z;

                    TiXmlElement* propriety = element->FirstChildElement("Property");

                    while ( propriety )
                    {
                        strTemp = propriety->Attribute("name");
                        sValue  = propriety->Attribute("value");
                        if ( IsStrEqual("mesh name", strTemp))
                        {
                            sValue      = UTF8ToANSI(sValue);
                            pSsceneNode =
                                mSceneMgr->getRootSceneNode()->createChildSceneNode(sValue + StringConverter::toString(StaticEntityIndex++));
                            pEntity = mSceneMgr->createEntity(sValue + StringConverter::toString(StaticEntityIndex++), sValue);
                            pSsceneNode->attachObject(pEntity);

                            delete[]sValue;
                        }
                        else if ( IsStrEqual("position", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &x, &y, &z);
                            pSsceneNode->setPosition(x, y, z);
                        }

                        else if ( IsStrEqual("orientation", strTemp))
                        {
                            sscanf(sValue, "%f %f %f %f", &w, &x, &y, &z);
                            pSsceneNode->setOrientation(w, x, y, z);
                        }
                        else if ( IsStrEqual("scale", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &x, &y, &z);
                            pSsceneNode->setScale(x, y, z);
                        }
                        else if ( IsStrEqual("receive shadows", strTemp))
                        {
                        }
                        else if ( IsStrEqual("receive decals", strTemp))
                        {
                        }
                        else if ( IsStrEqual("animation level", strTemp))
                        {
                        }
                        else if ( IsStrEqual("transparency", strTemp))
                        {
                        }
                        else
                        {
                            assert(0);
                        }

                        propriety = propriety->NextSiblingElement();

                        // insert the static entity into the scene
                    }
                }
                else if ( IsStrEqual("Model", strTemp))
                {
                }
                else if (  IsStrEqual("ParticleSystem", strTemp))
                {
                }
                else if ( IsStrEqual("TerrainLiquid", strTemp))
                {
                    static int     staticIndex    = 1;
                    TerrainLiquid* pTerrainLiquid = new TerrainLiquid;
                    SceneNode*     pSsceneNode    =
                        mSceneMgr->getRootSceneNode()->createChildSceneNode("terrain_liquid" + StringConverter::toString(staticIndex++));
                    TiXmlElement*  propriety = element->FirstChildElement("Property");

                    float          x, y, z;
                    float          texture_scale = 0.0f;
                    float          depth_scale   = 0.0f;
                    bool           depth_enable  = false;
                    while ( propriety )
                    {
                        strTemp = propriety->Attribute("name");
                        sValue  = propriety->Attribute("value");
                        if ( IsStrEqual("material", strTemp))
                        {
                            sValue = UTF8ToANSI(sValue);
                            pTerrainLiquid->setMaterial(sValue);
                            delete[] sValue;
                        }
                        else if ( IsStrEqual("position", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &x, &y, &z);
                            pSsceneNode->setPosition(x, y, z);
                        }
                        else if ( IsStrEqual("texture scale", strTemp))
                        {
                            sscanf(sValue, "%f", &texture_scale);
                        }
                        else if ( IsStrEqual("depth texture layer.enable", strTemp))
                        {
                            if ( IsStrEqual(sValue, "true"))
                            {
                                depth_enable = true;
                            }
                            else
                            {
                                depth_enable = false;
                            }
                        }
                        else if ( IsStrEqual("depth texture layer.height scale", strTemp))
                        {
                            sscanf(sValue, "%f", &depth_scale);
                        }

                        else
                        {
                            break;                            // ThrowException( "TerrainLiquid", strTemp/Files/syqking/TerrainLiquid_src.rar );
                        }
                        propriety = propriety->NextSiblingElement();
                    }

                    pTerrainLiquid->createTerrainLiquid(Ogre::Vector3(x, y, z), texture_scale, depth_enable, depth_scale,
                                                        mTerrainMgr->getTerrainInfo());
                    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pTerrainLiquid);
                }
                else if ( IsStrEqual("Effect", strTemp))
                {
                }
                else if ( IsStrEqual(strTemp, "Light"))
                {
                    TiXmlElement* propriety = element->FirstChildElement("Property");
                    const char*   lightName = element->Attribute("name");
                    Ogre::Light*  pLight;
                    if (lightName != NULL)
                    {
                        pLight = mSceneMgr->createLight(lightName);
                        Ogre::SceneNode* pLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(lightName);
                    }
                    else
                    {
                        iLight++;
                        char             MyStr[32];
                        sprintf(MyStr, "Light_%i", iLight);
                        pLight = mSceneMgr->createLight(MyStr);
                        Ogre::SceneNode* pLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(MyStr);
                    }

                    // add light to scene
                    float         r, g, b, x, y, z;
                    while ( propriety )
                    {
                        strTemp = propriety->Attribute("name");
                        sValue  = propriety->Attribute("value");
                        if ( IsStrEqual("type", strTemp))
                        {
                            if ( IsStrEqual(sValue, "directional"))
                            {
                                pLight->setType(Ogre::Light::LT_DIRECTIONAL);
                            }
                            else if ( IsStrEqual(sValue, "point"))
                            {
                                pLight->setType(Ogre::Light::LT_POINT);
                            }
                            else if ( IsStrEqual(sValue, "spotlight"))
                            {
                                pLight->setType(Ogre::Light::LT_SPOTLIGHT);
                            }
                            else
                            {
                                assert(0);                                      // error
                            }
                        }
                        else if ( IsStrEqual("diffuse", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &r, &g, &b);
                            pLight->setDiffuseColour(r, g, b);
                        }
                        else if ( IsStrEqual("specular", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &r, &g, &b);
                            pLight->setSpecularColour(r, g, b);
                        }
                        else if ( IsStrEqual("position", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &x, &y, &z);
                            pLight->setPosition(x, y, z);
                        }
                        else if ( IsStrEqual("direction", strTemp))
                        {
                            sscanf(sValue, "%f %f %f", &x, &y, &z);
                            pLight->setDirection(x, y, z);
                        }
                        else if ( IsStrEqual("cast shadows", strTemp))
                        {
                            if ( IsStrEqual(sValue, "true"))
                            {
                                pLight->setCastShadows(true);
                            }
                            else
                            {
                                pLight->setCastShadows(false);
                            }
                        }
                        else if ( IsStrEqual("range", strTemp))
                        {
                        }
                        else if ( IsStrEqual("attenuation.linear", strTemp))
                        {
                        }
                        else
                        {
                            // assert(0);
                        }
                        propriety = propriety->NextSiblingElement();
                    }

                    // pLightNode->attachObject( pLight );
                    // pLight->setVisible( false );
                }
                else
                {
                    assert(0);
                }
            }
            element = element->NextSiblingElement();
        }
    }
};


#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char** argv)
#endif
{
    // Create application object
    DemoETSM app;

    try {
        app.go();
    } catch ( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occured: %s\n",
                e.getFullDescription().c_str());
#endif
    }

    return 0;
}
