#pragma once
#include        "stdafx.h"


class DeferredSystem;

namespace       Ogre
{
class WaterManager
{
public:
    struct  ReflectInfo
    {
        TexturePtr pReflectTex;
        Viewport*  pReflectViewport;
        float      frequency;
    };

    typedef std::vector< Entity* >                          WaterList;
    typedef std::map< float, ReflectInfo >          Heiht_ReflectInfo;

public:
    WaterManager(DeferredSystem* pDeferredSys, SceneManager* pSceneMgr, Camera* pMainCamera, Viewport* pMainViewport);

    void            PrepareReflectTexture();

    void            SetWaterReflectFrequency(float height, float frequncy);
    float           GetWaterReflectFrequency(float height);

    Entity*         CreateWater(float height);

private:
    DeferredSystem*   m_pDeferredSystem;
    SceneManager*     m_pSceneMgr;
    Camera*           m_pMainCamera;
    Viewport*         m_pMainViewport;
    WaterList         m_WaterList;
    MaterialPtr       pMtlWater;


    Heiht_ReflectInfo m_mapHeihgtReflect;


    //	¡Ÿ ±≤‚ ‘
    Entity*           m_pWaterPlane;
    TexturePtr        m_pReflectTex;
    Viewport*         m_pReflectViewport;
};
}
