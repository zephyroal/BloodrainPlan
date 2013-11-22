#include "OgreSceneManager.h"
namespace Ogre {
class PSSMShadowListener : public SceneManager::Listener
{
    Ogre::Light*               light;
    Ogre::ShadowCameraSetupPtr setup;
    Ogre::Camera*              view_camera; // NOT shadow camera!
    Ogre::SceneManager*        sceneMgr;
    mutable int                split_index;
public:
    struct lightsLess
    {
        bool operator()(const Light* l1, const Light* l2) const
        {
            if (l1 == l2)
            {
                return false;
            }
            return l1->tempSquareDist < l2->tempSquareDist;
        }
    };

    PSSMShadowListener(Ogre::SceneManager* sm, Ogre::Light* l, Ogre::ShadowCameraSetupPtr s, Ogre::Camera* cam)
    {
        sceneMgr    = sm;
        light       = l;
        setup       = s;
        view_camera = cam;
        split_index = 0;
    }

    virtual ~PSSMShadowListener()
    {
    }

    virtual void shadowTexturesUpdated(size_t numberOfShadowTextures)
    {
    }

    virtual void shadowTextureCasterPreViewProj(Ogre::Light* light, Ogre::Camera* camera)
    {
        static bool  update       = true;
        static float split_dist[] = {0.1, 10, 10, 70, 70, 500, 0, 0};
        float        old_near     = view_camera->getNearClipDistance();
        float        old_far      = view_camera->getFarClipDistance();
        if (split_index > 0)
        {
            view_camera->setNearClipDistance(split_dist[2 * split_index]);
        }
        view_camera->setFarClipDistance(split_dist[2 * split_index + 1]);
        if (update)
        {
            setup->getShadowCamera(sceneMgr, view_camera, NULL, light, camera);
        }
        view_camera->setNearClipDistance(old_near);
        view_camera->setFarClipDistance(old_far);
        split_index++;
        if (split_index > 2)
        {
            split_index = 0;
        }
    }

    virtual void shadowTextureReceiverPreViewProj(Ogre::Light* light, Ogre::Frustum* frustum)
    {
    }

    virtual bool sortLightsAffectingFrustum(Ogre::LightList& lightList)
    {
        std::stable_sort(
            lightList.begin(), lightList.end(),
            lightsLess());
        return true;
    }
};
}
