// ---------------------------------------------------------------
// modified or developed by Shen Yuqing
// HUST CS 06
// syq.myth@gmail.com
// 2009
// ---------------------------------------------------------------
#include <OgreSceneManager.h>
#include <OgreHardwareBufferManager.h>
#include <OgreMath.h>

#include "ETTerrainInfo.h"
#include "TerrainLiquid.h"

#ifndef max
#define max(a, b)            (((a) > (b)) ? (a) : (b))
#endif

using namespace Ogre;

const unsigned short MAIN_BINDING = 0;

TerrainLiquid::TerrainLiquid()
    : m_bDepthEnable(false)
    , m_fDepthScale(0.0f)
{
    m_pTempGridArray = new std::list<POINT>;
}

TerrainLiquid::~TerrainLiquid()
{
    delete m_pTempGridArray;
}

Ogre::uint32 TerrainLiquid::getTypeFlags() const
{
    // 将TerrainLiquid看做地形的一部分
    return Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK;
}

Ogre::Real TerrainLiquid::getBoundingRadius(void) const
{
    return Math::Sqrt(max(mBox.getMaximum().squaredLength(), mBox.getMinimum().squaredLength()));
}

Ogre::Real TerrainLiquid::getSquaredViewDepth(const Ogre::Camera* cam) const
{
    const Vector3 vMin  = mBox.getMinimum();
    const Vector3 vMax  = mBox.getMaximum();
    const Vector3 vMid  = ((vMin - vMax) * 0.5) + vMin;
    const Vector3 vDist = cam->getDerivedPosition() - vMid;

    return vDist.squaredLength();
}

void TerrainLiquid::createTerrainLiquid(const Ogre::Vector3&   seedPos,
                                        float                  fTextureScale,
                                        bool                   bDepthEnable,
                                        float                  fDepthScale,
                                        const ET::TerrainInfo& terrainInfo)
{
    // init parameter
    mSeedPos        = seedPos;
    mStep           = terrainInfo.getScaling();
    mTerrainInfo    = &terrainInfo;
    m_fTextureScale = fTextureScale;
    m_bDepthEnable  = bDepthEnable;
    m_fDepthScale   = fDepthScale;

    // init AABB
    mBox.setMaximum(seedPos);
    mBox.setMinimum(seedPos);

    // seed fill
    __spreed(0, 0, FOUR_DIR);

    // create vertex & index
    __createVertexData();
    __createIndexData();
}

static int GetMin(int a, int b)
{
    return a < b ? a : b;
}

static int GetMax(int a, int b)
{
    return a > b ? a : b;
}

void TerrainLiquid::__addGrid(int x, int z)
{
    POINT point(x, z);
    m_pTempGridArray->push_back(point);
    mBox.merge(__getPos(x, z));
    mBox.merge(__getPos(x + 1, z + 1));
}

void TerrainLiquid::__createVertexData()
{
    // sort( mTempGridArray.begin(), mTempGridArray.end() );
    m_pTempGridArray->sort();
    Ogre::HardwareVertexBufferSharedPtr pVerttexBuffer;
    m_pVertex              = new Ogre::VertexData;
    m_pVertex->vertexStart = 0;
    m_pVertex->vertexCount = m_pTempGridArray->size() * 4;

    Ogre::VertexDeclaration*            decl   = m_pVertex->vertexDeclaration;
    Ogre::VertexBufferBinding*          bind   = m_pVertex->vertexBufferBinding;
    size_t                              offset = 0;

    decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(VET_FLOAT3);

    decl->addElement(MAIN_BINDING, offset, VET_FLOAT3, VES_NORMAL);
    offset += Ogre::VertexElement::getTypeSize(VET_FLOAT3);

    decl->addElement(MAIN_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    offset += Ogre::VertexElement::getTypeSize(VET_FLOAT2);

    if ( m_bDepthEnable )
    {
        decl->addElement(MAIN_BINDING, offset, VET_FLOAT1, VES_TEXTURE_COORDINATES, 1);
    }

    pVerttexBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(
        decl->getVertexSize(MAIN_BINDING), m_pVertex->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    bind->setBinding(MAIN_BINDING, pVerttexBuffer);

    const Ogre::VertexElement* posElem    = decl->findElementBySemantic(VES_POSITION);
    const Ogre::VertexElement* normalElem = decl->findElementBySemantic(VES_NORMAL);
    const Ogre::VertexElement* texElem0   = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);                    // dynamic texture
    const Ogre::VertexElement* texElem1   = NULL;
    if ( m_bDepthEnable )
    {
        texElem1 = decl->findElementBySemantic(VES_TEXTURE_COORDINATES, 1);                                     // depth texture
    }
    unsigned char*             pBase = static_cast<unsigned char*>(pVerttexBuffer->lock(HardwareBuffer::HBL_DISCARD));
    void*                      pPos, * pNormal, * pTex0, * pTex1;
    // tex0 为动态纹理
    // tex1 为一维深度图

    std::list<POINT>::iterator it;
    for ( it = m_pTempGridArray->begin(); it != m_pTempGridArray->end(); it++ )
    {
        POINT         point = (*it);

        // 4 vertex per grid
        Ogre::Vector3 pos;
        Ogre::Vector3 normal(0.0f, 1.0f, 0.0f);
        Ogre::Vector2 tex0;
        Ogre::Real    tex1;
        int           x, z;

        // left top
        x = point.x;
        z = point.z;
        posElem->baseVertexPointerToElement(pBase, &pPos);
        normalElem->baseVertexPointerToElement(pBase, &pNormal);
        texElem0->baseVertexPointerToElement(pBase, &pTex0);
        pos  = __getPos(x, z);
        tex0 = __getTexCoord(x, z, LEFT_TOP);
        memcpy(pPos, &pos, sizeof(Ogre::Vector3));
        memcpy(pNormal, &normal, sizeof(Ogre::Vector3));
        memcpy(pTex0, &tex0, sizeof(Ogre::Vector2));
        if ( m_bDepthEnable )
        {
            texElem1->baseVertexPointerToElement(pBase, &pTex1);
            tex1 = __getDepthTexCoord(x, z);
            memcpy(pTex1, &tex1, sizeof(Ogre::Real));
        }
        pBase += pVerttexBuffer->getVertexSize();

        // right top
        x = point.x + 1;
        z = point.z;
        posElem->baseVertexPointerToElement(pBase, &pPos);
        normalElem->baseVertexPointerToElement(pBase, &pNormal);
        texElem0->baseVertexPointerToElement(pBase, &pTex0);
        pos  = __getPos(x, z);
        tex0 = __getTexCoord(x, z, RIGHT_TOP);
        memcpy(pPos, &pos, sizeof(Ogre::Vector3));
        memcpy(pNormal, &normal, sizeof(Ogre::Vector3));
        memcpy(pTex0, &tex0, sizeof(Ogre::Vector2));
        if ( m_bDepthEnable )
        {
            texElem1->baseVertexPointerToElement(pBase, &pTex1);
            tex1 = __getDepthTexCoord(x, z);
            memcpy(pTex1, &tex1, sizeof(Ogre::Real));
        }
        pBase += pVerttexBuffer->getVertexSize();

        // left bottom
        x = point.x;
        z = point.z + 1;
        posElem->baseVertexPointerToElement(pBase, &pPos);
        normalElem->baseVertexPointerToElement(pBase, &pNormal);
        texElem0->baseVertexPointerToElement(pBase, &pTex0);
        pos  = __getPos(x, z);
        tex0 = __getTexCoord(x, z, LEFT_BOTTOM);
        memcpy(pPos, &pos, sizeof(Ogre::Vector3));
        memcpy(pNormal, &normal, sizeof(Ogre::Vector3));
        memcpy(pTex0, &tex0, sizeof(Ogre::Vector2));
        if ( m_bDepthEnable )
        {
            texElem1->baseVertexPointerToElement(pBase, &pTex1);
            tex1 = __getDepthTexCoord(x, z);
            memcpy(pTex1, &tex1, sizeof(Ogre::Real));
        }
        pBase += pVerttexBuffer->getVertexSize();

        // right bottom
        x = point.x + 1;
        z = point.z + 1;
        posElem->baseVertexPointerToElement(pBase, &pPos);
        normalElem->baseVertexPointerToElement(pBase, &pNormal);
        texElem0->baseVertexPointerToElement(pBase, &pTex0);
        pos  = __getPos(x, z);
        tex0 = __getTexCoord(x, z, RIGHT_BOTTOM);
        memcpy(pPos, &pos, sizeof(Ogre::Vector3));
        memcpy(pNormal, &normal, sizeof(Ogre::Vector3));
        memcpy(pTex0, &tex0, sizeof(Ogre::Vector2));
        if ( m_bDepthEnable )
        {
            texElem1->baseVertexPointerToElement(pBase, &pTex1);
            tex1 = __getDepthTexCoord(x, z);
            memcpy(pTex1, &tex1, sizeof(Ogre::Real));
        }
        pBase += pVerttexBuffer->getVertexSize();
    }

    pVerttexBuffer->unlock();
    mRenderOp.vertexData = m_pVertex;
}

bool TerrainLiquid::__isValidGrid(int x, int z, int dir)
{
    int           y = mSeedPos.y;

    int           left   = mTerrainInfo->getOffset().x;
    int           right  = left + (mTerrainInfo->getWidth() - 1) * mTerrainInfo->getScaling().x;
    int           top    = mTerrainInfo->getOffset().z;
    int           bottom = top + (mTerrainInfo->getHeight() - 1) * mTerrainInfo->getScaling().y;

    Ogre::Vector3 leftTop     = __getPos(x, z);
    Ogre::Vector3 rightTop    = __getPos(x + 1, z);
    Ogre::Vector3 leftBottom  = __getPos(x, z + 1);
    Ogre::Vector3 rightBottom = __getPos(x + 1, z + 1);

    int           lt = mTerrainInfo->getHeightAt(leftTop.x, leftTop.z);
    int           rt = mTerrainInfo->getHeightAt(rightTop.x, rightTop.z);
    int           lb = mTerrainInfo->getHeightAt(leftBottom.x, leftBottom.z);
    int           rb = mTerrainInfo->getHeightAt(rightBottom.x, rightBottom.z);

    // bounding check
    if ( leftTop.x < left || rightTop.x > right || leftTop.z < top || leftBottom.z > bottom )
    {
        return false;
    }

    if ( lt > leftTop.y && rt > rightTop.y && lb > leftBottom.y && rb > rightBottom.y )
    {
        return false;
    }

    else if ( dir == LEFT )
    {
        if ((lt < y || lb < y) && (rt >= y && rb >= y))
        {
            return false;
        }
    }

    else if ( dir == RIGHT )
    {
        if ((rt < y || rb < y) && (lt >= y && lb >= y))
        {
            return false;
        }
    }

    else if ( dir == UP )
    {
        if ((rt < y || lt < y) && (rb >= y && lb >= y))
        {
            return false;
        }
    }

    else if ( dir == DOWN )
    {
        if ((rb < y || lb < y) && (rt >= y && lt >= y))
        {
            return false;
        }
    }

    return true;
}

void TerrainLiquid::__createIndexData()
{
    m_pIndex             = new IndexData;
    m_pIndex->indexStart = 0;
    m_pIndex->indexCount = m_pTempGridArray->size() * 6;

    m_pIndex->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT,
        m_pIndex->indexCount,
        HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    ushort* pIdx = static_cast<ushort*>(m_pIndex->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
    for ( int i = 0; i < m_pTempGridArray->size(); ++i )
    {
        ushort iIndexTopLeft     = i * 4;
        ushort iIndexTopRight    = iIndexTopLeft + 1;
        ushort iIndexBottomLeft  = iIndexTopLeft + 2;
        ushort iIndexBottomRight = iIndexTopLeft + 3;

        *pIdx++ = iIndexTopLeft;
        *pIdx++ = iIndexBottomLeft;
        *pIdx++ = iIndexTopRight;
        *pIdx++ = iIndexBottomLeft;
        *pIdx++ = iIndexBottomRight;
        *pIdx++ = iIndexTopRight;
    }

    m_pIndex->indexBuffer->unlock();
    mRenderOp.indexData     = m_pIndex;
    mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST;
    mRenderOp.useIndexes    = true;
}

const Quaternion& TerrainLiquid::getWorldOrientation() const
{
    return Quaternion::IDENTITY;
}

const Vector3&    TerrainLiquid::getWorldPosition() const
{
    return Vector3::ZERO;
}

Ogre::Vector2 TerrainLiquid::__getTexCoord(int x, int z, int type)
{
    float tx = x * m_fTextureScale;
    float ty = z * m_fTextureScale;
    return Ogre::Vector2(tx, ty);
}

Ogre::Real TerrainLiquid::__getDepthTexCoord(int x, int z)
{
    float terrainHieght = mTerrainInfo->getHeightAt(mSeedPos.x + x * mStep.x, mSeedPos.z + z * mStep.z);
    float depth         = mSeedPos.y - terrainHieght;
    if ( depth < 0.0f )
    {
        depth = 0.0f;
    }

    if ( !m_bDepthEnable )
    {
        return 1.0f;
    }

    return depth * m_fDepthScale;
}

void TerrainLiquid::__spreed(int x, int z, int direction)
{
    if ( !__isGridContained(x, z) && __isValidGrid(x, z, direction))
    {
        __addGrid(x, z);
    }
    else
    {
        return;
    }

    __spreed(x, z - 1, UP);
    __spreed(x, z + 1, DOWN);
    __spreed(x - 1, z, LEFT);
    __spreed(x + 1, z, RIGHT);
}

bool TerrainLiquid::__isGridContained(int x, int z)
{
    POINT p(x, z);
    if ( find(m_pTempGridArray->begin(), m_pTempGridArray->end(), p) == m_pTempGridArray->end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void TerrainLiquid::__spreedOneStep(int x, int z)
{
    if ( !__isGridContained(x, z))
    {
        __addGrid(x, z);
    }
}
