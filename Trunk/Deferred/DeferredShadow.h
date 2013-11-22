#pragma once
#include        "stdafx.h"
#include        "DeferredSystem.h"

class DeferredShadow
{
public:
    struct  ShadowInfo
    {
        Ogre::TexturePtr pShadowTex;
        Ogre::Camera*    pShadowCamera;
        Ogre::Viewport*  pShadowViewport;
    };

public:
    DeferredShadow(DeferredSystem* pDeferredSys, Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport);
    ~DeferredShadow();

    //	准备阴影深度图
    void                    PrepareShadow();


    Ogre::Texture*  GetTexCastShadow();

protected:
    void    _parserShadowLightInfo();
    void    _ensureShadowInfo();

private:
    DeferredSystem*                     m_pDeferredSys;
    Ogre::Rectangle2D*                  m_pShadowQuad;
    Ogre::MaterialPtr                   m_pMtlShadowQuad;

    Ogre::GpuProgramParametersSharedPtr m_pParameterVP;
    Ogre::GpuProgramParametersSharedPtr m_pParameterFP;

    Ogre::SceneManager*                 m_pSceneMgr;
    Ogre::Camera*                       m_pMainCamera;
    Ogre::Viewport*                     m_pMainViewport;


    Ogre::Light*                        m_pSunLight;
    ShadowInfo                          m_ShadowInfo;

    Ogre::FocusedShadowCameraSetup      m_ShadowCameraSetup;
};
