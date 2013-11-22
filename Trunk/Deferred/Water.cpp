#include        "stdafx.h"
#include        "Water.h"
#include        "DeferredSystem.h"

namespace       Ogre
{
WaterManager::WaterManager(DeferredSystem* pDeferredSys, Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport) :
    m_pDeferredSystem(pDeferredSys),
    m_pSceneMgr(pSceneMgr),
    m_pMainCamera(pMainCamera),
    m_pMainViewport(pMainViewport)
{
    Plane plane;
    plane.normal = Vector3::UNIT_Y;
    plane.d      = 0;
    MeshManager::getSingleton().createPlane("WaterMesh",
                                            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                            plane,
                                            100,
                                            100,
                                            10,
                                            10,
                                            true,
                                            1,
                                            60,
                                            60,
                                            Vector3::UNIT_Z);
    m_pWaterPlane = m_pSceneMgr->createEntity("WaterPlane", "WaterMesh");

    pMtlWater = MaterialManager::getSingleton().getByName("WaterPlane");

    Pass*           pPass = NULL;
    pPass = pMtlWater->getTechnique(0)->getPass(0);
    pPass->getTextureUnitState(0)->setTextureName(m_pDeferredSystem->GetDeferredTexDepth()->getName());
    pPass->getTextureUnitState(3)->setTextureName(m_pDeferredSystem->GetDeferredTexDiffuse()->getName());


    //	创建一个临时的水
    m_pWaterPlane->setMaterialName("WaterPlane");
    m_pWaterPlane->setCastShadows(false);
    m_pSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 10, 0))->attachObject(m_pWaterPlane);
    m_pWaterPlane->setRenderQueueGroup(DeferredSystem::RenderQueue_Alpha);

    //	创建一个临时反射纹理
    int             screen_width  = m_pMainViewport->getActualWidth();
    int             screen_height = m_pMainViewport->getActualHeight();
    TextureManager* pTexMgr       = TextureManager::getSingletonPtr();

    m_pReflectTex = TextureManager::getSingleton().createManual("WaterPlane_Reflect",
                                                                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                TEX_TYPE_2D,
                                                                screen_width,
                                                                screen_height,
                                                                0,
                                                                PF_A8R8G8B8,
                                                                TU_RENDERTARGET);

    m_pReflectTex->getBuffer()->getRenderTarget()->setAutoUpdated(false);

    m_pReflectViewport = m_pReflectTex->getBuffer()->getRenderTarget()->addViewport(m_pMainCamera);

    m_pReflectViewport->setClearEveryFrame(true);
    m_pReflectViewport->setOverlaysEnabled(false);
    m_pReflectViewport->setBackgroundColour(ColourValue::White);

    m_pReflectViewport->setMaterialScheme("Reflect");

    RenderQueueInvocationSequence* pSolid = Root::getSingleton().createRenderQueueInvocationSequence("Solid");

    String                         strTemp = "Solid_";
    for ( int index = DeferredSystem::RenderQueue_Avatar; index <= DeferredSystem::RenderQueue_Sky; ++index )
    {
        pSolid->add(index, strTemp + StringConverter::toString(index));
    }

    m_pReflectViewport->setRenderQueueInvocationSequenceName(pSolid->getName());


    //	设置反射纹理
    pPass->getTextureUnitState(4)->setTextureName(m_pReflectTex->getName());
}

void WaterManager::PrepareReflectTexture()
{
    //	检测哪些反射图应该更新



    //	将视锥体内水的反射图生成


    //	临时生成反射图
    Plane plane;
    plane.normal = Vector3::UNIT_Y;
    plane.d      = -10.f - 0.1f;        //	适当升高一点

    m_pMainCamera->enableReflection(plane);
    m_pMainCamera->enableCustomNearClipPlane(plane);

    m_pReflectViewport->setCamera(m_pMainCamera);
    m_pReflectViewport->update();


    m_pMainCamera->disableReflection();
    m_pMainCamera->disableCustomNearClipPlane();
}

void WaterManager::SetWaterReflectFrequency(float height, float frequncy)
{
}

float WaterManager::GetWaterReflectFrequency(float height)
{
    return 0.f;
}

Entity* WaterManager::CreateWater(float height)
{
    Entity*                     pWater = NULL;

    Heiht_ReflectInfo::iterator iter = m_mapHeihgtReflect.find(height);

    if ( iter == m_mapHeihgtReflect.end())
    {
        //	创建新的
    }
    else
    {
        //	返回
    }

    return pWater;
}
}
