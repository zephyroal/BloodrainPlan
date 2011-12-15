#pragma once
#include	"stdafx.h"
#include	<map>
#include	<set>

typedef	std::map< Ogre::String, Ogre::Entity* >			Str_Entity;
typedef	std::vector< Ogre::Entity* >					Vec_Entity;

class	PickEntity
{
public:
	class	Listener
	{
	public:
		virtual ~Listener() {}

		virtual	void	FindObj( Ogre::Entity* pEntity ){}
		virtual	void	EraseObj( Ogre::Entity* pEntity ){}
	};

private:
	typedef	std::set< Listener* >						ListenerSet;

public:
	PickEntity();
	~PickEntity();

	//	绑定场景
	void	BindScene( Ogre::SceneManager* pSceneMgr, Ogre::Camera* pCamera );

	void	OnMousePressed( int mouseX, int mouseY );
	void	OnMouseMoved( int mouseX, int mouseY );
	void	OnMouseReleased( int mouseX, int mouseY );

	//	查询鼠标拾取到的子实体
	bool	PickSubEntity( int mouseX, int mouseY, Ogre::SubEntity** pResult, Ogre::Vector3 &hitposition, unsigned long mask = 0xFFFFFFFF );

	//	添加监听器
	void	AddListener( Listener* pListener );
	void	RemoveListener( Listener* pListener );

	void	SetQueryMask( unsigned long mask = 0xFFFFFFFF );

private:
	void			_getSubMeshData( int subMeshIndex, const Ogre::MeshPtr mesh, size_t& vertex_count, size_t& index_count, const Ogre::Vector3& position, const Ogre::Quaternion& orient, const Ogre::Vector3& scale );
	void			_getMeshData( const Ogre::MeshPtr mesh, size_t& vertex_count, size_t& index_count, const Ogre::Vector3& position, const Ogre::Quaternion& orient, const Ogre::Vector3& scale );
	
	bool			_pickSubEntity( const Ogre::Vector2& vPoint1, Ogre::SubEntity** pResult, Ogre::Vector3 &hitposition, unsigned long mask = 0xFFFFFFFF );
	Str_Entity		_pickRect( const Ogre::Vector2& vPoint1, const Ogre::Vector2& vPoint2, unsigned long mask = 0xFFFFFFFF );
	Vec_Entity		_findAddEntitis( Str_Entity oldMap, Str_Entity newMap );
	Vec_Entity		_findEraseEntitis( Str_Entity oldMap, Str_Entity newMap );

private:
	Ogre::SceneManager*							m_pSceneMgr;
	Ogre::Camera*								m_pCamera;
	Ogre::RaySceneQuery*						m_pSingleSceneQuery;
	Ogre::PlaneBoundedVolumeListSceneQuery*		m_pMultiSceneQuery;

	ListenerSet		m_Listeners;
	bool			m_IsMultiSelect;	//	是否进入框选状态
	Str_Entity		m_OldEntitis;		//	已框选中的物体
	Ogre::Vector2	m_MousePressPoint;	//	鼠标按下的位置

	unsigned long	m_QueryMask;

	unsigned int	m_VertexBufferSize;
	unsigned int	m_IndexBufferSize;
	Ogre::Vector3*	m_pVertexBuffer;
	unsigned long*	m_pIndexBuffer;
};