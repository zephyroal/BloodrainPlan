#include	"stdafx.h"
#include	"DeferredShadow.h"
#include	"DeferredSystem.h"

using namespace Ogre;

DeferredShadow::DeferredShadow( DeferredSystem* pDeferredSys, Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport )
{
	m_pDeferredSys = NULL;
	m_pShadowQuad = NULL;
	m_pSceneMgr = NULL;
	m_pMainCamera = NULL;
	m_pMainViewport = NULL;
	m_pSunLight = NULL;

	m_pDeferredSys = pDeferredSys;
	m_pMainViewport = pMainViewport;
	m_pMainCamera = pMainCamera;
	m_pSceneMgr = pSceneMgr;
	
	_ensureShadowInfo();
	_parserShadowLightInfo();

}

DeferredShadow::~DeferredShadow()
{

}

void	DeferredShadow::PrepareShadow()
{
	assert( m_pSceneMgr ); 


	//	todo: 找到本次渲染将会涉及的阴影灯找出来.

	//	逐一进行阴影叠加出来.

	//	在RenderObject 上添加 listener. 没渲染一个物体. 更新其材质信息.
	//	主要是更新 matProjToLightProj 


	//	1. 更新阴影相机
	m_ShadowInfo.pShadowCamera->setDirection( m_pSunLight->getDerivedDirection() );

	m_ShadowCameraSetup.getShadowCamera( m_pSceneMgr, 
		m_pMainCamera, 
		m_pMainViewport, 
		m_pSunLight, 
		m_ShadowInfo.pShadowCamera, 
		0 ); 

	//	更新阴影投射的深度图
	m_ShadowInfo.pShadowViewport->update();

	//	设置延迟阴影的相关参数 
	Ogre::Vector3 farRightTopCorner = m_pMainCamera->getViewMatrix( true ) * m_pMainCamera->getWorldSpaceCorners()[4];
	m_pParameterVP->setNamedConstant( "farRightTopCorner", farRightTopCorner );

	Matrix4 matShadowView = m_ShadowInfo.pShadowCamera->getViewMatrix( true );
	Matrix4 matShadowProj = m_ShadowInfo.pShadowCamera->getProjectionMatrixWithRSDepth();

	Matrix4 matShadowViewProj = matShadowProj * matShadowView;

	//	矩阵组合. 应该逆乘 
	Matrix4 matInvView = m_pMainCamera->getViewMatrix( true ).inverse();
	Matrix4 matViewToShadowProj = matShadowViewProj * matInvView;

	m_pParameterFP->setNamedConstant( "matViewToShadowProj", matViewToShadowProj );

}


void	DeferredShadow::_parserShadowLightInfo()
{
	//	先默认用一个方向光来实验
	if ( m_pSceneMgr->hasLight( "SunLight" ) )
	{
		m_pSunLight = m_pSceneMgr->getLight( "SunLight" );
	}
	else
	{
		m_pSunLight = m_pSceneMgr->createLight( "SunLight" );

		m_pSunLight->setCastShadows( true );
		m_pSunLight->setType( Light::LT_DIRECTIONAL );
		Vector3 vecDir( 0.f, -0.3f, 0.4f );
		vecDir.normalise();

		m_pSunLight->setDirection( vecDir );
		m_pSunLight->setDiffuseColour( ColourValue::Red );
	}
}

void	DeferredShadow::_ensureShadowInfo()
{
	//	确认阴影的投射深度纹理
	TexturePtr pShadowCastTexture = TextureManager::getSingleton().getByName( "ShadowCastTexture" );
	if ( pShadowCastTexture.getPointer() == NULL )
	{
		pShadowCastTexture = TextureManager::getSingleton().createManual( "ShadowCastTexture", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			1024,
			1024,
			0,
			PF_FLOAT32_R,
			TU_RENDERTARGET ); 

		Camera* pShadowCam = m_pSceneMgr->createCamera( "ShadowCastCamera" );
		pShadowCam->setAspectRatio( 1.f );

		RenderTarget* pCastShadowTarget = pShadowCastTexture->getBuffer()->getRenderTarget();
		pCastShadowTarget->setAutoUpdated( false );

		Viewport* pShadowViewport = pCastShadowTarget->addViewport( pShadowCam );
		pShadowViewport->setClearEveryFrame( true );
		pShadowViewport->setOverlaysEnabled( false );
		pShadowViewport->setBackgroundColour( ColourValue::White );
		pShadowViewport->setMaterialScheme( "CastShadow" );

		m_ShadowInfo.pShadowTex = pShadowCastTexture;
		m_ShadowInfo.pShadowCamera = pShadowCam;
		m_ShadowInfo.pShadowViewport = pShadowViewport;

		RenderQueueInvocationSequence* pShadowCastSeque = Root::getSingletonPtr()->createRenderQueueInvocationSequence( "ShadowCastQueue" );

		String strTemp = "ShadowCast_";
		for ( int index = DeferredSystem::RenderQueue_Avatar; index <= DeferredSystem::RenderQueue_Terrain; ++index )
		{
			pShadowCastSeque->add( index, strTemp + StringConverter::toString( index ) );
		}

		m_ShadowInfo.pShadowViewport->setRenderQueueInvocationSequenceName( pShadowCastSeque->getName() );
	}
	//	确认阴影屏幕四边形
	if ( m_pShadowQuad == NULL )
	{
		m_pShadowQuad = new Rectangle2D;
		m_pShadowQuad->setCorners( -1.f, 1.f, 1.f, -1.f );

		m_pShadowQuad->setRenderQueueGroup( DeferredSystem::RenderQueue_DeferredLight );

		m_pSceneMgr->getRootSceneNode()->attachObject( m_pShadowQuad );

		m_pMtlShadowQuad = MaterialManager::getSingleton().getByName( "DeferredShadow" );
		if ( m_pMtlShadowQuad.getPointer() == NULL )
		{
			assert( 0 );
			//	可以考虑用代码方式生成材质. 目前先算了
		}
		Pass* pPass = NULL;
		pPass = m_pMtlShadowQuad->getTechnique( 0 )->getPass( 0 );
		pPass->getTextureUnitState( 0 )->setTextureName( pShadowCastTexture->getName() );
		pPass->getTextureUnitState( 0 )->setTextureBorderColour( ColourValue::White );
		pPass->getTextureUnitState( 0 )->setTextureAddressingMode( TextureUnitState::TAM_BORDER );

		pPass->getTextureUnitState( 1 )->setTextureName( m_pDeferredSys->GetDeferredTexDepth()->getName() );

		m_pShadowQuad->setMaterial( m_pMtlShadowQuad->getName() );

		Ogre::Pass*	pPassShadowQuad = m_pMtlShadowQuad->getTechnique( 0 )->getPass( 0 );

		m_pParameterVP = pPassShadowQuad->getVertexProgramParameters();
		m_pParameterFP = pPassShadowQuad->getFragmentProgramParameters();
		m_pParameterVP->setIgnoreMissingParams( true );
		m_pParameterFP->setIgnoreMissingParams( true );
	}
}


Ogre::Texture*	DeferredShadow::GetTexCastShadow()
{
	return m_ShadowInfo.pShadowTex.getPointer();
}