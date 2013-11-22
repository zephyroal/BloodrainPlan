#pragma once
#include        "stdafx.h"
#include        "DeferredSystem.h"

class DeferredShadow
{
public:
    // shadowMap存于此
    struct  ShadowMapInfo
    {
        Ogre::TexturePtr pShadowMapTex;
        Ogre::Camera*    pShadowMapCamera;
        Ogre::Viewport*  pShadowMapViewport;
    };
public:
    DeferredShadow(DeferredSystem* pDeferredSys, Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport);
    ~DeferredShadow();
    //	准备阴影深度图
    void                                            PrepareShadowMap();
    Ogre::Texture*                          GetShadowMap();

protected:
    void    _parserShadowLightInfo();
    void    _ensureShadowInfo();
private:
    DeferredSystem*                m_pDeferredSys;
    Ogre::SceneManager*            m_pMainSceneMgr;
    Ogre::Camera*                  m_pMainCamera;
    Ogre::Viewport*                m_pMainViewport;
    Ogre::Light*                   m_pSunLight;
    ShadowMapInfo                  m_ShadowInfo;
    Ogre::FocusedShadowCameraSetup m_ShadowCameraSetup;
};
