#include	"stdafx.h"
#include	"TestCore.h"
#include	"DeferredSystem.h"
#include	"Option.h"
#include	<iostream>
#include	"OutDepth.h"
#include	"Water.h"

template<> TestCore*	Ogre::Singleton< TestCore >::msSingleton = 0;

TestCore::TestCore()
{
	m_hTopLevelWnd  = NULL;
	m_hRenderWnd    = NULL;
	m_Widht         = 0;
	m_Height        = 0;
	m_pRoot         = NULL;
	m_pOption       = NULL;
	m_pPickEntity   = NULL;

	m_pRenderWindow = NULL;
	m_pMainViewPort = NULL;
	m_pMainSceneMgr     = NULL;
	m_pMainCamera   = NULL;
	m_bIni          = false;

	AllocConsole();
	// 	freopen( "CONOUT$","w",stdout );
}

TestCore::~TestCore()
{

}

bool	TestCore::Initialize( HWND hTopLevelWnd, HWND hRenderWnd, int width, int height )
{
	//	支持中文目录
	::setlocale( LC_ALL,"Chinese-simplified" );
	m_hTopLevelWnd      = hTopLevelWnd;
	m_hRenderWnd        = hRenderWnd;
	m_Widht             = width;
	m_Height            = height;
	m_pOption           = new Option;
	m_pPickEntity       = new PickEntity;

	m_pPickEntity->AddListener( this );
	if ( m_pOption->Initialize() == false )
	{
		return false;
	}
	if ( _loadOgre() == false )
	{
		return false;
	}
	if ( _loadOIS() == false )
	{
		return false;
	}
	if ( _loadScene() == false )
	{
		return false;
	}
	m_bIni              = true;

	return true;
}

void	TestCore::Finalize()
{
	//释放OIS
	m_pInputMgr->destroyInputObject( m_pKeyboard );
	m_pInputMgr->destroyInputObject( m_pMouse );
	OIS::InputManager::destroyInputSystem( m_pInputMgr );
	//	释放RTSS
	Ogre::MaterialManager::getSingleton().setActiveScheme( Ogre::MaterialManager::DEFAULT_SCHEME_NAME );
	if ( m_pShaderGenerator )
	{				
		Ogre::RTShader::ShaderGenerator::finalize();
		m_pShaderGenerator = NULL;
	}
	if ( m_pMainSceneMgr )
	{
		m_pMainSceneMgr->clearScene();
		m_pRoot->destroySceneManager( m_pMainSceneMgr );
		m_pMainSceneMgr = NULL;
	}
	if ( m_pRenderWindow )
	{
		m_pRenderWindow->destroy();
		m_pRenderWindow = NULL;
	}
	if ( m_pRoot )
	{
		m_pRoot->shutdown();
		delete m_pRoot;
		m_pRoot = NULL;
	}
}

void	TestCore::Resize( int width, int height )
{
	if ( m_pMouse )
	{
		const OIS::MouseState& mouseState = m_pMouse->getMouseState();
		mouseState.width = m_Widht;
		mouseState.height = m_Height;
	}
}

void	TestCore::Update( float lapseTime )
{
	if(!m_bIni)
		return;
	m_pKeyboard->capture();
	m_pMouse->capture();
	_updateCameraRamble();
	m_pWaterMgr->PrepareReflectTexture();
	m_pDeferredSystem->Render();
}

void	TestCore::_updateCameraRamble()
{
	using namespace Ogre;
	float cameraSpeed = 2.f;
	Vector3 moveRelative( Vector3::ZERO );

	if ( m_pKeyboard->isKeyDown( OIS::KC_W ) )
	{
		moveRelative.z -= cameraSpeed;
	}
	if ( m_pKeyboard->isKeyDown( OIS::KC_S ) )
	{
		moveRelative.z += cameraSpeed;
	}
	if ( m_pKeyboard->isKeyDown( OIS::KC_A ) )
	{
		moveRelative.x -= cameraSpeed;
	}
	if ( m_pKeyboard->isKeyDown( OIS::KC_D ) )
	{
		moveRelative.x += cameraSpeed;
	}
	const OIS::MouseState& mouseState = m_pMouse->getMouseState();
	if ( mouseState.buttonDown( OIS::MB_Right ) )
	{
		m_pMainCamera->yaw( Degree( -mouseState.X.rel * 0.2f ) );
		m_pMainCamera->pitch( Degree( -mouseState.Y.rel * 0.2f ) );
	}
	m_pMainCamera->moveRelative( moveRelative );
}

bool	TestCore::mouseMoved( const OIS::MouseEvent &arg )
{
	m_pPickEntity->OnMouseMoved( arg.state.X.abs, arg.state.Y.abs );
	return true;
}

bool	TestCore::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if ( id == OIS::MB_Left )
	{
		m_pPickEntity->OnMousePressed( arg.state.X.abs, arg.state.Y.abs );
	}

	return true;
}

bool	TestCore::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if ( id == OIS::MB_Left )
	{
		m_pPickEntity->OnMouseReleased( arg.state.X.abs, arg.state.Y.abs );
	}

	return true;
}

bool	TestCore::keyPressed( const OIS::KeyEvent &arg )
{
	if ( arg.key == OIS::KC_ESCAPE )
	{
		::SendMessage( m_hTopLevelWnd, WM_CLOSE, 0, 0 );
	}

	return true;
}

bool	TestCore::keyReleased( const OIS::KeyEvent &arg )
{

	return true;
}

void	TestCore::FindObj( Ogre::Entity* pEntity )
{
	pEntity->getParentSceneNode()->showBoundingBox( true );
}

void	TestCore::EraseObj( Ogre::Entity* pEntity )
{
	pEntity->getParentSceneNode()->showBoundingBox( false );
}

bool	TestCore::setupResources(void)
{
	using namespace Ogre;
	// Load resource paths from config file
	ConfigFile cf;
#if OGRE_DEBUG_MODE
	cf.load( "resources_d.cfg");
#else
	cf.load("resources.cfg");
#endif

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();
	//感谢XX八部
	bool recursive;
	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
			// OS X does not set the working directory relative to the app,
			// In order to make things portable on OS X we need to provide
			// the loading with it's own bundle path location
			if (!StringUtil::startsWith(archName, "/", false)) // only adjust relative dirs
				archName = String(macBundlePath() + "/" + archName);
#endif
			recursive= false;
			if (archName[0] == '*')
			{
				archName.erase(0, 1);
				Ogre::StringUtil::trim(archName);
				recursive = true;
			}
			ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName,recursive);
		}
	}
	return true;
}

bool	TestCore::_loadOgre()
{
	using namespace Ogre;

#ifdef _DEBUG
	m_pRoot = new Root("plugins_d.cfg", 
		"ogre.cfg",  "Ogre.log");
#else
	m_pRoot = new Root("plugins.cfg", 
		"ogre.cfg",  "Ogre.log");
#endif
	/*
	#ifdef _DEBUG
	m_pRoot->loadPlugin( "Plugin_OctreeSceneManager_d" );
	m_pRoot->loadPlugin( "RenderSystem_Direct3D9_d" );
	#else
	m_pRoot->loadPlugin( "Plugin_OctreeSceneManager" );
	m_pRoot->loadPlugin( "RenderSystem_Direct3D9" );	
	#endif*/

	RenderSystem* pRenderSys = m_pRoot->getRenderSystemByName( "Direct3D9 Rendering Subsystem" );

	if ( pRenderSys == NULL )
	{
		return false;
	}

	m_pRoot->setRenderSystem( pRenderSys );

	const RenderSystemList& rList = m_pRoot->getAvailableRenderers();

	m_pRoot->initialise( false );

	NameValuePairList winParams;
	winParams["FSAA"] = "0";
	winParams["vsync"] = "false";
	winParams["useNVPerfHUD"] = "true";
	winParams["depthBuffer"] = "false";
	winParams["externalWindowHandle"] = Ogre::StringConverter::toString( (size_t)m_hRenderWnd );

	//	parser resource
	m_pRoot->addResourceLocation( m_pOption->GetWorkingPath() + "../Data\\RTShaderLib", "FileSystem", m_pOption->GetResGroupName() );
	m_pRoot->addResourceLocation( m_pOption->GetWorkingPath() + "../Data\\Scene", "FileSystem", m_pOption->GetResGroupName() );
	setupResources();

	m_pRenderWindow = m_pRoot->createRenderWindow( "PSSMFightMainWindow", m_Widht, m_Height, false, &winParams );

	if ( RTShader::ShaderGenerator::initialize() == false )
	{
		return false;
	}

	m_pShaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();
	m_pShaderGenerator->setTargetLanguage( "hlsl" );
	m_pShaderGenerator->addSubRenderStateFactory( new RTShader::OutDepthFactory );

	//	资源初始化放在创建主窗口和初始化之后. 因为创建了主窗口才初始化了D3D
	ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
	m_pMainSceneMgr = m_pRoot->createSceneManager( "OctreeSceneManager", "MainSceneMgr" );
	m_pShaderGenerator->addSceneManager( m_pMainSceneMgr );

	m_pMainCamera = m_pMainSceneMgr->createCamera( "MainCamera" );
	m_pPickEntity->BindScene( m_pMainSceneMgr, m_pMainCamera );
	m_pMainCamera->setAspectRatio( static_cast< float >( m_Widht ) / static_cast< float >( m_Height ) );
	m_pMainCamera->setFOVy( Radian( 45.f ) );
	m_pMainCamera->setNearClipDistance( 5.0f );
	m_pMainCamera->setFarClipDistance( 1000.0f );
	m_pMainCamera->setPosition( 0, 30, -30 );
	m_pMainCamera->lookAt( Vector3::ZERO );

	m_pMainViewPort = m_pRenderWindow->addViewport( m_pMainCamera );
	m_pMainViewPort->setBackgroundColour( ColourValue::Black );
	m_pMainViewPort->setClearEveryFrame( true, FBT_COLOUR );

	RenderQueueInvocationSequence* pMainRenderQueueSequ = m_pRoot->createRenderQueueInvocationSequence( "MainWindowRQIvcocationSequence" );
	pMainRenderQueueSequ->add( DeferredSystem::RenderQueue_MainWindow, "RenderQueue_MainWindow" );

	m_pMainViewPort->setRenderQueueInvocationSequenceName( pMainRenderQueueSequ->getName() );

	m_pDeferredSystem = new DeferredSystem;
	m_pDeferredSystem->BindScene( m_pMainSceneMgr, m_pMainCamera, m_pMainViewPort );

	m_pWaterMgr = new Ogre::WaterManager( m_pDeferredSystem, m_pMainSceneMgr, m_pMainCamera, m_pMainViewPort );

	return true;
}

bool	TestCore::_loadOIS()
{
	using namespace OIS;

	OIS::ParamList pl;
	std::ostringstream winHandleStr;

	winHandleStr << (size_t)m_hTopLevelWnd;

	pl.insert( std::make_pair("WINDOW", winHandleStr.str()) );
	pl.insert(std::make_pair(std::string("w32_mouse"), "DISCL_FOREGROUND"));     
	pl.insert(std::make_pair(std::string("w32_mouse"), "DISCL_NONEXCLUSIVE"));	

	m_pInputMgr = OIS::InputManager::createInputSystem( pl );


	m_pKeyboard = static_cast<OIS::Keyboard*>( m_pInputMgr->createInputObject( OIS::OISKeyboard, true ) );
	m_pMouse = static_cast<OIS::Mouse*>( m_pInputMgr->createInputObject( OIS::OISMouse, true ) );

	m_pKeyboard->setEventCallback( this );
	m_pMouse->setEventCallback( this );


	Resize( m_Widht, m_Height );

	return true;
}

bool	TestCore::_loadScene()
{
	using namespace Ogre;
    Entity* pTemp= NULL;
	SceneNode* pNode = NULL;

	m_pMainSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );


	Plane planeTerrain;
	planeTerrain.normal = Vector3::UNIT_Y;
	planeTerrain.d = 0;
	MeshManager::getSingleton().createPlane( "EarthMesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, planeTerrain, 100,100,25,25,true,1,60,60,Vector3::UNIT_Z );

    pTemp = m_pMainSceneMgr->createEntity( "Terrain", "EarthMesh" );
	pNode = m_pMainSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3::ZERO );
	pNode->attachObject( pTemp );
	pTemp->setRenderQueueGroup( DeferredSystem::RenderQueue_WorldGeom );
	pTemp->setCastShadows( false );
	pTemp->setMaterialName( "Examples/SceneCubeMap1" );
	
	pTemp = m_pMainSceneMgr->createEntity( "head1", "ninja.mesh" );
	pNode = m_pMainSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3::ZERO );
	pNode->attachObject( pTemp );
	pNode->scale( 0.1f, 0.1f, 0.1f );
	pTemp->setRenderQueueGroup( DeferredSystem::RenderQueue_WorldGeom );
	pTemp->setCastShadows( false );


	//	屏幕四边形
	Ogre::Rectangle2D* pMainWindowQuad = new Rectangle2D; 
	pMainWindowQuad->setCorners( -1.0f, 1.0f, 1.0f, -1.0f );
    //MainViewport RQIS 只对它进行渲染 
	pMainWindowQuad->setRenderQueueGroup( DeferredSystem::RenderQueue_MainWindow );
	m_pMainSceneMgr->getRootSceneNode()->attachObject( pMainWindowQuad );
	MaterialPtr pMtl = MaterialManager::getSingleton().getByName( "ScreenSM" );
	
    Pass* pPass = NULL;
	pPass = pMtl->getTechnique( 0 )->getPass( 0 );
	pPass->getTextureUnitState( 0 )->setTextureName( m_pDeferredSystem->GetDeferredTexDiffuse()->getName() );
	pPass->getTextureUnitState( 1 )->setTextureName( m_pDeferredSystem->GetDeferredTexDepth()->getName() );
	pPass->getTextureUnitState( 2 )->setTextureName( m_pDeferredSystem->GetShadowMap()->getName());
	pMainWindowQuad->setMaterial( pMtl->getName() );


	return true;
}

