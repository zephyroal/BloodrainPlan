#pragma once
#include	"stdafx.h"
#include	<map>
#include	<vector>

class	DeferredLightMtlGenerator;
class	DeferredShadow;

class	DeferredSystem
{
public:
	enum	DeferredRenderQueue
	{
		RenderQueue_Backgroud = 0,
		RenderQueue_Avatar = 5,					//	��ɫ

		RenderQueue_InstanceStatic = 15,		//	��̬ʵ����
		RenderQueue_InstanceVertex,				//	���㳣��ʵ����
		RenderQueue_WorldGeom,
		RenderQueue_Terrain,

		RenderQueue_Sky = 25,					//	���

		RenderQueue_DeferredLight,				//	�ӳ���ɫ��Դ Ŀǰֻ֧��һ�������

		
		
		RenderQueue_Alpha,						//	��͸������

		RenderQueue_MainWindow = 50,

	};

	typedef	std::vector< DeferredRenderQueue >	DeferredRenderQueueList;
	typedef	std::vector< Ogre::String >			OgreLightNameList;

	struct	TexDefinition
	{
		Ogre::String			strName;
		Ogre::String			strRefTexName;
		unsigned int			width;
		unsigned int			height;
		Ogre::PixelFormatList	formatList; 
		bool					bOverlaysEnabled;
		Ogre::ColourValue		backGroundColor;

	};

	struct	DeferredTargetDefinition
	{
		TexDefinition			texDefinition;
		DeferredRenderQueueList	renderQueueList;
		Ogre::String			strCameraName;
	};

	struct	DeferredSchemeData
	{
		Ogre::String				strViewportSchemeName;
		DeferredTargetDefinition	targetDefinitionSolid;
		DeferredTargetDefinition	targetDefinitionLighting;
	};

public:
	static Ogre::String	DefaultSchemeName;

public:
	DeferredSystem();
	~DeferredSystem();

	bool	BindScene( Ogre::SceneManager* pSceneMgr, Ogre::Camera* pMainCamera, Ogre::Viewport* pMainViewport );

	void	Update( float fLapseTime );

	void	Render();
	void	RenderEx( const Ogre::String& strSchemeName = DefaultSchemeName );

	//	���ݳ����е�light �����ӳٹ�Դ
	bool	RefreshSceneLights();

	/*
	���� nameList �������ӳ���Ⱦ�Ĺ�Դ

	���ǵ����̼߳���ʹ��
	*/
	void	AddLights( const OgreLightNameList& nameList );

	void	DelLights( const OgreLightNameList& nameList );

	bool	CreateRenderScheme( const Ogre::String& strSchemeName, const DeferredSchemeData& schemeData );

	Ogre::Texture*	GetDeferredTexDepth();
	Ogre::Texture*	GetDeferredTexDiffuse();

private:
	bool	_initailize();

private:
	Ogre::Root*									m_pRoot;
	Ogre::SceneManager*					m_pSceneMgr;
	Ogre::Camera*								m_pMainCamera;
	Ogre::Viewport*							m_pMainViewport;
	Ogre::CompositorManager*		m_pCompositorMgr;
	DeferredLightMtlGenerator*		m_pDeferredLightMtlGenerator;
	DeferredShadow*						m_pDeferredShadow;

	//	ʵ��
	Ogre::MultiRenderTarget*	m_pMultiTargetDiff_Depth_Normal;
	Ogre::TexturePtr					m_pTexDiff;
	Ogre::TexturePtr					m_pTexDepth;
	Ogre::Viewport*					m_pViewportDiff_Depth_Normal;
	
	Ogre::TexturePtr			m_pTexLightSpaceDepth;
	//	��ɫ + ��͸��
	Ogre::MultiRenderTarget*	m_pMultiTargetLighting_Trans;
	Ogre::TexturePtr			m_pTexLighting_Trans;
	Ogre::TexturePtr			m_pTexBackupLighting_Trans;
	Ogre::Viewport*				m_pViewportLighting_Trans;
};