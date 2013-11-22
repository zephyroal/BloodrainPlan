#include	"stdafx.h"
#include	"TestCore.h"
#include	"DeferredSystem.h"
#include	"Option.h"
#include	<iostream>
#include	"OutDepth.h"
#include	"Water.h"


template<> 
TestCore*	Ogre::Singleton< TestCore >::msSingleton = 0;

TestCore::TestCore()
{
	m_hTopLevelWnd = NULL;
	m_hRenderWnd = NULL;
	m_Widht = 0;
	m_Height = 0;
	m_pRoot = NULL;
	m_pOption = NULL;
	m_pPickEntity = NULL;

	m_pRenderWindow = NULL;
	m_pViewPort = NULL;
	m_pSceneMgr = NULL;
	m_pMainCamera = NULL;
// 	AllocConsole();
// 	freopen( "CONOUT$","w",stdout );
}

TestCore::~TestCore()
{

}

bool	TestCore::Initialize( HWND hTopLevelWnd, HWND hRenderWnd, int width, int height )
{
	//	支持中文目录
	::setlocale( LC_ALL,"Chinese-simplified" );

	m_hTopLevelWnd = hTopLevelWnd;
	m_hRenderWnd = hRenderWnd;
	m_Widht = width;
	m_Height = height;

	m_pOption = new Option;
	m_pPickEntity = new PickEntity;
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

	if ( m_pSceneMgr )
	{
		m_pSceneMgr->clearScene();
		m_pRoot->destroySceneManager( m_pSceneMgr );
		m_pSceneMgr = NULL;
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
	m_pKeyboard->capture();
	m_pMouse->capture();

	_updateCameraRamble();

	m_pWaterMgr->PrepareReflectTexture();

	m_pDeferredSystem->Render();
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


bool	TestCore::_loadOgre()
{
	using namespace Ogre;
	m_pRoot = new Root;

#ifdef _DEBUG
	m_pRoot->loadPlugin( "Plugin_OctreeSceneManager_d" );
	m_pRoot->loadPlugin( "RenderSystem_Direct3D9_d" );
#else
	m_pRoot->loadPlugin( "Plugin_OctreeSceneManager" );
	m_pRoot->loadPlugin( "RenderSystem_Direct3D9" );	
#endif

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
	m_pRoot->addResourceLocation( m_pOption->GetWorkingPath() + "Data\\RTShaderLib", "FileSystem", m_pOption->GetResGroupName() );
	m_pRoot->addResourceLocation( m_pOption->GetWorkingPath() + "Data\\Scene", "FileSystem", m_pOption->GetResGroupName() );

	m_pRenderWindow = m_pRoot->createRenderWindow( "TestCore", m_Widht, m_Height, false, &winParams );


	if ( RTShader::ShaderGenerator::initialize() == false )
	{
		return false;
	}

	m_pShaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();
	m_pShaderGenerator->setTargetLanguage( "hlsl" );
	m_pShaderGenerator->addSubRenderStateFactory( new RTShader::OutDepthFactory );

	//	资源初始化放在创建主窗口和初始化之后. 因为创建了主窗口才初始化了D3D
	ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

	m_pSceneMgr = m_pRoot->createSceneManager( "OctreeSceneManager", "TestMgr" );

	m_pShaderGenerator->addSceneManager( m_pSceneMgr );

	m_pMainCamera = m_pSceneMgr->createCamera( "TestCamera" );

	m_pPickEntity->BindScene( m_pSceneMgr, m_pMainCamera );

	m_pMainCamera->setAspectRatio( static_cast< float >( m_Widht ) / static_cast< float >( m_Height ) );

	m_pMainCamera->setFOVy( Radian( 45.f ) );

	m_pMainCamera->setNearClipDistance( 5.0f );

	m_pMainCamera->setFarClipDistance( 1000.0f );

	m_pMainCamera->setPosition( 0, 30, -30 );
	m_pMainCamera->lookAt( Vector3::ZERO );

	m_pViewPort = m_pRenderWindow->addViewport( m_pMainCamera );

	m_pViewPort->setBackgroundColour( ColourValue::Black );

	RenderQueueInvocationSequence* pMainRenderQueueSequ = m_pRoot->createRenderQueueInvocationSequence( "MainWindow" );
	pMainRenderQueueSequ->add( DeferredSystem::RenderQueue_MainWindow, "MainWindow" );
	m_pViewPort->setRenderQueueInvocationSequenceName( pMainRenderQueueSequ->getName() );

	m_pDeferredSystem = new DeferredSystem;
	m_pDeferredSystem->BindScene( m_pSceneMgr, m_pMainCamera, m_pViewPort );

	m_pWaterMgr = new Ogre::WaterManager( m_pDeferredSystem, m_pSceneMgr, m_pMainCamera, m_pViewPort );


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
	m_pSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );

	//	实心
 	Plane planeTerrain;
 	planeTerrain.normal = Vector3::UNIT_Y;
 	planeTerrain.d = 0;
 	MeshManager::getSingleton().createPlane( "EarthMesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, planeTerrain, 100,100,25,25,true,1,60,60,Vector3::UNIT_Z );
	

	Entity* pTemp= NULL;
	SceneNode* pNode = NULL;
	pTemp = m_pSceneMgr->createEntity( "head1", "ninja.mesh" );
	pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3::ZERO );
	pNode->attachObject( pTemp );
	pNode->scale( 0.1f, 0.1f, 0.1f );
	pTemp->setRenderQueueGroup( DeferredSystem::RenderQueue_WorldGeom );
	pTemp->setCastShadows( true );


	pTemp = m_pSceneMgr->createEntity( "Terrain", "EarthMesh" );
	pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3::ZERO );
	pNode->attachObject( pTemp );
	pTemp->setRenderQueueGroup( DeferredSystem::RenderQueue_WorldGeom );
	pTemp->setCastShadows( true );
	pTemp->setMaterialName( "Rockwall" );

	//	屏幕四边形
	{
		Ogre::Rectangle2D* pShadowQuad = new Rectangle2D; 
		pShadowQuad->setCorners( -1.f, 1.f, 1.f, -1.f );

		pShadowQuad->setRenderQueueGroup( DeferredSystem::RenderQueue_MainWindow );

		m_pSceneMgr->getRootSceneNode()->attachObject( pShadowQuad );

		MaterialPtr pMtl = MaterialManager::getSingleton().getByName( "DeferredMainQuadMtl" );
		
		Pass* pPass = NULL;
		pPass = pMtl->getTechnique( 0 )->getPass( 0 );
		pPass->getTextureUnitState( 0 )->setTextureName( m_pDeferredSystem->GetDeferredTexDiffuse()->getName() );

		pShadowQuad->setMaterial( "DeferredMainQuadMtl" );
	}

	return true;
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

