#include	"stdafx.h"
#include	"DeferredShadow.h"
#include	"DeferredSystem.h"

using namespace Ogre;

DeferredShadow::DeferredShadow( DeferredSystem* pDeferredSys, Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport )
{
	m_pDeferredSys  = NULL;
	m_pMainSceneMgr = NULL;
	m_pMainCamera   = NULL;
	m_pMainViewport = NULL;
	m_pSunLight     = NULL;

	m_pDeferredSys  = pDeferredSys;
	m_pMainViewport = pMainViewport;
	m_pMainCamera   = pMainCamera;
	m_pMainSceneMgr = pSceneMgr;

	_ensureShadowInfo();
	_parserShadowLightInfo();

}

DeferredShadow::~DeferredShadow()
{

}

void	DeferredShadow::PrepareShadowMap()
{
	assert( m_pMainSceneMgr ); 

	//	todo: �ҵ�������Ⱦ�����漰����Ӱ���ҳ���.
	//	��һ������Ӱ���ӳ���.
	//	��RenderObject ����� listener. û��Ⱦһ������. �����������Ϣ.
	//	��Ҫ�Ǹ��� matProjToLightProj 
	//	1. ������Ӱ���

	m_ShadowInfo.pShadowMapCamera->setDirection( m_pSunLight->getDerivedDirection() );
	m_ShadowCameraSetup.getShadowCamera( m_pMainSceneMgr, 
		m_pMainCamera, 
		m_pMainViewport, 
		m_pSunLight, 
		m_ShadowInfo.pShadowMapCamera, 
		0 ); 
	
	//	�����ӳ���Ӱ����ز��� 
	Ogre::Vector3 farRightTopCorner = m_pMainCamera->getViewMatrix( true ) * m_pMainCamera->getWorldSpaceCorners()[4];
	m_pDeferredSys->m_pParameterVP->setNamedConstant( "farRightTopCorner", farRightTopCorner );

	Matrix4 matShadowView = m_ShadowInfo.pShadowMapCamera->getViewMatrix( true );
	Matrix4 matShadowProj = m_ShadowInfo.pShadowMapCamera->getProjectionMatrixWithRSDepth();
	Matrix4 matShadowViewProj = matShadowProj * matShadowView;
	//	�������. Ӧ����� 
	Matrix4 matInvView = m_pMainCamera->getViewMatrix( true ).inverse();
	Matrix4 matViewToShadowProj = matShadowViewProj * matInvView;
	m_pDeferredSys->m_pParameterFP->setNamedConstant( "matViewToShadowProj", matViewToShadowProj );

    //	������ӰͶ������ͼ
	m_ShadowInfo.pShadowMapViewport->update();
}

void	DeferredShadow::_parserShadowLightInfo()
{
	//	��Ĭ����һ���������ʵ��
	if ( m_pMainSceneMgr->hasLight( "SunLight" ) )
	{
		m_pSunLight = m_pMainSceneMgr->getLight( "SunLight" );
	}
	else
	{
		m_pSunLight = m_pMainSceneMgr->createLight( "SunLight" );

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
	//	ȷ����Ӱ��Ͷ���������
	TexturePtr pShadowMapTex = TextureManager::getSingleton().getByName( "ShadowMapTex" );
	if ( pShadowMapTex.getPointer() == NULL )
	{
		pShadowMapTex = TextureManager::getSingleton().createManual( "ShadowMapTex", 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			1024,
			1024,
			0,
			PF_FLOAT32_R,
			TU_RENDERTARGET ); 
		Camera* pShadowCam = m_pMainSceneMgr->createCamera( "ShadowCastCamera" );
		pShadowCam->setAspectRatio( 1.f );
		RenderTarget* pCastShadowTarget = pShadowMapTex->getBuffer()->getRenderTarget();
		pCastShadowTarget->setAutoUpdated( false );
		Viewport* pShadowMapViewport = pCastShadowTarget->addViewport( pShadowCam );
		pShadowMapViewport->setClearEveryFrame( true );
		pShadowMapViewport->setOverlaysEnabled( false );
		pShadowMapViewport->setBackgroundColour( ColourValue::White );
		pShadowMapViewport->setMaterialScheme( "CastShadow" );

		m_ShadowInfo.pShadowMapTex = pShadowMapTex;
		m_ShadowInfo.pShadowMapCamera = pShadowCam;
		m_ShadowInfo.pShadowMapViewport = pShadowMapViewport;
		RenderQueueInvocationSequence* pShadowCastSeque = Root::getSingletonPtr()->createRenderQueueInvocationSequence( "ShadowCastQueue" );
		String strTemp = "ShadowCast_";
		for ( int index = DeferredSystem::RenderQueue_Avatar; index <= DeferredSystem::RenderQueue_Terrain; ++index )
		{
			pShadowCastSeque->add( index, strTemp + StringConverter::toString( index ) );
		}
		m_ShadowInfo.pShadowMapViewport->setRenderQueueInvocationSequenceName( pShadowCastSeque->getName() );
	}
	
}


Ogre::Texture*	DeferredShadow::GetShadowMap()
{
	return m_ShadowInfo.pShadowMapTex.getPointer();
}