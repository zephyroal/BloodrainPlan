// ---------------------------------------------------------------
// modified or deveoloped by Shen Yuqing
// HUST CS 06
// syq.myth@gmail.com
// 2009
// ---------------------------------------------------------------
/*
EDITABLE TERRAIN MANAGER for Ogre
Copyright (C) 2007  Holger Frydrych <h.frydrych@gmx.de>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

As a special exception, you may use this file as part of a free software
library without restriction.  Specifically, if other files instantiate
templates or use macros or inline functions from this file, or you compile
this file and link it with other files to produce an executable, this
file does not by itself cause the resulting executable to be covered by
the GNU General Public License.  This exception does not however
invalidate any other reasons why the executable file might be covered by
the GNU General Public License.
*/

#include "Impl/ETTerrainImpl.h"
#include "Impl/ETTile.h"
#include "Impl/ETIndexHandler.h"
#include "ETTerrainInfo.h"

#include <OgreColourValue.h>
#include <OgreHardwareBufferManager.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>

using namespace Ogre;

namespace ET
{
namespace Impl
{
const unsigned short MAIN_BINDING          = 0;
const unsigned short DELTA_BINDING         = 1;
const unsigned int   MORPH_CUSTOM_PARAM_ID = 77;

Tile::Tile(const String&    name,
           SceneManager*    sm,
           TerrainImpl*     tm,
           IndexHandler*    indexHandler,
           TerrainInfo&     info,
           Options          opts,
           size_t           startx,
           size_t           startz,
           bool             bTileTerrain,
           TileTerrainInfo* pTileTerrainInfo)
    : MovableObject(name), mSceneMgr(sm), mTerrainMgr(tm),
    mIndexHandler(indexHandler), mInfo(info), mOpt(opts),
    mStartX(startx), mStartZ(startz),
    mLOD(0),
    mLODChangeMinDistSqr(opts.maxMipMapLevel),
    mLightListDirty(true),
    mTerrain(0),
    mLastNextLevel(0),
    m_bTileTerrainTile(bTileTerrain),
    mTileTerrainInfo(*pTileTerrainInfo),
    tileTerrainMaterial(NULL)
{
    if (mOpt.maxMipMapLevel < 1)
    {
        mOpt.maxMipMapLevel = 1;
    }
    // disable LOD morphing if max LOD is 1
    if (mOpt.maxMipMapLevel == 1)
    {
        mOpt.useLODMorph = false;
    }

    mCastShadows = false;

    mTileX = startx / (mOpt.tileSize - 1);
    mTileZ = startz / (mOpt.tileSize - 1);

    // todo: need to create vertex data based on grid info
    createVertexData(startx, startz);
    createIndexData();
    calculateMinLevelDist2();
}

Tile::~Tile()
{
    delete mTerrain;
}

const String&         Tile::getMovableType() const
{
    static const String type = "EditableTerrainTile";
    return type;
}

const MaterialPtr&    Tile::getMaterial() const
{
    if (  m_bTileTerrainTile )
    {
        return mTerrainMgr->getTileTerrainMaterial();
    }
    else
    {
        return mTerrainMgr->getMaterial();
    }
}

const AxisAlignedBox& Tile::getBoundingBox() const
{
    return mBounds;
}

Real Tile::getBoundingRadius() const
{
    return mBoundingRadius;
}

void Tile::getWorldTransforms(Matrix4* m) const
{
    *m = mParentNode->_getFullTransform();
}

const Quaternion&     Tile::getWorldOrientation() const
{
    return mParentNode->_getDerivedOrientation();
}

const Vector3&        Tile::getWorldPosition() const
{
    return mParentNode->_getDerivedPosition();
}


Real Tile::getSquaredViewDepth(const Camera* cam) const
{
    Vector3 diff = mCenter - cam->getDerivedPosition();
    return diff.squaredLength();
}


const LightList&      Tile::getLights() const
{
    if (mLightListDirty)
    {
        mSceneMgr->_populateLightList(mCenter, getBoundingRadius(), mLightList);
        mLightListDirty = false;
    }
    return mLightList;
}


uint32 Tile::getTypeFlags() const
{
    return SceneManager::WORLD_GEOMETRY_TYPE_MASK;
}


void Tile::emptyBuffer(HardwareVertexBufferSharedPtr buf)
{
    void* pVoid = buf->lock(HardwareBuffer::HBL_DISCARD);
    memset(pVoid, 0, mOpt.tileSize * mOpt.tileSize * sizeof(float));
    buf->unlock();
}

void Tile::createVertexData(size_t startx, size_t startz)
{
    mTerrain              = new VertexData;
    mTerrain->vertexStart = 0;
    mTerrain->vertexCount = (mOpt.tileSize - 1) * (mOpt.tileSize - 1) * 4;

    VertexDeclaration*   decl = mTerrain->vertexDeclaration;
    VertexBufferBinding* bind = mTerrain->vertexBufferBinding;

    // first we need to declare the contents of our vertex buffer
    size_t               offset = 0;
    decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    if (mOpt.vertexNormals)
    {
        decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_NORMAL);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
    }
    if (mOpt.vertexTangents)
    {
        decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_TANGENT);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
    }
    decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    offset += VertexElement::getTypeSize(VET_FLOAT2);

    decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
    offset += VertexElement::getTypeSize(VET_FLOAT2);

    decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 2);
    offset += VertexElement::getTypeSize(VET_FLOAT2);

    mMainBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(
        decl->getVertexSize(MAIN_BINDING), mTerrain->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    // bind the buffer
    bind->setBinding(MAIN_BINDING, mMainBuffer);

    // declare delta buffers, if requested
    if (mOpt.useLODMorph)
    {
        decl->addElement(DELTA_BINDING, 0, VET_FLOAT1, VES_BLEND_WEIGHTS);
    }


    // now construct the vertex buffer from the heightmap data
    size_t               endx      = startx + mOpt.tileSize - 1;
    size_t               endz      = startz + mOpt.tileSize - 1;
    Real                 minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;
    PixMap               emptyPixMap;
    emptyPixMap.bottom = 0.9f;
    emptyPixMap.left   = 0.1f;
    emptyPixMap.right  = 0.9f;
    emptyPixMap.top    = 0.1f;
    emptyPixMap.id     = mTileTerrainInfo.imageArray.size() - 1;        // 最后一块

    const VertexElement* posElem  = decl->findElementBySemantic(VES_POSITION);
    const VertexElement* texElem0 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);             // lightmap & etm

    //
    const VertexElement* texElem1 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);               // layer0
    const VertexElement* texElem2 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 2);               // layer1

    unsigned char*       pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_DISCARD));

    const int            POS_OFFSET = 0.0f;

    int                  ii = 0, jj = 0;
    for (size_t j = startz; j < endz; ++j)
    {
        ii = 0;
        for (size_t i = startx; i < endx; ++i)
        {
            float*           pPos, * pTex0;                                                     // layer lightmap && etm

            AutoTexCoord     tex[2][4];                                                         // 第一维 layer0, layer1， 第二维表示上下左右

            g_info           ginfo = mTileTerrainInfo.gridInfoArray[j * (mInfo.getWidth() - 1) + i];
            PixMap           pm[2];
            pm[0] = mTileTerrainInfo.pixMapArray[ginfo.layer0];
            if ( ginfo.layer1 != 0xffff )
            {
                pm[1] = mTileTerrainInfo.pixMapArray[ginfo.layer1];
            }
            else
            {
                pm[1] = emptyPixMap;
            }

            Ogre::TexturePtr layer0 = Ogre::TextureManager::getSingletonPtr()->getByName("<layer0>");
            assert(!layer0.isNull());

            int              imageWidth = layer0->getWidth() / SPLICE_IMAGE_SIZE;

            ImageInfo        iInfo[2];
            for ( int i = 0; i < 2; ++i )
            {
                iInfo[i] = mTileTerrainInfo.imageArray[pm[i].id];
            }

            int              row[2] = { pm[0].id / SPLICE_COL, pm[1].id / SPLICE_COL };
            int              col[2] = { pm[0].id % SPLICE_COL, pm[1].id % SPLICE_COL };

            if ( row[0] >= SPLICE_ROW || row[1] > SPLICE_ROW )
            {
                OGRE_EXCEPT(0, "row > SPLICE_ROW", "");
            }

            float            base_x[2] = { (float)col[0] / SPLICE_COL, (float)col[1] / SPLICE_COL};
            float            base_y[2] = { (float)row[0] / SPLICE_ROW, (float)row[1] / SPLICE_ROW };

            posElem->baseVertexPointerToElement(pBase, &pPos);
            texElem0->baseVertexPointerToElement(pBase, &pTex0);
            texElem1->baseVertexPointerToElement(pBase, tex[0][0].getBase());
            texElem2->baseVertexPointerToElement(pBase, tex[1][0].getBase());



            // vertex left top
            Real             height = mInfo.getOffset().y + mInfo.at(i, j) * mInfo.getScaling().y;
            *pPos++ = mInfo.getOffset().x + mInfo.getScaling().x * i - POS_OFFSET;
            *pPos++ = height;
            *pPos++ = mInfo.getOffset().z + mInfo.getScaling().z * j - POS_OFFSET;

            *pTex0++ = float(i) / (mInfo.getWidth() - 1);
            *pTex0++ = float(j) / (mInfo.getHeight() - 1);

            for ( int i = 0; i < 2; ++i )
            {
                // already resize the image to 256x256, so needn't to multi the scale value.
                // *tex[i][0].getXPtr() = base_x[i]+(double)pm[i].left*iInfo[i].col_scale/SPLICE_COL;
                // *tex[i][0].getYPtr() = base_y[i]+(double)pm[i].top*iInfo[i].row_scale/SPLICE_ROW;
                *tex[i][0].getXPtr() = base_x[i] + (double)pm[i].left / SPLICE_COL;
                *tex[i][0].getYPtr() = base_y[i] + (double)pm[i].top / SPLICE_ROW;
            }

            pBase += mMainBuffer->getVertexSize();
            posElem->baseVertexPointerToElement(pBase, &pPos);
            texElem0->baseVertexPointerToElement(pBase, &pTex0);
            texElem1->baseVertexPointerToElement(pBase, tex[0][1].getBase());
            texElem2->baseVertexPointerToElement(pBase, tex[1][1].getBase());

            // vertex right top
            height  = mInfo.getOffset().y + mInfo.at(i + 1, j) * mInfo.getScaling().y;
            *pPos++ = mInfo.getOffset().x + mInfo.getScaling().x * (i + 1) + POS_OFFSET;
            *pPos++ = height;
            *pPos++ = mInfo.getOffset().z + mInfo.getScaling().z * j - POS_OFFSET;

            *pTex0++ = float(i + 1) / (mInfo.getWidth() - 1);
            *pTex0++ = float(j) / (mInfo.getHeight() - 1);

            for ( int i = 0; i < 2; ++i )
            {
                *tex[i][1].getXPtr() = base_x[i] + (double)pm[i].right / SPLICE_COL;
                *tex[i][1].getYPtr() = base_y[i] + (double)pm[i].top / SPLICE_ROW;
            }

            pBase += mMainBuffer->getVertexSize();
            posElem->baseVertexPointerToElement(pBase, &pPos);
            texElem0->baseVertexPointerToElement(pBase, &pTex0);
            texElem1->baseVertexPointerToElement(pBase, tex[0][2].getBase());
            texElem2->baseVertexPointerToElement(pBase, tex[1][2].getBase());

            // vertex left bottom
            height  = mInfo.getOffset().y + mInfo.at(i, j + 1) * mInfo.getScaling().y;
            *pPos++ = mInfo.getOffset().x + mInfo.getScaling().x * i - POS_OFFSET;
            *pPos++ = height;
            *pPos++ = mInfo.getOffset().z + mInfo.getScaling().z * (j + 1) + POS_OFFSET;

            *pTex0++ = float(i) / (mInfo.getWidth() - 1);
            *pTex0++ = float(j + 1) / (mInfo.getHeight() - 1);
            for ( int i = 0; i < 2; ++i )
            {
                *tex[i][2].getXPtr() = base_x[i] + (double)pm[i].left / SPLICE_COL;
                *tex[i][2].getYPtr() = base_y[i] + (double)pm[i].bottom / SPLICE_ROW;
            }
            pBase += mMainBuffer->getVertexSize();
            posElem->baseVertexPointerToElement(pBase, &pPos);
            texElem0->baseVertexPointerToElement(pBase, &pTex0);
            texElem1->baseVertexPointerToElement(pBase, tex[0][3].getBase());
            texElem2->baseVertexPointerToElement(pBase, tex[1][3].getBase());

            // vertex right bottom
            height  = mInfo.getOffset().y + mInfo.at(i + 1, j + 1) * mInfo.getScaling().y;
            *pPos++ = mInfo.getOffset().x + mInfo.getScaling().x * (i + 1) + POS_OFFSET;
            *pPos++ = height;
            *pPos++ = mInfo.getOffset().z + mInfo.getScaling().z * (j + 1) + POS_OFFSET;

            *pTex0++ = float(i + 1) / (mInfo.getWidth() - 1);
            *pTex0++ = float(j + 1) / (mInfo.getHeight() - 1);

            for ( int i = 0; i < 2; ++i )
            {
                *tex[i][3].getXPtr() = base_x[i] + (double)pm[i].right / SPLICE_COL;
                *tex[i][3].getYPtr() = base_y[i] + (double)pm[i].bottom / SPLICE_ROW;
            }

            bool bIndex = ginfo.tri == 1 ? true : false;                // g_info.tri是否为1，是否为正常三角形索引
            changeGridInfoUV(tex[0][0], tex[0][1], tex[0][2], tex[0][3], ginfo.op0, bIndex);

            if ( pm[1].id != mTileTerrainInfo.imageArray.size() - 1)
            {
                changeGridInfoUV(tex[1][0], tex[1][1], tex[1][2], tex[1][3], ginfo.op1, bIndex);
            }

            // if( m_bTileTerrainTile ){
            //	//g_info ginfo = mTileTerrainInfo.info[startz*(mInfo.getWidth()-1)+startx];
            //	//PixMap pm = mTileTerrainInfo.pixMapArray[ginfo.layer0];
            //	//int row = pm.id/4;
            //	//int col = pm.id%4;
            //	//if( row > 7 )
            //	//	OGRE_EXCEPT( 0, "row > 7","" );
            //	//float base_x = col/4;
            //	//float base_y = row/8;
            //	//ImageInfo iInfo = mTileTerrainInfo.imageArray[ginfo.layer0];

            //	//*pTex1++ = pm.left + ( mOpt.tileSize-1)*(pm.right-pm.left);
            //	//*pTex1++ = pm.top + ( mOpt.tileSize-1)*(pm.bottom-pm.top);
            //	*pTex1++ = 0;
            //	*pTex1++ = 0;
            // }
            // else {
            //	*pTex1++ = 0;
            //	*pTex1++ = 0;
            // }

            if (height < minHeight)
            {
                minHeight = height;
            }
            if (height > maxHeight)
            {
                maxHeight = height;
            }

            // ...
            pBase += mMainBuffer->getVertexSize();
            ii++;
        }
        jj++;
    }
    mMainBuffer->unlock();

    // set the extents of this terrain tile
    mBounds.setExtents(mInfo.getOffset().x + mInfo.getScaling().x * startx, minHeight,
                       mInfo.getOffset().z + mInfo.getScaling().z * startz,
                       mInfo.getOffset().x + mInfo.getScaling().x * endx, maxHeight,
                       mInfo.getOffset().z + mInfo.getScaling().z * (endz));

    mCenter = mBounds.getCenter();

    mBoundingRadius = (mBounds.getMaximum() - mCenter).length();

    // if using LOD morphing, create the delta buffers now (they'll be filled in _calculateMinLevelDist2)
    if (mOpt.useLODMorph)
    {
        for (unsigned int i = 0; i < mOpt.maxMipMapLevel - 1; ++i)
        {
            HardwareVertexBufferSharedPtr buf = HardwareBufferManager::getSingleton().createVertexBuffer(
                VertexElement::getTypeSize(VET_FLOAT1), mOpt.tileSize * mOpt.tileSize,
                HardwareBuffer::HBU_STATIC_WRITE_ONLY);
            emptyBuffer(buf);
            mDeltaBuffers.push_back(buf);
        }
    }


    // calc vertex normals, if necessary
    if (mOpt.vertexNormals)
    {
        calculateVertexNormals();
    }
    // calc vertex tangents, if necessary
    if (mOpt.vertexTangents)
    {
        calculateVertexTangents();
    }
}

void Tile::createIndexData()
{
    mIndexData              = new IndexData;
    mIndexData->indexStart  = 0;
    mIndexData->indexCount  = (mOpt.tileSize - 1) * (mOpt.tileSize - 1) * 6;
    mIndexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT,
        mIndexData->indexCount,
        HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    int     gridSize = mOpt.tileSize - 1;
    ushort* pIdx     = static_cast<ushort*>(mIndexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
    for (uint y = 0; y < gridSize; y++)
    {
        for (uint x = 0; x < gridSize; x++)
        {
            g_info info              = mTileTerrainInfo.gridInfoArray[(y + (mTileZ * gridSize)) * mTileTerrainInfo.width + gridSize * mTileX + x];
            ushort iIndexTopLeft     = (x + y * gridSize) * 4;
            ushort iIndexTopRight    = iIndexTopLeft + 1;
            ushort iIndexBottomLeft  = iIndexTopLeft + 2;
            ushort iIndexBottomRight = iIndexTopLeft + 3;

            if ( 0 == info.tri )
            {
                *pIdx++ = iIndexTopLeft;
                *pIdx++ = iIndexBottomLeft;
                *pIdx++ = iIndexTopRight;
                *pIdx++ = iIndexBottomLeft;
                *pIdx++ = iIndexBottomRight;
                *pIdx++ = iIndexTopRight;
            }
            else
            {
                *pIdx++ = iIndexBottomLeft;
                *pIdx++ = iIndexBottomRight;
                *pIdx++ = iIndexTopLeft;
                *pIdx++ = iIndexBottomRight;
                *pIdx++ = iIndexTopRight;
                *pIdx++ = iIndexTopLeft;
            }
            // if( x != gridSize-1 )
            // {
            //	*pIdx++ = iIndexTopRight;
            //	*pIdx++ = iIndexBottomRight;
            //	*pIdx++ = iIndexBottomRight+1;
            //
            //	*pIdx++ = iIndexBottomRight;
            //	*pIdx++ = iIndexBottomRight+3;
            //	*pIdx++ = iIndexBottomRight+1;
            // }
        }
    }
    mIndexData->indexBuffer->unlock();
}

void Tile::calculateVertexNormals()
{
    // set the vertex normals of the tile

    size_t               startx = mStartX;
    size_t               startz = mStartZ;
    // ..
    size_t               endx      = startx + mOpt.tileSize - 1;
    size_t               endz      = startz + mOpt.tileSize - 1;
    Real                 minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;

    const VertexElement* normElem = mTerrain->vertexDeclaration->findElementBySemantic(VES_NORMAL);
    unsigned char*       pBase    = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_NORMAL));

    for (size_t j = startz; j < endz; ++j)
    {
        for (size_t i = startx; i < endx; ++i)
        {
            float*  pNorm;
            normElem->baseVertexPointerToElement(pBase, &pNorm);

            Vector3 normal1 = mInfo.getNormalAt(mInfo.vertexToPosX((int)i), mInfo.vertexToPosZ((int)j));
            *pNorm++ = normal1.x;
            *pNorm++ = normal1.y;
            *pNorm++ = normal1.z;

            pBase += mMainBuffer->getVertexSize();
            normElem->baseVertexPointerToElement(pBase, &pNorm);

            Vector3 normal2 = mInfo.getNormalAt(mInfo.vertexToPosX((int)i + 1), mInfo.vertexToPosZ((int)j));
            *pNorm++ = normal2.x;
            *pNorm++ = normal2.y;
            *pNorm++ = normal2.z;

            pBase += mMainBuffer->getVertexSize();
            normElem->baseVertexPointerToElement(pBase, &pNorm);

            Vector3 normal3 = mInfo.getNormalAt(mInfo.vertexToPosX((int)i), mInfo.vertexToPosZ((int)j + 1));
            *pNorm++ = normal3.x;
            *pNorm++ = normal3.y;
            *pNorm++ = normal3.z;

            pBase += mMainBuffer->getVertexSize();
            normElem->baseVertexPointerToElement(pBase, &pNorm);

            Vector3 normal4 = mInfo.getNormalAt(mInfo.vertexToPosX((int)i + 1), mInfo.vertexToPosZ((int)j + 1));
            *pNorm++ = normal4.x;
            *pNorm++ = normal4.y;
            *pNorm++ = normal4.z;

            pBase += mMainBuffer->getVertexSize();
        }
    }
    mMainBuffer->unlock();
}

void Tile::calculateVertexTangents()
{
    // set the vertex tangents of the tile
    size_t               startx = mStartX;
    size_t               startz = mStartZ;
    size_t               endx   = startx + mOpt.tileSize - 1;
    size_t               endz   = startz + mOpt.tileSize - 1;
    // Real minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;

    const VertexElement* normElem = mTerrain->vertexDeclaration->findElementBySemantic(VES_TANGENT);
    unsigned char*       pBase    = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_NORMAL));

    for (size_t j = startz; j < endz; ++j)
    {
        for (size_t i = startx; i < endx; ++i)
        {
            float*  pTan;

            normElem->baseVertexPointerToElement(pBase, &pTan);
            Vector3 tangent1 = mInfo.getTangentAt(mInfo.vertexToPosX((int)i), mInfo.vertexToPosZ((int)j));

            *pTan++ = tangent1.x;
            *pTan++ = tangent1.y;
            *pTan++ = tangent1.z;

            pBase += mMainBuffer->getVertexSize();
            normElem->baseVertexPointerToElement(pBase, &pTan);
            Vector3 tangent2 = mInfo.getTangentAt(mInfo.vertexToPosX((int)i + 1), mInfo.vertexToPosZ((int)j));

            *pTan++ = tangent2.x;
            *pTan++ = tangent2.y;
            *pTan++ = tangent2.z;

            pBase += mMainBuffer->getVertexSize();
            normElem->baseVertexPointerToElement(pBase, &pTan);
            Vector3 tangent3 = mInfo.getTangentAt(mInfo.vertexToPosX((int)i), mInfo.vertexToPosZ((int)j + 1));

            *pTan++ = tangent3.x;
            *pTan++ = tangent3.y;
            *pTan++ = tangent3.z;

            pBase += mMainBuffer->getVertexSize();
            normElem->baseVertexPointerToElement(pBase, &pTan);
            Vector3 tangent4 = mInfo.getTangentAt(mInfo.vertexToPosX((int)i + 1), mInfo.vertexToPosZ((int)j + 1));

            *pTan++ = tangent4.x;
            *pTan++ = tangent4.y;
            *pTan++ = tangent4.z;

            pBase += mMainBuffer->getVertexSize();
        }
    }
    mMainBuffer->unlock();
}
/*
    void Tile::createVertexData(size_t startx, size_t startz)
    {
      mTerrain = new VertexData;
      mTerrain->vertexStart = 0;
      mTerrain->vertexCount = mOpt.tileSize * mOpt.tileSize;

      VertexDeclaration* decl = mTerrain->vertexDeclaration;
      VertexBufferBinding* bind = mTerrain->vertexBufferBinding;

      // first we need to declare the contents of our vertex buffer
      size_t offset = 0;
      decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_POSITION);
      offset += VertexElement::getTypeSize(VET_FLOAT3);
      if (mOpt.vertexNormals)
      {

                  decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_NORMAL);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
      }
      if (mOpt.vertexTangents)
      {
        decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_TANGENT);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
      }
      decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
          offset += VertexElement::getTypeSize(VET_FLOAT2);

          decl->addElement( MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1 );
      offset += VertexElement::getTypeSize(VET_FLOAT2);

      mMainBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(
        decl->getVertexSize(MAIN_BINDING), mTerrain->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

      // bind the buffer
      bind->setBinding(MAIN_BINDING, mMainBuffer);

      // declare delta buffers, if requested
      if (mOpt.useLODMorph)
      {
        decl->addElement(DELTA_BINDING, 0, VET_FLOAT1, VES_BLEND_WEIGHTS);
      }


      // now construct the vertex buffer from the heightmap data
      size_t endx = startx + mOpt.tileSize;
      size_t endz = startz + mOpt.tileSize;
      Real minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;

      const VertexElement* posElem = decl->findElementBySemantic(VES_POSITION);
      const VertexElement* texElem0 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);

          //
          const VertexElement* texElem1 = decl->findElementBySemantic( VES_TEXTURE_COORDINATES, 1 );

      unsigned char* pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_DISCARD));

          int ii = 0, jj = 0;
      for (size_t j = startz; j < endz; ++j)
      {
                  ii = 0;
        for (size_t i = startx; i < endx; ++i)
        {
          float* pPos, * pTex0, * pTex1;
          posElem->baseVertexPointerToElement(pBase, &pPos);
          texElem0->baseVertexPointerToElement(pBase, &pTex0);
                  texElem1->baseVertexPointerToElement(pBase, &pTex1);

                  Real height = mInfo.getOffset().y + mInfo.at(i, j) * mInfo.getScaling().y;
          *pPos++ = mInfo.getOffset().x + mInfo.getScaling().x * i;
          *pPos++ = height;
          *pPos++ = mInfo.getOffset().z + mInfo.getScaling().z * j;

          *pTex0++ = float(i) / (mInfo.getWidth() - 1);
          *pTex0++ = float(j) / (mInfo.getHeight() - 1);

                  if( m_bTileTerrainTile ){
                                //g_info ginfo = mTileTerrainInfo.info[startz*(mInfo.getWidth()-1)+startx];
                                //PixMap pm = mTileTerrainInfo.pixMapArray[ginfo.layer0];
                                //int row = pm.id/4;
                                //int col = pm.id%4;
                                //if( row > 7 )
                                //	OGRE_EXCEPT( 0, "row > 7","" );
                                //float base_x = col/4;
                                //float base_y = row/8;
                                //ImageInfo iInfo = mTileTerrainInfo.imageArray[ginfo.layer0];

                                //*pTex1++ = pm.left + ( mOpt.tileSize-1)*(pm.right-pm.left);
                                //*pTex1++ = pm.top + ( mOpt.tileSize-1)*(pm.bottom-pm.top);
                          *pTex1++ = 0;
                          *pTex1++ = 0;
                  }
                  else {
                                *pTex1++ = 0;
                                *pTex1++ = 0;
                  }

                  if (height < minHeight)
            minHeight = height;
          if (height > maxHeight)
            maxHeight = height;

          pBase += mMainBuffer->getVertexSize();
                  ii++;
                }
                jj++;
      }
      mMainBuffer->unlock();

      // set the extents of this terrain tile
      mBounds.setExtents(mInfo.getOffset().x + mInfo.getScaling().x * startx, minHeight,
        mInfo.getOffset().z + mInfo.getScaling().z * startz,
        mInfo.getOffset().x + mInfo.getScaling().x * endx, maxHeight,
        mInfo.getOffset().z + mInfo.getScaling().z * (endz));

      mCenter = mBounds.getCenter();

      mBoundingRadius = (mBounds.getMaximum() - mCenter).length();

      // if using LOD morphing, create the delta buffers now (they'll be filled in _calculateMinLevelDist2)
      if (mOpt.useLODMorph)
      {
        for (unsigned int i = 0; i < mOpt.maxMipMapLevel-1; ++i)
        {
          HardwareVertexBufferSharedPtr buf = HardwareBufferManager::getSingleton().createVertexBuffer(
            VertexElement::getTypeSize(VET_FLOAT1), mOpt.tileSize*mOpt.tileSize,
            HardwareBuffer::HBU_STATIC_WRITE_ONLY);
          emptyBuffer(buf);
          mDeltaBuffers.push_back(buf);
        }
      }


      // calc vertex normals, if necessary
      if (mOpt.vertexNormals)
        calculateVertexNormals();
      // calc vertex tangents, if necessary
      if (mOpt.vertexTangents)
        calculateVertexTangents();
    }

*/

/*
    void Tile::calculateVertexNormals()
    {
      // set the vertex normals of the tile

      size_t startx = mStartX;
      size_t startz = mStartZ;
      size_t endx = startx + mOpt.tileSize;
      size_t endz = startz + mOpt.tileSize;
      Real minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;

      const VertexElement* normElem = mTerrain->vertexDeclaration->findElementBySemantic(VES_NORMAL);
      unsigned char* pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_NORMAL));

      for (size_t j = startz; j < endz; ++j)
      {
        for (size_t i = startx; i < endx; ++i)
        {
          float* pNorm;
          normElem->baseVertexPointerToElement(pBase, &pNorm);

          Vector3 normal = mInfo.getNormalAt(mInfo.vertexToPosX((int)i), mInfo.vertexToPosZ((int)j));
          *pNorm++ = normal.x;
          *pNorm++ = normal.y;
          *pNorm++ = normal.z;

          pBase += mMainBuffer->getVertexSize();
        }
      }
      mMainBuffer->unlock();
    }

*/
/** Addition by SongOfTheWeave */
/*
    void Tile::calculateVertexTangents()
    {
      // set the vertex tangents of the tile
      size_t startx = mStartX;
      size_t startz = mStartZ;
      size_t endx = startx + mOpt.tileSize;
      size_t endz = startz + mOpt.tileSize;
      //Real minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;

      const VertexElement* normElem = mTerrain->vertexDeclaration->findElementBySemantic(VES_TANGENT);
      unsigned char* pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_NORMAL));

      for (size_t j = startz; j < endz; ++j)
      {
        for (size_t i = startx; i < endx; ++i)
        {
          float* pTan;
          normElem->baseVertexPointerToElement(pBase, &pTan);

          Vector3 tangent = mInfo.getTangentAt(mInfo.vertexToPosX((int)i), mInfo.vertexToPosZ((int)j));
          *pTan++ = tangent.x;
          *pTan++ = tangent.y;
          *pTan++ = tangent.z;

          pBase += mMainBuffer->getVertexSize();
        }
      }
      mMainBuffer->unlock();
    }
        */


IndexData* Tile::getIndexData()
{
    IndexData* data = mIndexHandler->requestIndexData(mLOD,
                                                      mTerrainMgr->getNeighbourState(mTileX, mTileZ));


    return data;
}


void Tile::getRenderOperation(RenderOperation& op)
{
    op.useIndexes    = true;
    op.operationType = RenderOperation::OT_TRIANGLE_LIST;
    op.vertexData    = mTerrain;
    op.indexData     = mIndexData; // getIndexData();
}

void Tile::_updateRenderQueue(RenderQueue* queue)
{
    mLightListDirty = true;
    queue->addRenderable(this, mRenderQueueID);
}


void Tile::_updateCustomGpuParameter(const GpuProgramParameters::AutoConstantEntry& constEntry,
                                     GpuProgramParameters*                          params) const
{
    if (constEntry.data == MORPH_CUSTOM_PARAM_ID)
    {
        params->_writeRawConstant(constEntry.physicalIndex, mLODMorphFactor);
    }
    else
    {
        Renderable::_updateCustomGpuParameter(constEntry, params);
    }
}


Vector3 Tile::getVector(size_t x, size_t z) const
{
    float posX = mInfo.getOffset().x + (mStartX + x) * mInfo.getScaling().x;
    float posZ = mInfo.getOffset().z + (mStartZ + z) * mInfo.getScaling().z;
    return Vector3(posX, mInfo.getHeightAt(posX, posZ), posZ);
}

void Tile::calculateMinLevelDist2()
{
    Real C = mOpt.factorC;

    // LOD 0 has no distance
    mLODChangeMinDistSqr[0] = 0.0f;


    for (unsigned int level = 1; level < mOpt.maxMipMapLevel; ++level)
    {
        mLODChangeMinDistSqr[level] = 0.0f;

        size_t step       = 1 << level;
        size_t higherstep = step >> 1;

        float* pDeltas = 0;
        // for LOD morphing, lock the according delta buffer now
        if (mOpt.useLODMorph)
        {
            // indexed at LOD-1, because there are only maxLOD-1 transitions between LODs...
            emptyBuffer(mDeltaBuffers[level - 1]);
            pDeltas = static_cast<float*>(mDeltaBuffers[level - 1]->lock(HardwareBuffer::HBL_NORMAL));
        }

        // for every vertex that is not used in the current LOD we calculate its interpolated
        // height and compare against its actual height. the largest difference of a vertex
        // is then used to determine the minimal distance for this LOD.
        for (size_t j = 0; j < mOpt.tileSize - step; j += step)
        {
            for (size_t i = 0; i < mOpt.tileSize - step; i += step)
            {
                Vector3 v1 = getVector(i, j);
                Vector3 v2 = getVector(i + step, j);
                Vector3 v3 = getVector(i, j + step);
                Vector3 v4 = getVector(i + step, j + step);
                Plane   t1(v1, v3, v2);
                Plane   t2(v2, v3, v4);

                size_t  zubound = (j == (mOpt.tileSize - step) ? step : step - 1);
                for (size_t z = 0; z <= zubound; ++z)
                {
                    size_t xubound = (i == (mOpt.tileSize - step) ? step : step - 1);
                    for (size_t x = 0; x < xubound; ++x)
                    {
                        size_t  fulldetailx = i + x;
                        size_t  fulldetailz = j + z;
                        if (fulldetailx % step == 0 && fulldetailz % step == 0)
                        {
                            continue;
                        }

                        Real    zpct = Real(z) / Real(step);
                        Real    xpct = Real(x) / Real(step);

                        Vector3 actualPos = getVector(fulldetailx, fulldetailz);
                        Real    interp_h;
                        if (xpct + zpct <= 1.0f)
                        {
                            interp_h = (
                                -(t1.normal.x * actualPos.x)
                                - t1.normal.z * actualPos.z
                                - t1.d) / t1.normal.y;
                        }
                        else
                        {
                            interp_h = (
                                -(t2.normal.x * actualPos.x)
                                - t2.normal.z * actualPos.z
                                - t2.d) / t2.normal.y;
                        }

                        Real    actual_h = getVector(fulldetailx, fulldetailz).y;
                        Real    delta    = fabs(interp_h - actual_h);

                        Real    D2 = delta * delta * C * C;

                        if (mLODChangeMinDistSqr[level] < D2)
                        {
                            mLODChangeMinDistSqr[level] = D2;
                        }

                        // for LOD morphing, store the difference in the delta buffer
                        if (mOpt.useLODMorph &&
                            fulldetailx != 0 && fulldetailx != (mOpt.tileSize - 1) &&
                            fulldetailz != 0 && fulldetailz != (mOpt.tileSize - 1))
                        {
                            pDeltas[fulldetailx + (fulldetailz * mOpt.tileSize)] = interp_h - actual_h;
                        }
                    }
                }
            }
        }

        // unlock delta buffers
        if (mOpt.useLODMorph)
        {
            mDeltaBuffers[level - 1]->unlock();
        }
    }


    // post validate
    for (unsigned int i = 1; i < mOpt.maxMipMapLevel; ++i)
    {
        // ensure level distances are increasing
        if (mLODChangeMinDistSqr[i] < mLODChangeMinDistSqr[i - 1])
        {
            mLODChangeMinDistSqr[i] = mLODChangeMinDistSqr[i - 1];
        }
    }
}


void Tile::_notifyCurrentCamera(Camera* cam)
{
    MovableObject::_notifyCurrentCamera(cam);

    Vector3               cpos = cam->getDerivedPosition();
    const AxisAlignedBox& aabb = getWorldBoundingBox(true);
    Vector3               diff(0, 0, 0);
    diff.makeFloor(cpos - aabb.getMinimum());
    diff.makeCeil(cpos - aabb.getMaximum());

    // find the LOD to use for this tile
    Real                  L = diff.squaredLength();
    mLOD = mOpt.maxMipMapLevel - 1;
    for (unsigned int i = 1; i < mOpt.maxMipMapLevel; ++i)
    {
        if (mLODChangeMinDistSqr[i] > L)
        {
            mLOD = i - 1;
            break;
        }
    }

    if (mOpt.useLODMorph)
    {
        // find the next LOD after the current one
        unsigned int nextLevel = mLOD + 1;
        for (unsigned int i = nextLevel; i < mOpt.maxMipMapLevel; ++i)
        {
            if (mLODChangeMinDistSqr[i] > mLODChangeMinDistSqr[mLOD])
            {
                nextLevel = i;
                break;
            }
        }

        // determine the LOD morph factor between the two LODs
        if (nextLevel == mOpt.maxMipMapLevel)
        {
            mLODMorphFactor = 0;
        }
        else
        {
            Real range   = mLODChangeMinDistSqr[nextLevel] - mLODChangeMinDistSqr[mLOD];
            Real percent = (L - mLODChangeMinDistSqr[mLOD]) / range;
            Real rescale = 1.0f / (1.0f - mOpt.lodMorphStart);
            mLODMorphFactor = std::max((percent - mOpt.lodMorphStart) * rescale, Real(0));
        }

        if (mLastNextLevel != nextLevel)
        {
            if (nextLevel != mOpt.maxMipMapLevel)
            {
                mTerrain->vertexBufferBinding->setBinding(DELTA_BINDING, mDeltaBuffers[nextLevel - 1]);
            }
            else
            {
                // bind dummy
                mTerrain->vertexBufferBinding->setBinding(DELTA_BINDING, mDeltaBuffers[0]);
            }
        }
        mLastNextLevel = nextLevel;
    }
}


unsigned int Tile::getLOD() const
{
    return mLOD;
}


void Tile::updateTerrain(size_t startx, size_t startz, size_t endx, size_t endz)
{
    // determine the area of this tile that needs to be updated
    size_t               fromX = std::max(mStartX, startx);
    size_t               fromZ = std::max(mStartZ, startz);
    size_t               toX   = std::min(endx, mStartX + mOpt.tileSize - 1);
    size_t               toZ   = std::min(endz, mStartZ + mOpt.tileSize - 1);

    const VertexElement* posElem = mTerrain->vertexDeclaration->findElementBySemantic(VES_POSITION);
    unsigned char*       pBase   = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_NORMAL));

    // update all necessary vertices
    for (size_t j = fromZ; j <= toZ; ++j)
    {
        for (size_t i = fromX; i <= toX; ++i)
        {
            size_t         tX = i - mStartX;
            size_t         tZ = j - mStartZ;
            // etm deform
            // unsigned char* pBasePos = pBase + (tZ*mOpt.tileSize + tX) * mMainBuffer->getVertexSize();

            // Real height = mInfo.getOffset().y + mInfo.at(i, j) * mInfo.getScaling().y;
            // float* pPos;
            // posElem->baseVertexPointerToElement(pBasePos, &pPos);
            // pPos[1] = height;

            Real           height = mInfo.getOffset().y + mInfo.at(i, j) * mInfo.getScaling().y;


            unsigned char* p1 = pBase + (tZ * (mOpt.tileSize - 1) + tX) * 4 * mMainBuffer->getVertexSize();
            float*         pPos;

            if ( j != mStartZ + mOpt.tileSize - 1 )
            {
                if ( i < mStartX + mOpt.tileSize - 1 )
                {
                    posElem->baseVertexPointerToElement(p1, &pPos);
                    pPos[1] = height;
                }
                if ( i > mStartX )
                {
                    unsigned char* p2 = p1 - 3 * mMainBuffer->getVertexSize();
                    posElem->baseVertexPointerToElement(p2, &pPos);
                    pPos[1] = height;
                }
            }

            if ( j != mStartZ )
            {
                if ( i < mStartX + mOpt.tileSize - 1 )
                {
                    unsigned char* p3 = p1 - ((mOpt.tileSize - 1) * 4 - 2) * mMainBuffer->getVertexSize();
                    posElem->baseVertexPointerToElement(p3, &pPos);
                    pPos[1] = height;
                }
                if ( i > mStartX )
                {
                    unsigned char* p4 = p1 - ((mOpt.tileSize - 1) * 4 + 1) * mMainBuffer->getVertexSize();
                    posElem->baseVertexPointerToElement(p4, &pPos);
                    pPos[1] = height;
                }
            }
        }
    }

    mMainBuffer->unlock();

    // update the extents of this terrain tile
    size_t ex = mStartX + mOpt.tileSize;
    size_t ez = mStartZ + mOpt.tileSize;
    // find min and max heights
    Real   minHeight = mInfo.getOffset().y + mInfo.getScaling().y, maxHeight = mInfo.getOffset().y;
    for (size_t j = mStartZ; j < ez; ++j)
    {
        for (size_t i = mStartX; i < ex; ++i)
        {
            Real height = mInfo.getOffset().y + mInfo.at(i, j) * mInfo.getScaling().y;
            if (height < minHeight)
            {
                minHeight = height;
            }
            if (height > maxHeight)
            {
                maxHeight = height;
            }
        }
    }
    mBounds.setMinimumY(minHeight);
    mBounds.setMaximumY(maxHeight);

    mCenter = mBounds.getCenter();

    mBoundingRadius = (mBounds.getMaximum() - mCenter).length();

    // recalculate the distances at which to switch LOD
    calculateMinLevelDist2();

    // recalculate vertex normals, if necessary
    if (mOpt.vertexNormals)
    {
        calculateVertexNormals();
    }
}

void Tile::updateTileTexture(int x, int y)
{
    AutoTexCoord         tex[2][4];                                             // 第一维 layer0, layer1， 第二维表示上下左右

    g_info               ginfo = mTileTerrainInfo.gridInfoArray[y * (mInfo.getWidth() - 1) + x];
    PixMap               pm[2];
    pm[0] = mTileTerrainInfo.pixMapArray[ginfo.layer0];
    if ( ginfo.layer1 != 0xffff )
    {
        pm[1] = mTileTerrainInfo.pixMapArray[ginfo.layer1];
    }
    else
    {
        pm[1].id = -1;
    }

    ImageInfo            iInfo[2];
    for ( int i = 0; i < 2; ++i )
    {
        if ( pm[i].id != -1 )
        {
            iInfo[i] = mTileTerrainInfo.imageArray[pm[i].id];
        }
        else
        {
            iInfo[i].col_scale = 0;
            iInfo[i].row_scale = 0;
        }
    }

    int                  row[2] = { pm[0].id / 4, pm[1].id / 4 };
    int                  col[2] = { pm[0].id % 4, pm[1].id % 4 };

    if ( row[0] > 7 || row[1] > 7 )
    {
        OGRE_EXCEPT(0, "row > 7", "");
    }

    float                base_x[2] = { (float)col[0] / 4, (float)col[1] / 4 };
    float                base_y[2] = { (float)row[0] / 8, (float)row[1] / 8 };


    VertexDeclaration*   decl     = mTerrain->vertexDeclaration;
    const VertexElement* texElem1 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);               // layer0
    const VertexElement* texElem2 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 2);               // layer1

    unsigned char*       pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_DISCARD));
    pBase += (y * (mInfo.getWidth() - 1) + x) * 4 * mMainBuffer->getVertexSize();

    texElem1->baseVertexPointerToElement(pBase, tex[0][0].getBase());
    texElem2->baseVertexPointerToElement(pBase, tex[1][0].getBase());

    for ( int i = 0; i < 2; ++i )
    {
        if ( pm[i].id != -1 )
        {
            *tex[i][0].getXPtr() = base_x[i] + pm[i].left * iInfo[i].col_scale / 4;
            *tex[i][0].getYPtr() = base_y[i] + pm[i].top * iInfo[i].row_scale / 8;
        }
    }

    pBase += mMainBuffer->getVertexSize();
    texElem1->baseVertexPointerToElement(pBase, tex[0][1].getBase());
    texElem2->baseVertexPointerToElement(pBase, tex[1][1].getBase());

    for ( int i = 0; i < 2; ++i )
    {
        if ( pm[i].id != -1 )
        {
            *tex[i][1].getXPtr() = base_x[i] + pm[i].right * iInfo[i].col_scale / 4;
            *tex[i][1].getYPtr() = base_y[i] + pm[i].top * iInfo[i].row_scale / 8;
        }
    }

    pBase += mMainBuffer->getVertexSize();
    texElem1->baseVertexPointerToElement(pBase, tex[0][2].getBase());
    texElem2->baseVertexPointerToElement(pBase, tex[1][2].getBase());

    for ( int i = 0; i < 2; ++i )
    {
        if ( pm[i].id != -1 )
        {
            *tex[i][2].getXPtr() = base_x[i] + pm[i].left * iInfo[i].col_scale / 4;
            *tex[i][2].getYPtr() = base_y[i] + pm[i].bottom * iInfo[i].row_scale / 8;
        }
    }

    pBase += mMainBuffer->getVertexSize();
    texElem1->baseVertexPointerToElement(pBase, tex[0][3].getBase());
    texElem2->baseVertexPointerToElement(pBase, tex[1][3].getBase());

    for ( int i = 0; i < 2; ++i )
    {
        if ( pm[i].id != -1 )
        {
            *tex[i][3].getXPtr() = base_x[i] + pm[i].right * iInfo[i].col_scale / 4;
            *tex[i][3].getYPtr() = base_y[i] + pm[i].bottom * iInfo[i].row_scale / 8;
        }
    }

    mMainBuffer->unlock();
}

void Tile::setTileTexture(int x, int y, int layer, int imageId, TexCoord* coord)
{
    AutoTexCoord         tex[4];                                                // 第一维 layer0, layer1， 第二维表示上下左右

    ImageInfo            iInfo = mTileTerrainInfo.imageArray[imageId];

    int                  row = imageId / 4; // 所用brush在atlas中所处行列
    int                  col = imageId % 4;

    if ( row > 7 )
    {
        OGRE_EXCEPT(0, "row > 7", "");
    }

    float                base_x = (float)col / 4;
    float                base_y = (float)row / 8;


    VertexDeclaration*   decl = mTerrain->vertexDeclaration;
    const VertexElement* texElem;

    if ( layer == 0 )
    {
        texElem = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);                   // layer0
    }
    else
    {
        texElem = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 2);                   // layer1
    }
    unsigned char*       pBase = static_cast<unsigned char*>(mMainBuffer->lock(HardwareBuffer::HBL_DISCARD));
    pBase += (y * (mOpt.tileSize - 1) + x) * 4 * mMainBuffer->getVertexSize();

    for ( int i = 0; i < 4; ++i )
    {
        texElem->baseVertexPointerToElement(pBase, tex[i].getBase());

        *tex[i].getXPtr() = base_x + coord[i].u / SPLICE_COL;
        *tex[i].getYPtr() = base_y + coord[i].v / SPLICE_ROW;

        pBase += mMainBuffer->getVertexSize();
    }

    mMainBuffer->unlock();
}

void changeGridInfoUV(AutoTexCoord& leftTop, AutoTexCoord& rightTop, AutoTexCoord& leftBottom, AutoTexCoord& rightBottom, uchar state, bool bIndex)
{
    // 0 不变
    // 1 图片水平翻转
    // 2 图片垂直翻转
    // 4 顺时针旋转90度
    // 8 对角线上方顶点纹理坐标复制到对角线下方顶点。（与对角线垂直的两个顶点）
    uchar res1 = state & 1;
    uchar res2 = state & 2;
    uchar res3 = state & 4;
    uchar res4 = state & 8;

    if ( res1 != 0 )
    {
        leftTop.Exchange(rightTop);
        leftBottom.Exchange(rightBottom);
    }

    if ( res2 != 0 )
    {
        leftTop.Exchange(leftBottom);
        rightTop.Exchange(rightBottom);
    }

    if ( res3 != 0 )
    {
        leftTop.Exchange(rightTop);
        leftBottom.Exchange(rightTop);
        rightBottom.Exchange(rightTop);
    }

    if ( res4 != 0 )
    {
        // 非正常索引
        if ( bIndex )
        {
            leftBottom.setX(rightTop.getX());
            leftBottom.setY(rightTop.getY());
        }
        // 正常索引
        else
        {
            rightBottom.setX(leftTop.getX());
            rightBottom.setY(leftTop.getY());
        }
    }
}

void Tile::setTerrainType(bool bTileTerrain)
{
    m_bTileTerrainTile = bTileTerrain;
}

#if OGRE_VERSION_MINOR > 4
/** Shoggoth compatibility function */
void Tile::visitRenderables(Renderable::Visitor* visitor, bool debugRenderables)
{
    visitor->visit(this, 0, false, 0);
}




#endif
}
}
