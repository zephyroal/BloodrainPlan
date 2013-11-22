#include	"stdafx.h"
#include	"DeferredSystem.h"
#include	"DeferredShadow.h"

using namespace Ogre;

Ogre::String DeferredSystem::DefaultSchemeName = "Scheme_DefaultDeferredShading";

DeferredSystem::DeferredSystem()
{
	m_pRoot = NULL;
	m_pMainSceneMgr = NULL;
	m_pMainCamera = NULL;
	m_pMainViewport = NULL;
	m_pCompositorMgr = NULL;
	m_pDeferredLightMtlGenerator = NULL;
	m_pMultiTargetDiff_Depth_Normal = NULL;
	m_pMultiTargetLighting_Trans = NULL;
	m_pViewportLighting_Trans = NULL;
    	m_pShadowMapQuad   = NULL;
}

DeferredSystem::~DeferredSystem()
{

}

bool	DeferredSystem::BindScene( Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport )
{
	assert( pSceneMgr );
	assert( pMainViewport );
	assert( pMainCamera );
	m_pMainSceneMgr = pSceneMgr;
	m_pMainCamera = pMainCamera;
	m_pMainViewport = pMainViewport;
	m_pCompositorMgr = CompositorManager::getSingletonPtr();
	//	先初始化系统
	m_pDeferredShadow = new DeferredShadow( this, pSceneMgr, pMainCamera, pMainViewport );
	_initailize();
	
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

		//最终处理结果 用于Ogre::Rectangle2D* pShadowMapMapQuad 
		m_pTexDiff = pTexMgr->createManual( "Deferred_Diff", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			screen_width,
			screen_height,
			0,
			PF_A8R8G8B8,
			TU_RENDERTARGET );

		//视角深度
		m_pTexDepth = pTexMgr->createManual( "Deferred_Depth", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			screen_width,
			screen_height,
			0,
			PF_FLOAT32_GR,
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
        //渲染完Solid物体 再就是渲染上影阴面片 
		pDefault->add( RenderQueue_DeferredLight, "DeferredLight" );
		//再渲染半透明
        pDefault->add( RenderQueue_Alpha, "Alpha" );
		m_pViewportDiff_Depth_Normal->setRenderQueueInvocationSequenceName( pDefault->getName() );
	}

     //	确认阴影屏幕四边形
	if ( m_pShadowMapQuad == NULL )
	{
		m_pShadowMapQuad = new Rectangle2D;
		m_pShadowMapQuad->setCorners( -1.0f, 1.0f, 1.0f, -1.0f );
		AxisAlignedBox aabb;
		aabb.setInfinite();
		m_pShadowMapQuad->setBoundingBox(aabb);
		m_pShadowMapQuad->setRenderQueueGroup( DeferredSystem::RenderQueue_DeferredLight );
		m_pMainSceneMgr->getRootSceneNode()->attachObject( m_pShadowMapQuad );

		m_pMtlShadowQuad = MaterialManager::getSingleton().getByName( "DeferredShadow" );
		if ( m_pMtlShadowQuad.getPointer() == NULL )
		{
			assert( 0 );
			//	可以考虑用代码方式生成材质. 目前先算了
		}
		Pass* pPass = NULL;
		pPass = m_pMtlShadowQuad->getTechnique( 0 )->getPass( 0 );
        pPass->getTextureUnitState( 0 )->setTextureName( GetDeferredTexDiffuse()->getName() );
	    pPass->getTextureUnitState( 1 )->setTextureName( GetDeferredTexDepth()->getName());

		pPass->getTextureUnitState( 2 )->setTextureName( GetShadowMap()->getName() );
		pPass->getTextureUnitState( 2 )->setTextureBorderColour( ColourValue::White );
		pPass->getTextureUnitState( 2 )->setTextureAddressingMode( TextureUnitState::TAM_BORDER );
	
		m_pShadowMapQuad->setMaterial( m_pMtlShadowQuad->getName() );

		Ogre::Pass*	pPassShadowQuad = m_pMtlShadowQuad->getTechnique( 0 )->getPass( 0 );

		m_pParameterVP = pPassShadowQuad->getVertexProgramParameters();
		m_pParameterFP = pPassShadowQuad->getFragmentProgramParameters();
		m_pParameterVP->setIgnoreMissingParams( true );
		m_pParameterFP->setIgnoreMissingParams( true );
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
	m_pDeferredShadow->PrepareShadowMap();
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

Ogre::Texture*	DeferredSystem::GetShadowMap()
{
	return m_pDeferredShadow->GetShadowMap();
}

Ogre::Texture*	DeferredSystem::GetDeferredTexDepth()
{
	return m_pTexDepth.getPointer();
}

Ogre::Texture*	DeferredSystem::GetDeferredTexDiffuse()
{
	return m_pTexDiff.getPointer();
}
