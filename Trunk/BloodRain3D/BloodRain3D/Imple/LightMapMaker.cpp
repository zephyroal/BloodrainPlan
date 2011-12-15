#include "LightMapMaker.h"   
/*
#include "GameScene/TerrainData.h"   
#include "GameScene/WXScene.h"   
#include "GameScene/WXSceneInfo.h"   
#include "GameScene/WXStaticEntityObject.h"   
#include "GameScene/WXModelObject.h"   

namespace WX   
{   
	//----------------------------------------------------------------------------------   
	LightMapMaker::LightMapMaker(SceneManipulator* manipulator)   
		: mSceneManipulator(manipulator)   
		, mRenderTexture(0)   
	{   

	}   

	//--------------------------------------------------------------------------------   
	LightMapMaker::~LightMapMaker(void)   
	{   

	}   

	//--------------------------------------------------------------------------------   
	void LightMapMaker::initial(void)   
	{   
		// 确保地形已经载入   
		TerrainData* data = mSceneManipulator->getTerrainData();   
		assert(data && "TerrainData* == NULL");   

		// 创建lightmap摄像机,垂直俯瞰整个地图   
		Ogre::SceneManager* mSceneManager = mSceneManipulator->getSceneManager();   
		//if (mSceneManipulator->getTerrainData()->mLightmapImage == NULL)   
		{   
			static const String msLightmapMaker("Lightmap maker");   
			Ogre::Camera* camera = 0;   
			try  
			{   
				camera = mSceneManager->getCamera(msLightmapMaker);   
			}   
			catch (const Ogre::Exception& e)   
			{   
				// 只提示异常,而不退出程序   
				if (e.getNumber() == e.ERR_ITEM_NOT_FOUND)   
				{   
					//MessageBox(NULL, e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);   
				}   
				else  
				{   
					throw;   
				}   

			}   
			if (!camera)   
			{   
				camera = mSceneManager->createCamera(msLightmapMaker);   
				camera->setAutoAspectRatio(true);   
				camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC); // 平行投影   
				// 在Ogre1.6版本中,平行投影的大小改为setOrthoWindowWidth和setOrthoWindowHeight来决定,   
				// 而不是以前版本用的FOV和近截面,so...   
				camera->setFOVy(Ogre::Degree(90));   
				camera->setOrthoWindowWidth(mSceneManipulator->getTerrainData()->getXSize()*mSceneManipulator->getBaseScale());   
				camera->setOrthoWindowHeight(mSceneManipulator->getTerrainData()->getZSize()*mSceneManipulator->getBaseScale());   

				Ogre::Quaternion orientation;   
				orientation.FromAxes(Ogre::Vector3::UNIT_X, Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Vector3::UNIT_Y);   
				camera->setOrientation(orientation);   
			}   

			// 计算所有对象包围盒,以此来决定摄像机的参数   
			Ogre::AxisAlignedBox aabb;   
			Ogre::SceneManager::MovableObjectIterator itm =   
				mSceneManager->getMovableObjectIterator(Ogre::EntityFactory::FACTORY_TYPE_NAME);   
			while (itm.hasMoreElements())   
			{   
				Ogre::MovableObject* movable = itm.getNext();   
				aabb.merge(movable->getWorldBoundingBox(true));   
			}   
			camera->setNearClipDistance(32 * mSceneManipulator->getBaseScale() / 2);    
			camera->setFarClipDistance(camera->getNearClipDistance() + aabb.getMaximum().y - aabb.getMinimum().y);   
			camera->setPosition(0, camera->getNearClipDistance() + aabb.getMaximum().y, 0);   

			// 调整阴影参数   
			Ogre::Real distance = camera->getNearClipDistance() * Ogre::Math::Sqrt(1 + Ogre::Math::Sqr(camera->getAspectRatio()));   
			mSceneManager->setShadowFarDistance(distance);   
			Ogre::Real camHeight = camera->getPosition().y;   
			mSceneManager->setShadowDirLightTextureOffset(camHeight / distance);   


			// 创建lightmap的Rtt纹理,和天龙一样,设为地形大小的8倍   
			Ogre::TexturePtr pTexture = (Ogre::TexturePtr)Ogre::TextureManager::getSingleton().getByName("LightmapRttTex");   
			if (pTexture.isNull())   
			{   
				size_t width = data->getXSize() * 8;   
				size_t height = data->getZSize() * 8;   
				pTexture = Ogre::TextureManager::getSingleton().createManual(   
					"LightmapRttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,   
					Ogre::TEX_TYPE_2D,   
					width, height, 1, 0, Ogre::PF_BYTE_RGB,   
					Ogre::TU_RENDERTARGET, 0);   
			}   
			Ogre::HardwarePixelBufferSharedPtr pBuffer = pTexture->getBuffer(0, 0);   
			mRenderTexture = pBuffer->getRenderTarget(0);     

			// 创建lightmap的视口   
			Ogre::Viewport* viewport = mRenderTexture->addViewport(mSceneManipulator->getSceneManager()->getCamera("Lightmap maker"));   
			viewport->setOverlaysEnabled(false);   
			viewport->setSkiesEnabled(false);   
			viewport->setShadowsEnabled(true);   
			// 自定义一个渲染队列调用组,只渲染地形队列,这是关键,我们只要阴影,不要其他的.   
			// 如果你全部都渲染了,那就变成了minimap,缩略图了.   
			Ogre::RenderQueueInvocationSequence* rqis =   
				Ogre::Root::getSingleton().createRenderQueueInvocationSequence("Lightmap maker");   
			Ogre::RenderQueueInvocation* rqi =   
				rqis->add(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1, "World Geometry");   
			viewport->setRenderQueueInvocationSequenceName("Lightmap maker");   
		}   

	}   
	//--------------------------------------------------------------------------------   
	void LightMapMaker::destroy(void)   
	{   
		if (mRenderTexture)   
		{   
			mSceneManipulator->getSceneManager()->destroyCamera("Lightmap maker");   

			mRenderTexture->removeAllViewports();   

			Ogre::TextureManager::getSingleton().remove("LightmapRttTex");   

			mRenderTexture = NULL;   
		}   
	}   

	//--------------------------------------------------------------------------------   
	void LightMapMaker::outputTexture(const Ogre::String& fileName)   
	{   
		// 如果已经有lightmap了,跳过   
		if (mSceneManipulator->getTerrainData()->mLightmapImage)    
		{   
			// 需要提示么?   
			return;   
		}   

		// 让所有静态物体都可以投射阴影,来制作lightmap   
		Scene::ObjectsByTypeRange objests = mSceneManipulator->getSceneInfo()->findObjectsByType(StaticEntityObject::msType);   
		for (Scene::ObjectsByTypeIterator it = objests.first; it != objests.second; ++ it)   
		{   
			ObjectPtr object = (*it);   
			object->setPropertyAsString("cast shadows", "true");   
			//object->setPropertyAsString("receive shadows", "true");   
		}   
		objests = mSceneManipulator->getSceneInfo()->findObjectsByType(ModelObject::msType);   
		for (Scene::ObjectsByTypeIterator it = objests.first; it != objests.second; ++ it)   
		{   
			ObjectPtr object = (*it);   
			object->setPropertyAsString("cast shadows", "true");   
			//object->setPropertyAsString("receive shadows", "true");   
		}   

		// 用BaseWhite代替原来的地形材质,并重构地形   
		TerrainData::MaterialTemplates& materials = mSceneManipulator->getTerrainData()->mMaterialTemplates;   
		TerrainData::MaterialTemplates materialsCopy(materials);   
		materials["OneLayer"] = "BaseWhite";   
		materials["OneLayerLightmap"] = "BaseWhite";   
		materials["TwoLayer"] = "BaseWhite";   
		materials["TwoLayerLightmap"] = "BaseWhite";   
		mSceneManipulator->getTerrain()->buildGeometry(mSceneManipulator->getBaseSceneNode(), true);   

		// 更新   
		//mSceneManipulator->getSceneManager()->_updateSceneGraph(mSceneManipulator->getSceneManager()->getCamera("Lightmap maker"));   
		mRenderTexture->update();   

		// 保存lightmap到文件   
		Ogre::String LightmapFileName(fileName);   
		String baseName, path, outExtention;   
		if (fileName.empty())   
		{   
			WX::TerrainData* data = mSceneManipulator->getTerrainData();   
			Ogre::StringUtil::splitBaseFilename(data->mHeightmapFilename, baseName, outExtention);   
			LightmapFileName = baseName + ".lightmap.png";   
		}   
		mRenderTexture->writeContentsToFile(LightmapFileName);   

		// 载入lightmap   
		static const String TEMP_GROUP_NAME = "#TEMP#"; // 临时资源组   
		Ogre::StringUtil::splitFilename(LightmapFileName, baseName, path);   
		mSceneManipulator->getTerrainData()->mLightmapFilename = baseName;   
		Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();   
		rgm.addResourceLocation(path, "FileSystem", TEMP_GROUP_NAME, false);   
		// 这个函数本来是protected,以后最好还是重新载入整个地形,而不是单独载入lightmap   
		mSceneManipulator->getTerrainData()->_loadLightmap(baseName, "image", TEMP_GROUP_NAME);    

		// 换回原来的地形材质,并重构地形   
		std::swap(materials, materialsCopy);   
		mSceneManipulator->getTerrain()->buildGeometry(mSceneManipulator->getBaseSceneNode(), true);   

		// 所有静态物体不再需要投射阴影了,因为我们有lightmap   
		objests = mSceneManipulator->getSceneInfo()->findObjectsByType(StaticEntityObject::msType);   
		for (Scene::ObjectsByTypeIterator it = objests.first; it != objests.second; ++ it)   
		{   
			ObjectPtr object = (*it);   
			object->setPropertyAsString("cast shadows", "false");   
			//object->setPropertyAsString("receive shadows", "true");   
		}   
		objests = mSceneManipulator->getSceneInfo()->findObjectsByType(ModelObject::msType);   
		for (Scene::ObjectsByTypeIterator it = objests.first; it != objests.second; ++ it)   
		{   
			ObjectPtr object = (*it);   
			object->setPropertyAsString("cast shadows", "false");   
			//object->setPropertyAsString("receive shadows", "true");   
		}   

		// 销毁临时资源组   
		rgm.destroyResourceGroup(TEMP_GROUP_NAME);   
	}   


}  
*/