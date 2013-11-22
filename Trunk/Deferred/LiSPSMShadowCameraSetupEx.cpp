#include        "stdafx.h"
#include        "LiSPSMShadowCameraSetupEx.h"


namespace Ogre
{
LiSPSMShadowCameraSetupEx::LiSPSMShadowCameraSetupEx()
{
}

void LiSPSMShadowCameraSetupEx::getShadowCamera(const SceneManager* sm,
                                                const Camera*       cam,
                                                const Viewport*     vp,
                                                const Light*        light,
                                                Camera*             texCam,
                                                size_t              iteration) const
{
    LiSPSMShadowCameraSetup::getShadowCamera(sm, cam, vp, light, texCam, iteration);

    const Matrix4& matView = cam->getViewMatrix(true);
    const Matrix4& matProj = cam->getProjectionMatrix();

    Matrix4        matViewProj = matView * matViewProj;

    //	通知更新延迟阴影中的矩阵
    if ( m_DeferredShadowFPParamter->_findNamedConstantDefinition("matShadowViewProj"))
    {
        m_DeferredShadowFPParamter->setNamedConstant("matShadowViewProj", matViewProj);
    }
}
}
