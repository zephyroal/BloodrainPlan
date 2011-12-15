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
		// ȷ�������Ѿ�����   
		TerrainData* data = mSceneManipulator->getTerrainData();   
		assert(data && "TerrainData* == NULL");   

		// ����lightmap�����,��ֱ���������ͼ   
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
				// ֻ��ʾ�쳣,�����˳�����   
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
				camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC); // ƽ��ͶӰ   
				// ��Ogre1.6�汾��,ƽ��ͶӰ�Ĵ�С��ΪsetOrthoWindowWidth��setOrthoWindowHeight������,   
				// ��������ǰ�汾�õ�FOV�ͽ�����,so...   
				camera->setFOVy(Ogre::Degree(90));   
				camera->setOrthoWindowWidth(mSceneManipulator->getTerrainData()->getXSize()*mSceneManipulator->getBaseScale());   
				camera->setOrthoWindowHeight(mSceneManipulator->getTerrainData()->getZSize()*mSceneManipulator->getBaseScale());   

				Ogre::Quaternion orientation;   
				orientation.FromAxes(Ogre::Vector3::UNIT_X, Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Vector3::UNIT_Y);   
				camera->setOrientation(orientation);   
			}   

			// �������ж����Χ��,�Դ�������������Ĳ���   
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

			// ������Ӱ����   
			Ogre::Real distance = camera->getNearClipDistance() * Ogre::Math::Sqrt(1 + Ogre::Math::Sqr(camera->getAspectRatio()));   
			mSceneManager->setShadowFarDistance(distance);   
			Ogre::Real camHeight = camera->getPosition().y;   
			mSceneManager->setShadowDirLightTextureOffset(camHeight / distance);   


			// ����lightmap��Rtt����,������һ��,��Ϊ���δ�С��8��   
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

			// ����lightmap���ӿ�   
			Ogre::Viewport* viewport = mRenderTexture->addViewport(mSceneManipulator->getSceneManager()->getCamera("Lightmap maker"));   
			viewport->setOverlaysEnabled(false);   
			viewport->setSkiesEnabled(false);   
			viewport->setShadowsEnabled(true);   
			// �Զ���һ����Ⱦ���е�����,ֻ��Ⱦ���ζ���,���ǹؼ�,����ֻҪ��Ӱ,��Ҫ������.   
			// �����ȫ������Ⱦ��,�Ǿͱ����minimap,����ͼ��.   
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
		// ����Ѿ���lightmap��,����   
		if (mSceneManipulator->getTerrainData()->mLightmapImage)    
		{   
			// ��Ҫ��ʾô?   
			return;   
		}   

		// �����о�̬���嶼����Ͷ����Ӱ,������lightmap   
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

		// ��BaseWhite����ԭ���ĵ��β���,���ع�����   
		TerrainData::MaterialTemplates& materials = mSceneManipulator->getTerrainData()->mMaterialTemplates;   
		TerrainData::MaterialTemplates materialsCopy(materials);   
		materials["OneLayer"] = "BaseWhite";   
		materials["OneLayerLightmap"] = "BaseWhite";   
		materials["TwoLayer"] = "BaseWhite";   
		materials["TwoLayerLightmap"] = "BaseWhite";   
		mSceneManipulator->getTerrain()->buildGeometry(mSceneManipulator->getBaseSceneNode(), true);   

		// ����   
		//mSceneManipulator->getSceneManager()->_updateSceneGraph(mSceneManipulator->getSceneManager()->getCamera("Lightmap maker"));   
		mRenderTexture->update();   

		// ����lightmap���ļ�   
		Ogre::String LightmapFileName(fileName);   
		String baseName, path, outExtention;   
		if (fileName.empty())   
		{   
			WX::TerrainData* data = mSceneManipulator->getTerrainData();   
			Ogre::StringUtil::splitBaseFilename(data->mHeightmapFilename, baseName, outExtention);   
			LightmapFileName = baseName + ".lightmap.png";   
		}   
		mRenderTexture->writeContentsToFile(LightmapFileName);   

		// ����lightmap   
		static const String TEMP_GROUP_NAME = "#TEMP#"; // ��ʱ��Դ��   
		Ogre::StringUtil::splitFilename(LightmapFileName, baseName, path);   
		mSceneManipulator->getTerrainData()->mLightmapFilename = baseName;   
		Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();   
		rgm.addResourceLocation(path, "FileSystem", TEMP_GROUP_NAME, false);   
		// �������������protected,�Ժ���û�������������������,�����ǵ�������lightmap   
		mSceneManipulator->getTerrainData()->_loadLightmap(baseName, "image", TEMP_GROUP_NAME);    

		// ����ԭ���ĵ��β���,���ع�����   
		std::swap(materials, materialsCopy);   
		mSceneManipulator->getTerrain()->buildGeometry(mSceneManipulator->getBaseSceneNode(), true);   

		// ���о�̬���岻����ҪͶ����Ӱ��,��Ϊ������lightmap   
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

		// ������ʱ��Դ��   
		rgm.destroyResourceGroup(TEMP_GROUP_NAME);   
	}   


}  
*/