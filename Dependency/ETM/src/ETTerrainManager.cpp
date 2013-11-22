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

#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "Impl/ETTerrainImpl.h"
#include "Impl/ETTile.h"

#include <OgreStringConverter.h>

using namespace Ogre;
using namespace std;
using Ogre::uint;
using Ogre::ushort;

namespace ET
{
namespace Impl
{
uint checkTileSizeAndGetMaxLOD(size_t tileSize)
{
    // check whether the tilesize suffices 2^n+1
    for (unsigned int i = 0; i <= 10; ++i)
    {
        if (tileSize == (uint)(1 << i) + 1)
        {
            return i + 1;
        }
    }
    OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Given tilesize does not satisfy 2^n+1", "ET::Impl::checkTileSizeAndMaxLOD");
}

uint checkInfo(const TerrainInfo& info, size_t tileSize)
{
    uint ret = checkTileSizeAndGetMaxLOD(tileSize);
    // check width and height
    if (info.getWidth() == 1 || (info.getWidth() % (tileSize - 1) != 1 && tileSize != 2))
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Heightmap width must be a multiple of (tileSize-1) + 1", "ET::Impl::checkInfo");
    }
    if (info.getHeight() == 1 || (info.getHeight() % (tileSize - 1) != 1 && tileSize != 2))
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Heightmap height must be a multiple of (tileSize-1) + 1", "ET::Impl::checkInfo");
    }
    return ret;
}




TerrainImpl::TerrainImpl(SceneManager* sceneMgr, const std::string& name)
    : mSceneMgr(sceneMgr), mTerrainLoaded(false), mIndexHandler(0), mInstanceName(name),
    mViewportHeight(0), maxLODAtBorders(false)
{
    mOpt.maxPixelError = 3;
    mOpt.useLODMorph   = false;
    mOpt.factorC       = 0.0f;
}

TerrainImpl::~TerrainImpl()
{
    destroyTerrain();
}


void TerrainImpl::createTerrain(const TerrainInfo& info, size_t tileSize, uint maxLOD, bool vertexNormals, bool vertexTangents)
{
    // ensure we have a viewport height set
    if (mOpt.factorC <= 0)
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "You need to set a valid pixel error margin.", "TerrainImpl::createTerrain");
    }

    // check given info for validity and determine the maximal possible LOD
    uint maxPossibleLOD = checkInfo(info, tileSize);

    // delete currently loaded terrain (if any)
    destroyTerrain();

    // load new terrain
    mInfo         = info;
    mOpt.tileSize = tileSize;
    if (maxLOD < 1)
    {
        maxLOD = 1;
    }
    mOpt.maxMipMapLevel = min(maxLOD, maxPossibleLOD);
    mOpt.vertexNormals  = vertexNormals;
    mOpt.vertexTangents = vertexTangents;
    mIndexHandler       = new IndexHandler(mOpt.tileSize, mOpt.maxMipMapLevel);
    createTiles();

    mTerrainLoaded = true;
}

void TerrainImpl::createTerrain(const TerrainInfo&     info,
                                const TileTerrainInfo& tInfo,
                                size_t                 tileSize,
                                uint                   maxLOD,
                                bool                   vertexNormals,
                                bool                   vertexTangents)
{
    // ensure we have a viewport height set
    if (mOpt.factorC <= 0)
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "You need to set a valid pixel error margin.", "TerrainImpl::createTerrain");
    }

    // check given info for validity and determine the maximal possible LOD
    uint maxPossibleLOD = checkInfo(info, tileSize);

    // delete currently loaded terrain (if any)
    destroyTerrain();

    // load new terrain
    mInfo            = info;
    mTileTerrainInfo = tInfo;
    mOpt.tileSize    = tileSize;
    if (maxLOD < 1)
    {
        maxLOD = 1;
    }
    mOpt.maxMipMapLevel = min(maxLOD, maxPossibleLOD);
    mOpt.vertexNormals  = vertexNormals;
    mOpt.vertexTangents = vertexTangents;
    mIndexHandler       = new IndexHandler(mOpt.tileSize, mOpt.maxMipMapLevel);
    createTiles();

    mTerrainLoaded = true;
}

void TerrainImpl::createTiles()
{
    // create a root node for the terrain tiles
    mTerrainNode = 0;
    std::string nodeName = mInstanceName + "/Terrain";
    if (mSceneMgr->hasSceneNode(nodeName))
    {
        mTerrainNode = mSceneMgr->getSceneNode(nodeName);
    }
    else
    {
        mTerrainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
    }

    // create the tile grid
    size_t       numTilesCol = (mInfo.getWidth() - 1) / (mOpt.tileSize - 1);
    size_t       numTilesRow = (mInfo.getHeight() - 1) / (mOpt.tileSize - 1);
    mTiles.assign(numTilesCol, TileCol(numTilesRow, (Tile*)0));

    // for each tile, create a scene node to attach to and create the tile
    for (size_t i = 0; i < numTilesCol; ++i)
    {
        for (size_t j = 0; j < numTilesRow; ++j)
        {
            std::string name = nodeName + "/Tile[" + StringConverter::toString(i) + "][" + StringConverter::toString(j) + "]";
            SceneNode*   node = 0;

            if (mSceneMgr->hasSceneNode(name))
            {
                node = mSceneMgr->getSceneNode(name);
                if (node->getParentSceneNode() != mTerrainNode)
                {
                    mTerrainNode->addChild(node);
                }
            }
            else
            {
                node = mTerrainNode->createChildSceneNode(name);
            }
            mTiles[i][j] = new Tile(name,
                                    mSceneMgr,
                                    this,
                                    mIndexHandler,
                                    mInfo,
                                    mOpt,
                                    i * (mOpt.tileSize - 1),
                                    j * (mOpt.tileSize - 1),
                                    true,
                                    &mTileTerrainInfo);
            node->attachObject(mTiles[i][j]);
        }
    }
}

void TerrainImpl::destroyTerrain()
{
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        for (size_t j = 0; j < mTiles[i].size(); ++j)
        {
            delete mTiles[i][j];
        }
    }
    mTiles.clear();
    delete mIndexHandler;
    mIndexHandler = 0;
    mInfo.setHeightmap(0, 0, 0);
    mTerrainLoaded = false;
}


void TerrainImpl::setLODErrorMargin(unsigned int maxPixelError, unsigned int viewportHeight)
{
    mOpt.maxPixelError = maxPixelError;

    // calculate the C factor used in LOD distance calculations
    float T = 2 * maxPixelError / float(viewportHeight);
    mOpt.factorC = 1 / T;
}

void TerrainImpl::setUseLODMorphing(bool lodMorph, float startMorphing, const std::string& morphParamName)
{
    mOpt.useLODMorph   = lodMorph;
    mOpt.lodMorphStart = startMorphing;
    mOpt.lodMorphParam = morphParamName;
}

unsigned int TerrainImpl::getNeighbourState(size_t x, size_t z) const
{
    unsigned int north = 0, east = 0, south = 0, west = 0;

    // get LOD of the tile
    unsigned int lod = mTiles[x][z]->getLOD();

    // check for any neighbours and save their LOD if it's greater than ours
    if (z > 0 && mTiles[x][z - 1]->getLOD() > lod)
    {
        north = mTiles[x][z - 1]->getLOD();
    }
    if (x < mTiles.size() - 1 && mTiles[x + 1][z]->getLOD() > lod)
    {
        east = mTiles[x + 1][z]->getLOD();
    }
    if (z < mTiles[x].size() - 1 && mTiles[x][z + 1]->getLOD() > lod)
    {
        south = mTiles[x][z + 1]->getLOD();
    }
    if (x > 0 && mTiles[x - 1][z]->getLOD() > lod)
    {
        west = mTiles[x - 1][z]->getLOD();
    }

    // shift-unite the neighbours' LODs into an int
    return (north << 24) | (east << 16) | (south << 8) | west;
}


void TerrainImpl::deform(int x, int z, const Brush& brush, float intensity)
{
    // positions given are supposed to be the mid of the brush
    // so adjust accordingly

    // I don't know why... [10/8/2009 KingMars]
    x -= (int)brush.getWidth() / 2 - 1;
    z -= (int)brush.getHeight() / 2 - 1;

    // x -= (int)brush.getWidth()/2;
    // z -= (int)brush.getHeight()/2;

    // iterate over all fields of the brush array and apply them to the
    // heightmap data if they lie within
    for (size_t i = 0; i < brush.getWidth(); ++i)
    {
        int posX = x + (int)i;
        if (posX < 0 || posX >= (int)mInfo.getWidth())
        {
            continue;
        }
        for (size_t j = 0; j < brush.getHeight(); ++j)
        {
            int    posZ = z + (int)j;
            if (posZ < 0 || posZ >= (int)mInfo.getHeight())
            {
                continue;
            }

            float& height = mInfo.at(size_t(posX), size_t(posZ));
            height += intensity * brush.at(i, j);
            //  [10/7/2009 KingMars]
            // height = brush.at( i, j );
            // if (height > 1)
            //        height = 1;
            //      if (height < 0)
            //        height = 0;
        }
    }

    updateTiles(x, z, x + (int)brush.getWidth(), z + (int)brush.getHeight());
}

bool floatEqul(float f, float v)
{
    if ( f - v >= -0.01 && v - f <= 0.01 )
    {
        return true;
    }

    return false;
}

// 预留纹理坐标
static void fixFloat(float& f1, float& f2)
{
    if ( f1 == f2 )
    {
        return;
    }

    if ( f1 < f2 )
    {
        f1 += (float)1 / 256;
        f2 -= (float)1 / 256;
    }
    else
    {
        f1 -= (float)1 / 256;
        f2 += (float)1 / 256;
    }
}

// 获取某行某列的UV坐标
TexCoord getCoord(TexCoord leftTop, TexCoord rightTop, TexCoord leftBottom, int col, int row, int colSize, int rowSize)
{
    TexCoord res;
    if ( leftTop.u != rightTop.u )
    {
        res.u =  leftTop.u < rightTop.u ? (float)col / colSize : (float)(colSize - col) / colSize;
    }
    else if ( leftTop.u != leftBottom.u )
    {
        res.u = leftTop.u < leftBottom.u ? (float)row / rowSize : (float)(rowSize - row) / rowSize;
    }
    else
    {
        assert(0);
    }

    if ( leftTop.v != rightTop.v )
    {
        res.v = leftTop.v < rightTop.v ? (float)col / colSize : (float)(colSize - col) / colSize;
    }
    else if ( leftTop.v != leftBottom.v )
    {
        res.v = leftTop.v < leftBottom.v ? (float)row / rowSize : (float)(rowSize - row) / rowSize;
    }
    else
    {
        assert(0);
    }

    return res;
}

void TerrainImpl::tileSplat(int x, int z, const TileBrush& brush)
{
    int       tileX = x / mTileTerrainInfo.tileSize;
    int       tileZ = z / mTileTerrainInfo.tileSize;

    // 在哪个Grid
    int       gridX = x - tileX * mTileTerrainInfo.tileSize;
    int       gridZ = z - tileZ * mTileTerrainInfo.tileSize;
    // 所使用画刷的纹理信息，（在atlas中的uv坐标）
    ImageInfo info    = mTileTerrainInfo.imageArray[brush.imageID];
    int       colSize = info.col_scale * SPLICE_COL;
    int       rowSize = info.row_scale * SPLICE_COL;
    int       splatColSize, splatRowSize;      // 有几块

    if ( brush.brushType == TileBrush::POINT )
    {
        splatColSize = 1;
        splatRowSize = 1;
    }
    else if ( brush.brushType == TileBrush::H_TOW )
    {
        splatColSize = colSize > 2 ? 2 : colSize;
        splatRowSize = 1;
    }
    else if ( brush.brushType == TileBrush::V_TOW )
    {
        splatRowSize = rowSize > 2 ? 2 : rowSize;
        splatColSize = 1;
    }
    else if ( brush.brushType == TileBrush::FOUR )
    {
        splatRowSize = 2;
        splatColSize = 2;
    }

    float        tex[4][2] = { {0, 0}, {1.0f, 0}, {0, 1.0f}, {1.0f, 1.0f}};
    TexCoord     texCoord[4];
    AutoTexCoord aTexCoord[4];
    for ( int i = 0; i < 4; ++i )
    {
        texCoord[i].u      = tex[i][0];
        texCoord[i].v      = tex[i][1];
        aTexCoord[i].x_ptr = &(tex[i][0]);
    }

    g_info       ginfo  = mTileTerrainInfo.gridInfoArray[z * mTileTerrainInfo.width + x];
    bool         bIndex = ginfo.tri == 1 ? true : false;
    // 翻转等操作
    if (brush.layer == 0)         // 暂时沿用原来的op值，且观之，后边再改成强度值
    {
        brush.op = ginfo.op0;
    }
    else if (brush.layer == 1)
    {
        brush.op = (mTileTerrainInfo.gridInfoArray[z * mTileTerrainInfo.width + x].iBrushCount++);
    }
    changeGridInfoUV(aTexCoord[0], aTexCoord[1], aTexCoord[2], aTexCoord[3], brush.op, bIndex);

    for ( int i = 0; i < 4; ++i )
    {
        texCoord[i].u = aTexCoord[i].getX();
        texCoord[i].v = aTexCoord[i].getY();
    }

    for ( int i = 0; i < rowSize; ++i )
    {
        if ( i >= splatRowSize )
        {
            break;
        }

        for ( int j = 0; j < colSize; ++j )
        {
            if ( j >= splatColSize )
            {
                break;
            }

            TexCoord coord[4];
            coord[0] = getCoord(texCoord[0], texCoord[1], texCoord[2], j, i, colSize, rowSize);
            coord[1] = getCoord(texCoord[0], texCoord[1], texCoord[2], j + 1, i, colSize, rowSize);
            coord[2] = getCoord(texCoord[0], texCoord[1], texCoord[2], j, i + 1, colSize, rowSize);
            coord[3] = getCoord(texCoord[0], texCoord[1], texCoord[2], j + 1, i + 1, colSize, rowSize);
            fixFloat(coord[0].u, coord[3].u);
            fixFloat(coord[0].v, coord[3].v);
            fixFloat(coord[1].u, coord[2].u);
            fixFloat(coord[1].v, coord[2].v);

            // here we might create new pixmap for TileTerrainInfo..
            // if already exist, set the index
            // if not exist, add it int pixmapArray.
            // 在这里就需要对纹理的强度做出调整，brush筛选，是个问题 hemu11/04/08
            mTiles[tileX][tileZ]->setTileTexture(gridX + j, gridZ + i, brush.layer, brush.imageID, coord);
            // mTiles[tileX][tileZ]->updateTileTexture( gridX + j, gridZ + i);
        }
    }
}

void TerrainImpl::setTerrainType(int x, int z, bool bTileTerrain)
{
    int tileX   = x / mTileTerrainInfo.tileSize;
    int tileZ   = z / mTileTerrainInfo.tileSize;
    int tileNum = mInfo.getWidth() / mOpt.tileSize;
    if ( tileX > tileNum     || tileZ > tileNum || tileX < 0 || tileZ < 0 )
    {
        return;
    }

    mTiles[tileX][tileZ]->setTerrainType(bTileTerrain);
}
void TerrainImpl::setHeights(int x, int z, const Brush& brush)
{
    // positions given are supposed to be the mid of the brush
    // so adjust accordingly
    x -= (int)brush.getWidth() / 2;
    z -= (int)brush.getHeight() / 2;

    // iterate over all fields of the brush array and apply them to
    // the heightmap data if they lie within
    for (size_t i = 0; i < brush.getWidth(); ++i)
    {
        int posX = x + (int)i;
        if (posX < 0 || posX >= (int)mInfo.getWidth())
        {
            continue;
        }
        for (size_t j = 0; j < brush.getHeight(); ++j)
        {
            int    posZ = z + (int)j;
            if (posZ < 0 || posZ >= (int)mInfo.getHeight())
            {
                continue;
            }

            float& height = mInfo.at(size_t(posX), size_t(posZ));
            height = brush.at(i, j);
            //  [10/7/2009 KingMars]
            // if (height > 1)
            //  height = 1;
            // if (height < 0)
            //  height = 0;
        }
    }

    updateTiles(x, z, x + (int)brush.getWidth(), z + (int)brush.getHeight());
}

void TerrainImpl::getHeights(int x, int z, Brush& brush) const
{
    // positions given are supposed to be the mid of the brush
    // so adjust accordingly
    x -= (int)brush.getWidth() / 2;
    z -= (int)brush.getHeight() / 2;

    // iterate over all fields of the brush array and fill them
    // if they lie within
    for (size_t i = 0; i < brush.getWidth(); ++i)
    {
        int posX = x + (int)i;
        for (size_t j = 0; j < brush.getHeight(); ++j)
        {
            int posZ = z + (int)j;
            if (posX < 0 || posX >= (int)mInfo.getWidth() || posZ < 0 || posZ >= (int)mInfo.getHeight())
            {
                brush.at(i, j) = -1;
                continue;
            }

            brush.at(i, j) = mInfo.at(size_t(posX), size_t(posZ));
        }
    }
}


void TerrainImpl::updateTiles(int fromX, int fromZ, int toX, int toZ)
{
    // iterate over all terrain tiles and update them if they lie within
    // the update areal.
    for (size_t i = 0; i < mTiles.size(); ++i)
    {
        int startX = (int) ((mOpt.tileSize - 1) * i);
        int endX   = (int) ((mOpt.tileSize - 1) * (i + 1));
        for (size_t j = 0; j < mTiles[i].size(); ++j)
        {
            int    startZ = (int) ((mOpt.tileSize - 1) * j);
            int    endZ   = (int) ((mOpt.tileSize - 1) * (j + 1));
            size_t x1     = (size_t) max(fromX, startX);
            size_t x2     = (size_t) min(toX, endX);
            size_t z1     = (size_t) max(fromZ, startZ);
            size_t z2     = (size_t) min(toZ, endZ);
            if (x2 >= x1 && z2 >= z1)
            {
                // update this tile
                mTiles[i][j]->updateTerrain(x1, z1, x2, z2);
            }
        }
    }

    // ...
    // update world bounds of the scene nodes holding the tiles (is this necessary?)
    // for (unsigned short i = 0; i < mTerrainNode->numChildren(); ++i)
    // {
    //  SceneNode* node = static_cast<SceneNode*>(mTerrainNode->getChild(i));
    //  node->_updateBounds();
    // }
}
}


TerrainManager::TerrainManager(SceneManager* sceneMgr, const std::string& name)
    : mImpl(new Impl::TerrainImpl(sceneMgr, name))
{
}

TerrainManager::~TerrainManager()
{
    delete mImpl;
}

void TerrainManager::createTerrain(const TerrainInfo&     info,
                                   const TileTerrainInfo& tileTerrainInfo,
                                   size_t                 tileSize,
                                   uint                   maxLOD,
                                   bool                   vertexNormals,
                                   bool                   vertexTangents)
{
    mImpl->createTerrain(info, tileTerrainInfo, tileSize, maxLOD, vertexNormals, vertexTangents);
}

void TerrainManager::destroyTerrain()
{
    mImpl->destroyTerrain();
}

const TerrainInfo& TerrainManager::getTerrainInfo() const
{
    if (!mImpl->isTerrainLoaded())
    {
        OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "No terrain loaded.", "TerrainManager::getTerrainInfo");
    }
    return mImpl->getTerrainInfo();
}

void TerrainManager::setLODErrorMargin(uint maxPixelError, uint viewportHeight)
{
    mImpl->setLODErrorMargin(maxPixelError, viewportHeight);
}


void TerrainManager::setUseLODMorphing(bool lodMorph, float startMorphing, const std::string& morphParamName)
{
    mImpl->setUseLODMorphing(lodMorph, startMorphing, morphParamName);
}


void TerrainManager::setMaterial(MaterialPtr material)
{
    mImpl->setMaterial(material);
}

const MaterialPtr& TerrainManager::getMaterial() const
{
    return mImpl->getMaterial();
}

void TerrainManager::setTileTerrainMaterial(Ogre::MaterialPtr material)
{
    mImpl->setTileTerrainMaterial(material);
}

const MaterialPtr& TerrainManager::getTileTerrainMaetrial() const
{
    return mImpl->getTileTerrainMaterial();
}

void TerrainManager::deform(int x, int z, const Brush& brush, float intensity)
{
    if (!mImpl->isTerrainLoaded())
    {
        OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "No terrain loaded.", "TerrainManager::deform");
    }
    mImpl->deform(x, z, brush, intensity);
}

void TerrainManager::setHeights(int x, int z, const Brush& brush)
{
    if (!mImpl->isTerrainLoaded())
    {
        OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "No terrain loaded.", "TerrainManager::setHeights");
    }
    mImpl->setHeights(x, z, brush);
}

void TerrainManager::getHeights(int x, int z, Brush& brush) const
{
    if (!mImpl->isTerrainLoaded())
    {
        OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "No terrain loaded.", "TerrainManager::getHeights");
    }
    mImpl->getHeights(x, z, brush);
}

//  [10/7/2009 KingMars]
const TileTerrainInfo& TerrainManager::getTileTerrainInfo() const
{
    return mImpl->getTileTerrainInfo();
}

void TerrainManager::tileSplat(int x, int y, const TileBrush& brush) const
{
    mImpl->tileSplat(x, y, brush);
}
int TerrainManager::getImgNum()
{
    return mImpl->getImgNum();
}
void TerrainManager::setTerrainType(int x, int y, bool bTileTerrain)
{
    mImpl->setTerrainType(x, y, bTileTerrain);
}
}
