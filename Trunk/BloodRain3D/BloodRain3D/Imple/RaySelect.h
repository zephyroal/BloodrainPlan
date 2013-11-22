#include "CommonInclude.h"
bool PickEntity(Ogre::RaySceneQuery* mRaySceneQuery, Ogre::Ray& ray, Ogre::Entity** result, Ogre::Vector3& hitpoint,
                Ogre::uint32 mask = 0, bool excludeInVisible = true, const Ogre::String& excludeobject = "", Ogre::Real max_distance = 10000.0f);

void GetMeshInformationEx(const Ogre::MeshPtr     mesh,
                          size_t&                 vertex_count,
                          Ogre::Vector3*&         vertices,
                          size_t&                 index_count,
                          unsigned long*&         indices,
                          const Ogre::Vector3&    position,
                          const Ogre::Quaternion& orient,
                          const Ogre::Vector3&    scale);

/*使用时用如下代码即可（可能需要根据实际情况有所改动）：
Entity* rayResult=NULL;
Vector3 hitPoint;
CEGUI::Point mousePos= CEGUI::MouseCursor::getSingleton().getPosition();
Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(e.state.width),mousePos.d_y/float(e.state.height));
RaySceneQuery* rayQuery=mSceneMgr->createRayQuery(Ray());
if (PickEntity(rayQuery,mouseRay,&rayResult,QF_moveable_object,hitPoint,true))
        rayResult->getParentSceneNode(); //find a entity*/

// 以上源码参考Ogitor，为了配合自己的程序，稍有改动。为了方便大家，所以贴出来，建议大家直接看Ogitor源码。
