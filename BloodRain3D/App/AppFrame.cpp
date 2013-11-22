#include "AppFrame.h"

CBloodRain3DApp::CBloodRain3DApp() :
    mHydraxEnable(true)
{
    mTerrainMgr = NULL;
    mSplatMgr   = NULL;
}

CBloodRain3DApp::~CBloodRain3DApp()
{
    delete mTerrainMgr;
    delete mSplatMgr;
}

bool CBloodRain3DApp::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if (!mRoot->restoreConfig())
    {
        if (!mRoot->showConfigDialog())
        {
            return false;
        }
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
    }
    mWindow = mRoot->initialise(true, "Zephyr-地形编辑器——路漫漫兮");
    return true;
}

void CBloodRain3DApp::chooseSceneManager(void)
{
    // Create instance of the Editable Terrain Scene Manager
    mSceneMgr = mRoot->createSceneManager("OctreeSceneManager", "ZephyrTerrain");

    if (mOverlaySystem)
    {
        mSceneMgr->addRenderQueueListener(mOverlaySystem);
    }
}

void CBloodRain3DApp::createFrameListener(void)
{
    // 完成初始化工作后就把任务丢给主Procedure负责操作交互
    mFrameListener = new CProcedure(mWindow, mCamera, mSceneMgr, mTerrainMgr, mSplatMgr, mTrayMgr);
    mFrameListener->showDebugOverlay(true);
    mRoot->addFrameListener(mFrameListener);
    // HydraX，水体效果监听器
    if (mHydraxEnable)
    {
        mRoot->addFrameListener(new CHydraxListener(mWindow, mCamera, mSceneMgr));
    }
}

void CBloodRain3DApp::createScene(void)
{
    // 设置阴影
    // set shadow properties
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
    mSceneMgr->setShadowTextureSize(2048);
    mSceneMgr->setShadowColour(ColourValue(0.6f, 0.6f, 0.6f));
    mSceneMgr->setShadowTextureCount(2);
    mSceneMgr->setShowDebugShadows(false);
    mSceneMgr->setShadowFarDistance(3000);

    // LiSPSMShadowCameraSetup* mLiSPSMSetup;
    // mLiSPSMSetup = new LiSPSMShadowCameraSetup();
    // mLiSPSMSetup->setUseAggressiveFocusRegion(false);
    // ShadowCameraSetupPtr mCurrentShadowCameraSetup = ShadowCameraSetupPtr(mLiSPSMSetup);
    // mSceneMgr->setShadowCameraSetup(mCurrentShadowCameraSetup);

    // mSceneMgr->setShadowTextureCasterMaterial(CUSTOM_CASTER_MATERIAL);
    // mSceneMgr->setShadowTextureReceiverMaterial(CUSTOM_RECEIVER_MATERIAL);
    mSceneMgr->setShadowTextureSelfShadow(false);
    /*
    mSceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
            mSceneMgr->setShadowTechnique(Ogre::SHADOWDETAILTYPE_TEXTURE);
            mSceneMgr->setShadowTextureSelfShadow(true);
            mSceneMgr->setShadowTextureCasterMaterial("Ogre/DepthShadowmap/Caster/Float");
            mSceneMgr->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Receiver/Float");*/

	mSceneDir  = std::string("../../media/scene/");
    mSceneName = std::string("zhulin");

    int                 layerTextureWidth  = ET::SPLICE_COL * ET::SPLICE_IMAGE_SIZE;
    int                 layerTextureHeight = ET::SPLICE_ROW * ET::SPLICE_IMAGE_SIZE;
    // Atlas纹理
    TexturePtr          layer0 = TextureManager::getSingletonPtr()->createManual(
        "<layer0>", "General", TEX_TYPE_2D,
        layerTextureWidth, layerTextureHeight, 1, 3, PF_BYTE_RGBA, TU_WRITE_ONLY);

    // 小Atlas，用于渲染在UI界面之上，方面操作
    TexturePtr          ui = TextureManager::getSingletonPtr()->createManual(
        "<TileImagesetTexture>", "General", TEX_TYPE_2D,
        layerTextureWidth, 128, 1, 3, PF_BYTE_RGBA, TU_WRITE_ONLY);
    // layer0和layer1用的相同的纹理。
    MaterialPtr         material(MaterialManager::getSingleton().getByName("Terrain/TwoLayerLightmap"));
    material->getTechnique(0)->getPass(0)->getTextureUnitState(1)->setTextureName("<layer0>");
    // create terrain manager
    mTerrainMgr = new ET::TerrainManager(mSceneMgr);
    mTerrainMgr->setLODErrorMargin(2, mCamera->getViewport()->getActualHeight());
    mTerrainMgr->setUseLODMorphing(true, 0.2f, "morphFactor");
    // set position and size of the terrain
    ET::TileTerrainInfo tileTerrainInfo;
    // 创建高度，光度图，及相应使用的纹理大图
	tileTerrainInfo.LoadTerrain( mSceneDir, mSceneName + ".Terrain");
    int                 width  = tileTerrainInfo.width;
    int                 height = tileTerrainInfo.height;
    ET::TerrainInfo     terrainInfo(width + 1, height + 1, tileTerrainInfo.heightMapData);
    int                 x = width * tileTerrainInfo.scale_x;
    int                 z = height * tileTerrainInfo.scale_z;
    // 有的地形需要有个偏移才行,<center>值，正常不需设置
    // terrainInfo.setExtents(AxisAlignedBox( -x/2-400, 0, -z/2-200, x/2-400, tileTerrainInfo.scale_y, z/2-200 ) );
    terrainInfo.setExtents(AxisAlignedBox(-x / 2, 0, -z / 2, x / 2, tileTerrainInfo.scale_y, z / 2));
    // 由于ETM中只支持高度0.0f-1.0f,所以必须再弄一个包围盒，不然光线追踪出问题，鼠标位置不对。
    terrainInfo.setBounding(AxisAlignedBox(-x / 2, 0, -z / 2, x / 2, 4000, z / 2));
    // 根据地形高度图，纹理列表创建相应的地形
    mTerrainMgr->createTerrain(terrainInfo, tileTerrainInfo, tileTerrainInfo.tileSize + 1, 255, true, false);
    // 创建相应地图景物
	createTTLBScene( mSceneDir+mSceneName + ".Scene");
    // create the splatting manager
    mSplatMgr = new ET::SplattingManager("ETSplatting", "ET", 192, 192, 3);
    // specify number of splatting textures we need to handle
    mSplatMgr->setNumTextures(6);
    // create a manual lightmap texture
    TexturePtr          lightmapTex = TextureManager::getSingleton().createManual(
        "ETLightmap", "ET", TEX_TYPE_2D, 192, 192, 1, PF_BYTE_RGB);
    Image               lightmap;
    ET::createTerrainLightmap(terrainInfo, lightmap, 192, 192, Vector3(1, -1, 1), ColourValue::White,
                              ColourValue(0.3f, 0.3f, 0.3f));
    lightmapTex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
    mTerrainMgr->setMaterial(MaterialManager::getSingleton().getByName("ETTerrainMaterial"));
    mTerrainMgr->setTileTerrainMaterial(MaterialManager::getSingleton().getByName("Terrain/TwoLayerLightmap"));
    SceneNode*          camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    camNode->setPosition(750, 800, 800);
    camNode->attachObject(mCamera);
    mCamera->lookAt(750, 0, 750);
    mCamera->yaw(Degree(-45));
}

void CBloodRain3DApp::createTTLBScene(const std::string& sceneName)
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
                        assert(0);                              // error
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
            // pLight->setVisible( true );
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
                    linerstart = atoi(sValue);
                }
                else if ( IsStrEqual("fog.linear end", strTemp))
                {
                    linerend = atoi(sValue);
                }
                else
                {
                    assert(0);
                }

                propriety = propriety->NextSiblingElement();
            }
            mSceneMgr->setAmbientLight(ambient);
            mSceneMgr->setFog(mode, color, 0.001f, linerstart, linerend);
        }

        else if ( IsStrEqual(strTemp, "SkyDome"))
        {
            if (!mHydraxEnable )
            {
                // add skydome
                while (propriety)
                {
                    strTemp = propriety->Attribute("name");
                    sValue  = propriety->Attribute("value");
                    if (IsStrEqual("material", strTemp))
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
        }

        else
        {
            // const char* Name1 = element->Attribute( "model name");
            // const char* Name2 = element->Attribute( "position");
            assert(0);             // error!
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
						sValue = UTF8ToANSI(sValue);
                        pSsceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(sValue + StringConverter::toString(StaticEntityIndex++));
                        pEntity     = mSceneMgr->createEntity(sValue + StringConverter::toString(StaticEntityIndex++), sValue);
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
                        // assert(0);
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
                        break;                        // ThrowException( "TerrainLiquid", strTemp/Files/syqking/TerrainLiquid_src.rar );
                    }
                    propriety = propriety->NextSiblingElement();
                }

                // pTerrainLiquid->createTerrainLiquid( Ogre::Vector3( x,y,z ), texture_scale, depth_enable, depth_scale, mTerrainMgr->getTerrainInfo() );
                // mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pTerrainLiquid);
            }
            else if ( IsStrEqual("Effect", strTemp))
            {
            }
            else if ( IsStrEqual(strTemp, "Light"))
            {
                TiXmlElement*    propriety  = element->FirstChildElement("Property");
                const char*      lightName  = element->Attribute("name");
                Ogre::Light*     pLight     = NULL;
                Ogre::SceneNode* pLightNode = NULL;
                if (lightName != NULL)
                {
                    pLight     = mSceneMgr->createLight(lightName);
                    pLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(lightName);
                }
                else
                {
                    iLight++;
                    char MyStr[32];
                    sprintf(MyStr, "Light_%i", iLight);
                    pLight     = mSceneMgr->createLight(MyStr);
                    pLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(MyStr);
                }

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

                pLightNode->attachObject(pLight);
                pLight->setVisible(true);
            }
            else if (IsStrEqual(strTemp, "Enviroment"))
            {
                TiXmlElement* propriety = element->FirstChildElement("Property");
                const char*   lightName = element->Attribute("name");
            }
            else
            {
                TiXmlElement* propriety = element->FirstChildElement("Property");
                const char*   lightName = element->Attribute("name");
                // assert(0);
            }
        }
        element = element->NextSiblingElement();
    }
}

// method to perform any required cleanup operations.
void CBloodRain3DApp::cleanupSample()
{
    // me? cleanup? what?
}
