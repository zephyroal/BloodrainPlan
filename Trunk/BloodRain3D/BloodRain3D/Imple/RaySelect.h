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

/*ʹ��ʱ�����´��뼴�ɣ�������Ҫ����ʵ����������Ķ�����
Entity* rayResult=NULL;
Vector3 hitPoint;
CEGUI::Point mousePos= CEGUI::MouseCursor::getSingleton().getPosition();
Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(e.state.width),mousePos.d_y/float(e.state.height));
RaySceneQuery* rayQuery=mSceneMgr->createRayQuery(Ray());
if (PickEntity(rayQuery,mouseRay,&rayResult,QF_moveable_object,hitPoint,true))
        rayResult->getParentSceneNode(); //find a entity*/

// ����Դ��ο�Ogitor��Ϊ������Լ��ĳ������иĶ���Ϊ�˷����ң�������������������ֱ�ӿ�OgitorԴ�롣
