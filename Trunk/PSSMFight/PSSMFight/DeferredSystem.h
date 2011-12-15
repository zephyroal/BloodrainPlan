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
		RenderQueue_Avatar = 5,					//	角色

		RenderQueue_InstanceStatic = 15,		//	静态实例化
		RenderQueue_InstanceVertex,				//	顶点常量实例化
		RenderQueue_WorldGeom,
		RenderQueue_Terrain,

		RenderQueue_Sky = 25,					//	天空

		RenderQueue_DeferredLight,				//	延迟作色光源 目前只支持一个方向光

		
		
		RenderQueue_Alpha,						//	半透明物体

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

	//	根据场景中的light 更新延迟光源
	bool	RefreshSceneLights();

	/*
	根据 nameList 来构建延迟渲染的光源

	考虑到多线程加载使用
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

	//	实心
	Ogre::MultiRenderTarget*	m_pMultiTargetDiff_Depth_Normal;
	Ogre::TexturePtr					m_pTexDiff;
	Ogre::TexturePtr					m_pTexDepth;
	Ogre::Viewport*					m_pViewportDiff_Depth_Normal;
	
	Ogre::TexturePtr			m_pTexLightSpaceDepth;
	//	上色 + 半透明
	Ogre::MultiRenderTarget*	m_pMultiTargetLighting_Trans;
	Ogre::TexturePtr			m_pTexLighting_Trans;
	Ogre::TexturePtr			m_pTexBackupLighting_Trans;
	Ogre::Viewport*				m_pViewportLighting_Trans;
};