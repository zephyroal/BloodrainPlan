#include "ExampleApplication.h"
#include <OIS/OIS.h>

using namespace Ogre;

class MyListener:public ExampleFrameListener
{
protected:
	RenderTexture* depth_map ;
	SceneManager *mSceneMgr;
	TexturePtr texPtr;
	MaterialPtr matPtr;
public:
	MyListener(RenderWindow* win, Camera* cam,SceneManager *pSceneMgr):ExampleFrameListener(win,cam)
	{
		mSceneMgr=pSceneMgr;
		matPtr = MaterialManager::getSingleton().getByName("MyDepthMapMaterial");
		texPtr = TextureManager::getSingleton().getByName("DepthMap");
		depth_map = texPtr->getBuffer()->getRenderTarget();
	}
	bool frameRenderingQueued(const FrameEvent& evt)
	{
			ExampleFrameListener::frameRenderingQueued(evt);
			// no scene manager render state changes
			// (other than object world matrices)
			//mSceneMgr->_suppressRenderStateChanges(true);
			// use your depth pass (you could do this manually
			// using just the render system, but _setPass()
			// is easier if you have a built pass already)
			/*
			SceneNode *node=mSceneMgr->getSceneNode("NinjaNode");
						for (int i=0,Entity *ent=node->getAttachedObject(i);i<node->getAttachedObjectIterator())
						{
						}*/
			
			SceneNode *rootNode=mSceneMgr->getRootSceneNode();
			SceneNode::ObjectIterator iterMovableObject = rootNode->getAttachedObjectIterator();
			while ( iterMovableObject.hasMoreElements() )
			{
				// 不清楚没把movableObject从节点解绑.删除节点会不会有问题。应该不会才对...
				Entity *ent=mSceneMgr->getEntity(iterMovableObject.getNext()->getName());
				if(ent->getNumSubEntities()>0)
					ent->setMaterial(matPtr);
			}
			// 遍历删除子节点
			Ogre::SceneNode::ChildNodeIterator iterChild = rootNode->getChildIterator();
			while ( iterChild.hasMoreElements() )
			{
				String name=iterChild.getNext()->getName();
				if(mSceneMgr->hasEntity(name))
				{
					Entity *ent=mSceneMgr->getEntity(name);
					if(ent->getNumSubEntities()>0)
						ent->setMaterial(matPtr);
				}
			}
			//MovableObjectCollection* col=mSceneMgr->getMovableObjectCollection();
			// like said above, _suppressRenderStateChanges(true)
			// also suppresses the camera matrices
			//mSceneMgr->_setViewMatrix(mCamera->getViewMatrix(true));
			//mSceneMgr->_setProjectionMatrix(mCamera->getProjectionMatrixRS());
			// you might want to use sceneMgr->_renderScene(...)
			// if you need more control
			depth_map->update();
			// allow the scene manager to change the render states
			//mSceneMgr->_suppressRenderStateChanges(false);
			return true;
		}
};
class DepthMapApplication : public ExampleApplication, public RenderTargetListener
{
protected:
	Technique *mDepthTechnique;
	MyListener  *mMyFrameListener;
public:
	DepthMapApplication() {};
	~DepthMapApplication()
	{
		if (mMyFrameListener)
			delete mMyFrameListener;
	};

protected:
	void createScene(void)
	{
		Entity *ent = mSceneMgr->createEntity( "Ninja", "ninja.mesh" );
		SceneNode *nnode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "NinjaNode", Vector3(0, 0, -100) );
		nnode->attachObject( ent );
		// light creation, irrelevant 

		// Create the scene node 
		SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode1", Vector3(0, 0, 0)); 
		node->attachObject(mCamera); 

		MeshPtr pMesh = MeshManager::getSingleton().createPlane("Plane", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			Plane(Vector3(0.0f, 1.0f, 0.0f), 0.0f), 
			500.0, 500.0, 100, 100, true, 1, 1, 1, Vector3::NEGATIVE_UNIT_Z); 
		Entity* pPlaneEntity = mSceneMgr->createEntity("PlaneEntity", "Plane"); 
		SceneNode* pSceneNodePlane = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlaneNode"); 
		pSceneNodePlane->attachObject(pPlaneEntity); 
		pPlaneEntity->setMaterialName("Document"); 

		Entity* pSphereEntity = mSceneMgr->createEntity("SphereEntity", SceneManager::PT_SPHERE); 
		SceneNode* pSceneNodeSphere = mSceneMgr->getRootSceneNode()->createChildSceneNode("SphereNode"); 
		pSceneNodeSphere->attachObject(pSphereEntity); 
		pSceneNodeSphere->translate(0.0f, 60.0f, 0.0f); 
		pSphereEntity->setMaterialName("PlanetMap");       

		//Viewport *vp = mCamera->getViewport(); 
		//mCamera->getViewport()->setDimensions(0.0, 0.0, 0.5, 1.0); 
		//vp->setBackgroundColour(ColourValue(1.0, 0.0, 0.0)); 

		//vp = mWindow->addViewport(mCamera, 1, 0.5, 0.0, 0.5, 1.0); 
		//vp->setBackgroundColour(ColourValue(1.0, 1.0, 0.0)); 

		createDepthRenderTextureHLSL();//createDepthRenderTexture(); 
		/*
		CompositorInstance* pCompositor = CompositorManager::getSingleton().addCompositor(mWindow->getViewport(1), "DepthMap");
		std::cout << pCompositor << std::endl; 
		std::cout.flush();
		CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(1), "DepthMap", true); 
		pCompositor->getTechnique()->getTargetPass(0)->setInputMode(CompositionTargetPass::IM_PREVIOUS); 
		*/
	}
	void createDepthRenderTextureHLSL() 
	{ 
		TexturePtr texPtr = TextureManager::getSingleton().createManual("DepthMap", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 512, 512, 0, PF_R8G8B8, TU_RENDERTARGET);
		RenderTexture *depth_map = texPtr->getBuffer()->getRenderTarget();
		Viewport *viewport=depth_map->addViewport(mCamera);
		viewport->setOverlaysEnabled(false);
		viewport->setBackgroundColour(ColourValue::White); 
		depth_map->setAutoUpdated(false);
		
		
		MaterialPtr pMtl = MaterialManager::getSingleton().create("RenderQuad","General");
		pMtl->getTechnique( 0 )->getPass( 0 )->createTextureUnitState("DepthMapUnit");
		pMtl->getTechnique( 0 )->getPass( 0 )->getTextureUnitState(0)->setTextureName("DepthMap");
		
		Ogre::OverlayElement *mpicture=NULL;
		mpicture = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","Overlay/Picture");
		mpicture->setParameter("metrics_mode","pixels");
		mpicture->setParameter("left","5");
		mpicture->setParameter("top","50");
		mpicture->setParameter("width","400");
		mpicture->setParameter("height","250");
		//	mpicture->setMaterialName("picture/Picture");
		mpicture->setMaterialName("RenderQuad");
		Ogre::Overlay *mOverlay = Ogre::OverlayManager::getSingleton().create("Overlay");
		mOverlay->add2D((Ogre::OverlayContainer*)mpicture);
		mOverlay->show();
	}
	virtual void createFrameListener(void)
	{
		mMyFrameListener=new MyListener(mWindow, mCamera,mSceneMgr);
		mMyFrameListener->showDebugOverlay(true);
		mRoot->addFrameListener((ExampleFrameListener*)mMyFrameListener);
	}
	void createDepthRenderTexture() 
	{ 
		// Create the depth render texture 
		TexturePtr depthTexture = TextureManager::getSingleton().createManual( 
			"NinjaDepthMap", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			TEX_TYPE_2D, mCamera->getViewport()->getActualWidth(), mCamera->getViewport()->getActualHeight(), 
			0, PF_FLOAT16_R, TU_RENDERTARGET); 

		// Get its render target and add a viewport to it 
		RenderTexture *depthTarget = depthTexture->getBuffer()->getRenderTarget(); 
		Viewport* depthViewport = depthTarget->addViewport(mCamera, 40); 
		depthViewport->setBackgroundColour(ColourValue::Black); 

		// Register 'this' as a render target listener 
		depthTarget->addListener(this); 

		// Get the technique to use when rendering the depth render texture 
		MaterialPtr mDepthMaterial = MaterialManager::getSingleton().getByName("DepthMap"); 
		mDepthMaterial->load(); // needs to be loaded manually 
		mDepthTechnique = mDepthMaterial->getTechnique(0); 
// 		Ogre::String path="C:\\windows\\main.txt";
// 		path=Ogre::StringUtil::standardisePath(path);
		// Create a custom render queue invocation sequence for the depth render texture 
		RenderQueueInvocationSequence* invocationSequence = 
			Root::getSingleton().createRenderQueueInvocationSequence("DepthMap"); 

		// Add a render queue invocation to the sequence, and disable shadows for it 
		RenderQueueInvocation* invocation = invocationSequence->add(RENDER_QUEUE_MAIN, "main"); 
		invocation->setSuppressShadows(true); 
		invocation->setSuppressRenderStateChanges(true);
		mSceneMgr->_setPass(mDepthTechnique->getPass(0), true, false);


		// Set the render queue invocation sequence for the depth render texture viewport 
		depthViewport->setRenderQueueInvocationSequenceName("DepthMap"); 
		//depthViewport2->setRenderQueueInvocationSequenceName("DepthMap"); 
		
	}    
};


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
	// Create application object
	DepthMapApplication app;

	try {
		app.go();
	} catch(Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!",
			MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n",
			e.getFullDescription().c_str());
#endif
	}


	return 0;
}