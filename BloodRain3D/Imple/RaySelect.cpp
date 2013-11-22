#include "RaySelect.h"
void GetMeshInformationEx(const Ogre::MeshPtr     mesh,
                          size_t&                 vertex_count,
                          Ogre::Vector3*&         vertices,
                          size_t&                 index_count,
                          unsigned long*&         indices,
                          const Ogre::Vector3&    position,
                          const Ogre::Quaternion& orient,
                          const Ogre::Vector3&    scale)
{
    bool   added_shared   = false;
    size_t current_offset = 0;
    size_t shared_offset  = 0;
    size_t next_offset    = 0;
    size_t index_offset   = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        // We only need to add the shared vertices once
        if (submesh->useSharedVertices)
        {
            if ( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared  = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices  = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh*    submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if (submesh->useSharedVertices)
            {
                added_shared  = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement*          posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char*                      vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float*                              pReal;

            for ( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData*                   index_data = submesh->indexData;
        size_t                             numTris    = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf       = index_data->indexBuffer;

        bool                               use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long*                     pLong  = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short*                    pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t                             offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris * 3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris * 3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}
bool PickEntity(Ogre::RaySceneQuery* mRaySceneQuery,
                Ogre::Ray&           ray,
                Ogre::Entity**       result,
                Ogre::Vector3&       hitpoint,
                Ogre::uint32         mask,
                bool                 excludeInVisible,
                const std::string&  excludeobject,
                Ogre::Real           max_distance)
{
    mRaySceneQuery->setRay(ray);
    // mRaySceneQuery->setQueryMask(mask);
    mRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
    mRaySceneQuery->setSortByDistance(true);

    if (mRaySceneQuery->execute().size() <= 0)
    {
        return false;
    }
    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real                 closest_distance = max_distance;
    Ogre::Vector3              closest_result;
    Ogre::RaySceneQueryResult& query_result = mRaySceneQuery->getLastResults();
    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) && (closest_distance < query_result[qr_idx].distance))
        {
            break;
        }

        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL) && (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
        {
            // get the entity to check
            Ogre::Entity*  pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

            if (excludeInVisible)
            {
                if (!pentity->getVisible())
                {
                    continue;
                }
            }
            if (pentity->getName() == excludeobject)
            {
                continue;
            }

            // mesh data to retrieve
            size_t         vertex_count;
            size_t         index_count;
            Ogre::Vector3* vertices;
            unsigned long* indices;

            // get the mesh information
            GetMeshInformationEx(pentity->getMesh(), vertex_count, vertices, index_count, indices,
                                 pentity->getParentNode()->_getDerivedPosition(),
                                 pentity->getParentNode()->_getDerivedOrientation(),
                                 pentity->getParentNode()->_getDerivedScale());

            // maybe there is a bug in GetMeshInformationEx(),when mesh is a line or a circle, the vertex_count is not multiple of 3
            //            if (index_count%3 != 0)
            //            {
            //                index_count-=index_count%3;
            //            }

            // test for hitting individual triangles on the mesh
            bool           new_closest_found = false;
            for (int i = 0; i < static_cast<int>(index_count); i += 3)
            {
                // check for a hit against this triangle
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
                                                                         vertices[indices[i + 1]], vertices[indices[i + 2]], true, true);

                // if it was a hit check if its the closest
                if (hit.first)
                {
                    if ((closest_distance < 0.0f) || (hit.second < closest_distance))
                    {
                        // this is the closest so far, save it off
                        closest_distance  = hit.second;
                        new_closest_found = true;
                    }
                }
            }

            // free the verticies and indicies memory
            delete[] vertices;
            delete[] indices;

            // if we found a new closest raycast for this object, update the
            // closest_result before moving on to the next object.
            if (new_closest_found)
            {
                closest_result = ray.getPoint(closest_distance);
                (*result)      = pentity;
            }
        }
    }

    // return the result
    if (closest_distance != max_distance)
    {
        hitpoint = closest_result;
        return true;
    }
    else
    {
        // raycast failed
        return false;
    }
}
