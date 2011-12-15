#pragma	once
#include	"stdafx.h"
#include	"PickEntity.h"
#include "OgreConfigFile.h"

class	Option;
class	DeferredSystem;

namespace Ogre
{
	class	WaterManager;
}

class	TestCore 
	: public Ogre::Singleton< TestCore >
	, public OIS::MouseListener
	, public OIS::KeyListener
	, public PickEntity::Listener
{
public:
	TestCore();
	~TestCore();

	bool	Initialize( HWND hTopLevelWnd, HWND hRenderWnd, int width, int height );
	void	Finalize();

	void	Resize( int width, int height );

	void	Update( float lapseTime );

public:
	virtual bool	mouseMoved( const OIS::MouseEvent &arg );
	virtual bool	mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool	mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	virtual bool	keyPressed( const OIS::KeyEvent &arg );
	virtual bool	keyReleased( const OIS::KeyEvent &arg );

	virtual	void	FindObj( Ogre::Entity* pEntity );
	virtual void	EraseObj( Ogre::Entity* pEntity );

private:
	bool	_loadOgre();
	bool	_loadOIS();
	bool	_loadScene();

	void	_updateCameraRamble();
	bool		setupResources();

private:
	HWND			m_hTopLevelWnd;
	HWND			m_hRenderWnd;
	int				m_Widht;
	int				m_Height;

	Option*			m_pOption;
	PickEntity*		m_pPickEntity;
	bool					m_bIni;
public:
	Ogre::Root*								m_pRoot;
	Ogre::RenderWindow*						m_pRenderWindow;
	Ogre::Viewport*							m_pViewPort;
	Ogre::SceneManager*						m_pSceneMgr;
	Ogre::Camera*							m_pMainCamera;
	Ogre::RTShader::ShaderGenerator*		m_pShaderGenerator;

	OIS::InputManager*						m_pInputMgr;
	OIS::Keyboard*							m_pKeyboard;
	OIS::Mouse*								m_pMouse;

	DeferredSystem*							m_pDeferredSystem;
	Ogre::WaterManager*						m_pWaterMgr;
};



