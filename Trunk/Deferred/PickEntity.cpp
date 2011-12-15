#include	"stdafx.h"
#include	"PickEntity.h"
#include	<minmax.h>
#include	<iostream>
using namespace std;

PickEntity::PickEntity()
{
	m_pSceneMgr = NULL;
	m_pCamera = NULL;
	m_pSingleSceneQuery = NULL;
	m_pMultiSceneQuery = NULL;
	m_IsMultiSelect = false;

	m_VertexBufferSize = 0;
	m_IndexBufferSize = 0;
	m_pVertexBuffer = 0;
	m_pIndexBuffer = 0;
}

PickEntity::~PickEntity()
{
	if ( m_pSingleSceneQuery )
	{
		m_pSceneMgr->destroyQuery( m_pSingleSceneQuery );
		m_pSingleSceneQuery = NULL;
	}

	if ( m_pMultiSceneQuery )
	{
		m_pSceneMgr->destroyQuery( m_pMultiSceneQuery );
		m_pMultiSceneQuery = NULL;
	}
}

void	PickEntity::BindScene( Ogre::SceneManager* pSceneMgr, Ogre::Camera* pCamera )
{
	assert( pSceneMgr );
	assert( pCamera );

	//	销毁之前的查询器
	if ( m_pSingleSceneQuery )
	{
		m_pSceneMgr->destroyQuery( m_pSingleSceneQuery );
		m_pSingleSceneQuery = NULL;
	}

	if ( m_pMultiSceneQuery )
	{
		m_pSceneMgr->destroyQuery( m_pMultiSceneQuery );
		m_pMultiSceneQuery = NULL;
	}

	//	绑定当前场景
	m_pSceneMgr = pSceneMgr;
	m_pCamera = pCamera;

	m_pSingleSceneQuery = m_pSceneMgr->createRayQuery( Ogre::Ray() );
	m_pMultiSceneQuery = m_pSceneMgr->createPlaneBoundedVolumeQuery( Ogre::PlaneBoundedVolumeList() );
}

void	PickEntity::OnMousePressed( int mouseX, int mouseY )
{
	if ( m_IsMultiSelect )
	{
		return ;
	}

	ListenerSet::iterator lisIter = m_Listeners.begin();
	for ( ; lisIter != m_Listeners.end(); ++lisIter )
	{
		Str_Entity::iterator entityIter = m_OldEntitis.begin();
		for ( ; entityIter != m_OldEntitis.end(); ++entityIter )
		{
			(*lisIter)->EraseObj( entityIter->second );
		}
	}
	m_OldEntitis.clear();

	m_MousePressPoint.x = static_cast< float >( mouseX );
	m_MousePressPoint.y = static_cast< float >( mouseY );

	Ogre::Vector2 mousePoint;
	mousePoint.x = m_MousePressPoint.x / m_pCamera->getViewport()->getActualWidth();
	mousePoint.y = m_MousePressPoint.y / m_pCamera->getViewport()->getActualHeight();


	Ogre::SubEntity* pSelectObj = NULL;
	Ogre::Vector3 vHitPos( Ogre::Vector3::ZERO );

	if ( _pickSubEntity( mousePoint, &pSelectObj, vHitPos ) )
	{
		Ogre::Entity* pEntity = pSelectObj->getParent();
		m_OldEntitis[pEntity->getName()] = pEntity;
		m_IsMultiSelect = false;

		ListenerSet::iterator lisIter = m_Listeners.begin();
		for ( ; lisIter != m_Listeners.end(); ++lisIter )
		{
			(*lisIter)->FindObj( pEntity );
		}
	}
	else
	{
		m_IsMultiSelect = true;
		cout << endl;
		cout << "开始框选" << endl;
	}
	m_IsMultiSelect = true;
}

void	PickEntity::OnMouseMoved( int mouseX, int mouseY )
{
	if ( m_IsMultiSelect == false )
		return;


	Ogre::Vector2 point2;
	point2.x = static_cast< float >( mouseX );
	point2.y = static_cast< float >( mouseY );

	//	如果距离小于 视为无效框选
	float width = abs( point2.x - m_MousePressPoint.x );
	float height = abs( point2.y - m_MousePressPoint.y );
	if ( width < 3.f || height < 3.f )
	{
		return ;
	}

	Ogre::Vector2 point1;
	point1.x = m_MousePressPoint.x / m_pCamera->getViewport()->getActualWidth();
	point1.y = m_MousePressPoint.y / m_pCamera->getViewport()->getActualHeight();
	point2.x = point2.x / m_pCamera->getViewport()->getActualWidth();
	point2.y = point2.y / m_pCamera->getViewport()->getActualHeight();

	Str_Entity movableMap = _pickRect( point1, point2 );

	Vec_Entity::iterator movableIter;

	//	找出新增加的物体
	Vec_Entity addMovables = _findAddEntitis( m_OldEntitis, movableMap );
	movableIter = addMovables.begin();
	for ( ; movableIter != addMovables.end(); ++movableIter )
	{
		ListenerSet::iterator lisIter = m_Listeners.begin();
		for ( ; lisIter != m_Listeners.end(); ++lisIter )
		{
			(*lisIter)->FindObj( *movableIter );
		}
	}

	//	找出去掉的物体
	Vec_Entity eraseMovables = _findEraseEntitis( m_OldEntitis, movableMap );
	movableIter = eraseMovables.begin();
	for ( ; movableIter != eraseMovables.end(); ++movableIter )
	{
		ListenerSet::iterator lisIter = m_Listeners.begin();
		for ( ; lisIter != m_Listeners.end(); ++lisIter )
		{
			(*lisIter)->EraseObj( *movableIter );
		}
	}

	//	复制过来
	m_OldEntitis.clear();
	m_OldEntitis = movableMap;
}

void	PickEntity::OnMouseReleased( int mouseX, int mouseY )
{
	m_IsMultiSelect = false;
	cout << "鼠标弹起" << endl;
}

bool	PickEntity::PickSubEntity( int mouseX, int mouseY, Ogre::SubEntity** pResult, Ogre::Vector3 &hitposition, unsigned long mask )
{
	if ( m_pSceneMgr == NULL || m_pCamera == NULL )
	{
		return false;
	}

	Ogre::Vector2 point;
	point.x = static_cast< float >( mouseX ) / m_pCamera->getViewport()->getActualWidth();
	point.y = static_cast< float >( mouseY ) / m_pCamera->getViewport()->getActualHeight();

	return _pickSubEntity( point, pResult, hitposition, mask );
}

void	PickEntity::AddListener( Listener* pListener )
{
	m_Listeners.insert( pListener );
}

void	PickEntity::RemoveListener( Listener* pListener )
{

	ListenerSet::iterator lisIter = m_Listeners.begin();
	for ( ; lisIter != m_Listeners.end(); ++lisIter )
	{
		if ( (*lisIter) == pListener )
		{
			m_Listeners.erase( lisIter );
		}
	}

}

void	PickEntity::_getSubMeshData( int subMeshIndex, const Ogre::MeshPtr mesh, size_t& vertex_count, size_t& index_count, const Ogre::Vector3& position, const Ogre::Quaternion& orient, const Ogre::Vector3& scale )
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	// Calculate how many vertices and indices we're going to need
	//for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( subMeshIndex );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	if(vertex_count > m_VertexBufferSize)
	{
		delete [] m_pVertexBuffer;
		m_pVertexBuffer = new Ogre::Vector3[vertex_count];
		m_VertexBufferSize = vertex_count;
	}

	if(index_count > m_IndexBufferSize)
	{
		delete [] m_pIndexBuffer;
		m_pIndexBuffer = new unsigned long[index_count];
		m_IndexBufferSize = index_count;
	}

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	//for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(subMeshIndex);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				m_pVertexBuffer[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				m_pIndexBuffer[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				m_pIndexBuffer[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
			}
		}

		ibuf->unlock();
		current_offset = next_offset;
	}
	index_count = index_offset;
}

void	PickEntity::_getMeshData( const Ogre::MeshPtr mesh, size_t& vertex_count, size_t& index_count, const Ogre::Vector3& position, const Ogre::Quaternion& orient, const Ogre::Vector3& scale )
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	if(vertex_count > m_VertexBufferSize)
	{
		delete [] m_pVertexBuffer;
		m_pVertexBuffer = new Ogre::Vector3[vertex_count];
		m_VertexBufferSize = vertex_count;
	}

	if(index_count > m_IndexBufferSize)
	{
		delete [] m_pIndexBuffer;
		m_pIndexBuffer = new unsigned long[index_count];
		m_IndexBufferSize = index_count;
	}

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				m_pVertexBuffer[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				m_pIndexBuffer[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				m_pIndexBuffer[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
			}
		}

		ibuf->unlock();
		current_offset = next_offset;
	}
	index_count = index_offset;
}

bool	PickEntity::_pickSubEntity( const Ogre::Vector2& vPoint1, Ogre::SubEntity** pResult, Ogre::Vector3 &hitposition, unsigned long mask )
{
	if ( m_pSceneMgr == NULL || m_pCamera == NULL )
	{
		return false;
	}

	Ogre::Ray cameraRay = m_pCamera->getCameraToViewportRay( vPoint1.x, vPoint1.y );
	m_pSingleSceneQuery->setRay( cameraRay );
	m_pSingleSceneQuery->setQueryMask( mask );
	m_pSingleSceneQuery->setSortByDistance( true );

	unsigned int visibilityMask = m_pSceneMgr->getVisibilityMask();

	if ( m_pSingleSceneQuery->execute().size() <= 0 ) 
	{
		return false;
	}

	Ogre::Real closest_distance = -1.f;
	Ogre::Vector3 closest_result;
	Ogre::RaySceneQueryResult &query_result = m_pSingleSceneQuery->getLastResults();

	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		// stop checking if we have found a raycast hit that is closer
		// than all remaining entities
		if ((closest_distance >= 0.0f) && (closest_distance < query_result[qr_idx].distance))
			break;

		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL) && (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
		{
			// get the entity to check
			Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

			if(!(pentity->getVisibilityFlags() & visibilityMask))
				continue;

			// mesh data to retrieve
			size_t vertex_count;
			size_t index_count;

			// get the mesh information
			for ( unsigned int index = 0; index < pentity->getNumSubEntities(); ++index )
			{
				_getSubMeshData( index, pentity->getMesh(), vertex_count, index_count, 
					pentity->getParentNode()->_getDerivedPosition(),
					pentity->getParentNode()->_getDerivedOrientation(),
					pentity->getParentNode()->_getDerivedScale());

				// test for hitting individual triangles on the mesh
				bool new_closest_found = false;
				for (int i = 0; i < static_cast<int>(index_count); i += 3)
				{
					// check for a hit against this triangle
					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(cameraRay, m_pVertexBuffer[m_pIndexBuffer[i]],
						m_pVertexBuffer[m_pIndexBuffer[i+1]], m_pVertexBuffer[m_pIndexBuffer[i+2]], true, true);

					// if it was a hit check if its the closest
					if (hit.first)
					{
						if ((closest_distance < 0.0f) || (hit.second < closest_distance))
						{
							// this is the closest so far, save it off
							closest_distance = hit.second;
							new_closest_found = true;
						}
					}
				}

				// if we found a new closest raycast for this object, update the
				// closest_result before moving on to the next object.
				if (new_closest_found)
				{
					closest_result = cameraRay.getPoint(closest_distance);
					(*pResult) = pentity->getSubEntity( index );
				}
			}
		}
	}

	m_pSingleSceneQuery->clearResults();

	if ( closest_distance != -1.f )
	{
		hitposition = closest_result;
		return true;
	}

	return false;
}

Str_Entity	PickEntity::_pickRect( const Ogre::Vector2& vPoint1, const Ogre::Vector2& vPoint2, unsigned long mask )
{
	using namespace Ogre;
	Str_Entity entitisMap;
	if ( m_pSceneMgr == NULL || m_pCamera == NULL )
	{
		return entitisMap;
	}

	float mouseLeft = static_cast< float >( min( vPoint1.x, vPoint2.x ) );
	float mouseRight = static_cast< float >( max( vPoint1.x, vPoint2.x ) );
	float mouseTop = static_cast< float >( min( vPoint1.y, vPoint2.y ) );
	float mouseBottom = static_cast< float >( max( vPoint1.y, vPoint2.y ) );

	Ray rayTopLeft = m_pCamera->getCameraToViewportRay( mouseLeft, mouseTop ); 
	Ray rayTopRight = m_pCamera->getCameraToViewportRay( mouseRight, mouseTop ); 
	Ray rayBottomLeft = m_pCamera->getCameraToViewportRay( mouseLeft, mouseBottom ); 
	Ray rayBottomRight = m_pCamera->getCameraToViewportRay( mouseRight, mouseBottom ); 

	PlaneBoundedVolume vol; 
	vol.planes.push_back( Plane( rayTopLeft.getPoint(3), rayTopRight.getPoint(3), rayBottomRight.getPoint(3) ) );         // 前平面 
	vol.planes.push_back( Plane( rayTopLeft.getOrigin(), rayTopLeft.getPoint(100), rayTopRight.getPoint(100) ) );         // 顶平面 
	vol.planes.push_back( Plane( rayTopLeft.getOrigin(), rayBottomLeft.getPoint(100), rayTopLeft.getPoint(100) ) );       // 左平面 
	vol.planes.push_back( Plane( rayTopLeft.getOrigin(), rayBottomRight.getPoint(100), rayBottomLeft.getPoint(100) ) );   // 底平面 
	vol.planes.push_back( Plane( rayTopLeft.getOrigin(), rayTopRight.getPoint(100), rayBottomRight.getPoint(100) ) );     // 右平面 


	PlaneBoundedVolumeList volList; 
	volList.push_back( vol ); 

	unsigned int visibilityMask = m_pSceneMgr->getVisibilityMask();

	m_pMultiSceneQuery->setVolumes( volList );
	m_pMultiSceneQuery->setQueryMask( mask );

	SceneQueryResult& result = m_pMultiSceneQuery->execute();

	SceneQueryResultMovableList::iterator iter = result.movables.begin();
	for ( ; iter != result.movables.end(); ++iter )
	{
		MovableObject* pMovable = *iter;

		if ( pMovable == NULL )
		{
			continue;
		}

		if( !( pMovable->getVisibilityFlags() & visibilityMask ) )
			continue;

		if ( pMovable->getMovableType() == Ogre::EntityFactory::FACTORY_TYPE_NAME )
		{
			entitisMap[pMovable->getName()] = static_cast< Ogre::Entity* >( pMovable );
		}
	}

	return entitisMap;
}

Vec_Entity	PickEntity::_findAddEntitis( Str_Entity oldMap, Str_Entity newMap )
{
	Vec_Entity entityList;
	
	//	在新列表中存在. 在旧列表中不存在. 属于新增加的
	Str_Entity::iterator iter = newMap.begin();
	for ( ; iter != newMap.end(); ++iter )
	{
		Ogre::String strName = iter->first;
	
		if ( oldMap.find( strName ) == oldMap.end() )	//	未找到
		{
			entityList.push_back( iter->second );
		}
	}

	return entityList;
}

Vec_Entity	PickEntity::_findEraseEntitis( Str_Entity oldMap, Str_Entity newMap )
{
	Vec_Entity entityList;

	//	在旧列表中存在. 在新列表中不存在. 属于需要被删除的
	Str_Entity::iterator iter = oldMap.begin();
	for ( ; iter != oldMap.end(); ++iter )
	{
		Ogre::String strName = iter->first;

		if ( newMap.find( strName ) == newMap.end() )
		{
			entityList.push_back( iter->second );
		}
	}

	return entityList;
}