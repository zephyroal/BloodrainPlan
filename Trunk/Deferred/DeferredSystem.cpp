#include	"stdafx.h"
#include	"DeferredSystem.h"
#include	"DeferredShadow.h"

using namespace Ogre;

Ogre::String DeferredSystem::DefaultSchemeName = "Scheme_DefaultDeferredShading";

DeferredSystem::DeferredSystem()
{
	m_pRoot = NULL;
	m_pSceneMgr = NULL;
	m_pMainCamera = NULL;
	m_pMainViewport = NULL;
	m_pCompositorMgr = NULL;
	m_pDeferredLightMtlGenerator = NULL;
	m_pMultiTargetDiff_Depth_Normal = NULL;
	m_pMultiTargetLighting_Trans = NULL;
	m_pViewportLighting_Trans = NULL;
}

DeferredSystem::~DeferredSystem()
{

}

bool	DeferredSystem::BindScene( Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport )
{
	assert( pSceneMgr );
	assert( pMainViewport );
	assert( pMainCamera );

	m_pSceneMgr = pSceneMgr;
	m_pMainCamera = pMainCamera;
	m_pMainViewport = pMainViewport;
	m_pCompositorMgr = CompositorManager::getSingletonPtr();

	//	先初始化系统
	_initailize();

	//	然后初始化其他
	m_pDeferredShadow = new DeferredShadow( this, pSceneMgr, pMainCamera, pMainViewport );

	return true;
}

bool	DeferredSystem::_initailize()
{
	m_pRoot = Root::getSingletonPtr();
	int screen_width = m_pMainViewport->getActualWidth();
	int screen_height = m_pMainViewport->getActualHeight();
	TextureManager* pTexMgr = TextureManager::getSingletonPtr();

	{
		//	实心
		m_pMultiTargetDiff_Depth_Normal = m_pRoot->getRenderSystem()->createMultiRenderTarget( "Multi_Diff_Depth_wNormal" );

		m_pTexDiff = pTexMgr->createManual( "Deferred_Diff", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			screen_width,
			screen_height,
			0,
			PF_A8R8G8B8,
			TU_RENDERTARGET );

		m_pTexDepth = pTexMgr->createManual( "Deferred_Depth", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			screen_width,
			screen_height,
			0,
			PF_FLOAT16_GR,
			TU_RENDERTARGET );

		m_pTexDiff->getBuffer()->getRenderTarget()->setAutoUpdated( false );
		m_pTexDepth->getBuffer()->getRenderTarget()->setAutoUpdated( false );

		m_pMultiTargetDiff_Depth_Normal->bindSurface( 0, m_pTexDiff->getBuffer()->getRenderTarget() );
		m_pMultiTargetDiff_Depth_Normal->bindSurface( 1, m_pTexDepth->getBuffer()->getRenderTarget() );

		m_pMultiTargetDiff_Depth_Normal->setAutoUpdated( false );

		m_pViewportDiff_Depth_Normal = m_pMultiTargetDiff_Depth_Normal->addViewport( m_pMainCamera );

		m_pViewportDiff_Depth_Normal->setOverlaysEnabled( false );
		m_pViewportDiff_Depth_Normal->setClearEveryFrame( true );
		m_pViewportDiff_Depth_Normal->setSkiesEnabled( false );

		RenderQueueInvocationSequence* pDefault = m_pRoot->createRenderQueueInvocationSequence( "Default" );

		String strTemp = "Solid_";
		for ( int index = RenderQueue_Avatar; index <= RenderQueue_Sky; ++index )
		{
			pDefault->add( index, strTemp + StringConverter::toString( index ) );
		}

		pDefault->add( RenderQueue_DeferredLight, "DeferredLight" );

		pDefault->add( RenderQueue_Alpha, "Alpha" );

		m_pViewportDiff_Depth_Normal->setRenderQueueInvocationSequenceName( pDefault->getName() );

	}

	return true;
}

void	DeferredSystem::Update( float fLapseTime )
{
}

void	DeferredSystem::Render()
{
	//	只拣选一次即可
	
	//	准备阴影深度图
	m_pDeferredShadow->PrepareShadow();

	//	实心物体
	m_pViewportDiff_Depth_Normal->update();

	m_pRoot->renderOneFrame();
}

void	DeferredSystem::RenderEx( const Ogre::String& strSchemeName /* = DefaultSchemeName */ )
{

}

bool	DeferredSystem::RefreshSceneLights()
{
	//	遍历场景灯光 去掉场景中没有的. 构建新增的


	return true;
}

void	DeferredSystem::AddLights( const OgreLightNameList& nameList )
{

}

void	DeferredSystem::DelLights( const OgreLightNameList& nameList )
{

}

bool	DeferredSystem::CreateRenderScheme( const Ogre::String& strSchemeName, const DeferredSchemeData& schemeData )
{

	return true;
}


Ogre::Texture*	DeferredSystem::GetDeferredTexDepth()
{
	return m_pTexDepth.getPointer();
}

Ogre::Texture*	DeferredSystem::GetDeferredTexDiffuse()
{
	return m_pTexDiff.getPointer();
}
