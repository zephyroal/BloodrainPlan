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

#include "ETTerrainInfo.h"
#include <OgreTexture.h>
#include <OgreTextureManager.h>
#include <OgreVector3.h>
#include <OgreAxisAlignedBox.h>
#include <OgreException.h>
#include <OgreRay.h>
#include <OgreHardwarePixelBuffer.h>
#include <windows.h>
#include <OgreD3D9Texture.h>
#include <OgreD3D9Mappings.h>


#include "TinyXml/tinyxml.h"
#include "tinyxml/tinystr.h"



using namespace Ogre;
using namespace std;

namespace ET
{
namespace Impl
{
struct InfoImpl
{
    /** extents of the heightmap */
    size_t             width, height;
    /** the heightmap */
    std::vector<float> heightmap;

    /** offset at which the (0, 0) point of the terrain is placed in Ogre */
    Vector3            offset;
    /** scale of a terrain vertex */
    Vector3            scale;
};
}

TerrainInfo::TerrainInfo()
    : mWidth(0), mHeight(0), mOffset(Vector3::ZERO), mScale(Vector3::UNIT_SCALE)
{
}

TerrainInfo::TerrainInfo(size_t width, size_t height, const float* heightmap)
    : mWidth(0), mHeight(0), mOffset(Vector3::ZERO), mScale(Vector3::UNIT_SCALE)
{
    setHeightmap(width, height, heightmap);
}

TerrainInfo::TerrainInfo(size_t width, size_t height, const std::vector<float>& heightmap)
    : mWidth(0), mHeight(0), mOffset(Vector3::ZERO), mScale(Vector3::UNIT_SCALE)
{
    setHeightmap(width, height, heightmap);
}


void TerrainInfo::setHeightmap(size_t width, size_t height, const float* heightmap)
{
    mHeightmap.assign(heightmap, heightmap + width * height);
    mWidth  = width;
    mHeight = height;
}

void TerrainInfo::setHeightmap(size_t width, size_t height, const std::vector<float>& heightmap)
{
    if (width * height != heightmap.size())
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Given heightmap dimensions and size of the vector do not match.", "TerrainInfo::setHeightmap");
    }

    mHeightmap = heightmap;
    mWidth     = width;
    mHeight    = height;
}


void TerrainInfo::setExtents(const AxisAlignedBox& extents)
{
    if (mWidth == 0)
    {
        OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "You must set a heightmap first.", "TerrainInfo::setExtents");
    }

    mOffset = extents.getMinimum();
    mScale  = extents.getMaximum() - extents.getMinimum();

    //  [10/7/2009 KingMars]
    mScale.x /= (mWidth - 1);
    mScale.z /= (mHeight - 1);
}

AxisAlignedBox TerrainInfo::getExtents() const
{
    if (mWidth == 0)
    {
        OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "You must set a heightmap first.", "TerrainInfo::setExtents");
    }

    Vector3 maximum = mScale;
    maximum.x *= mWidth;
    maximum.z *= mHeight;
    maximum   += mOffset;
    return AxisAlignedBox(mOffset, maximum);
}


float TerrainInfo::getHeightAt(float x, float z) const
{
    // scale down
    x -= mOffset.x;
    z -= mOffset.z;
    x /= mScale.x;
    z /= mScale.z;
    if (x < 0.0 || x > mWidth - 1 || z < 0.0 || z > mHeight - 1)
    {
        // out of bounds
        return mOffset.y - 1;
    }

    // retrieve height from heightmap via bilinear interpolation
    size_t xi   = (size_t)x, zi = (size_t)z;
    float  xpct = x - xi, zpct = z - zi;
    if (xi == mWidth - 1)
    {
        // one too far
        --xi;
        xpct = 1.0f;
    }
    if (zi == mHeight - 1)
    {
        --zi;
        zpct = 1.0f;
    }

    // retrieve heights
    float  heights[4];
    heights[0] = at(xi, zi);
    heights[1] = at(xi, zi + 1);
    heights[2] = at(xi + 1, zi);
    heights[3] = at(xi + 1, zi + 1);

    // interpolate
    float  w[4];
    w[0] = (1.0 - xpct) * (1.0 - zpct);
    w[1] = (1.0 - xpct) * zpct;
    w[2] = xpct * (1.0 - zpct);
    w[3] = xpct * zpct;
    float  ipHeight = w[0] * heights[0] + w[1] * heights[1] + w[2] * heights[2] + w[3] * heights[3];

    // scale to actual height
    ipHeight *= mScale.y;
    ipHeight += mOffset.y;

    return ipHeight;
}


Vector3 TerrainInfo::getNormalAt(float x, float z) const
{
    int     flip = 1;
    Vector3 here(x, getHeightAt(x, z), z);
    Vector3 left(x - 1, getHeightAt(x - 1, z), z);
    Vector3 down(x, getHeightAt(x, z + 1), z + 1);
    if (left.x < 0.0)
    {
        flip *= -1;
        left  = Vector3(x + 1, getHeightAt(x + 1, z), z);
    }
    if (down.z >= mOffset.z + mScale.z * (mHeight - 1))
    {
        flip *= -1;
        down  = Vector3(x, getHeightAt(x, z - 1), z - 1);
    }
    left -= here;
    down -= here;

    Vector3 norm = flip * left.crossProduct(down);
    norm.normalise();

    return norm;
}


/** Addition from SongOfTheWeave */
Vector3 TerrainInfo::getTangentAt(float x, float z) const
{
    Ogre::Vector3 v3Return;
    int           flip = 1;
    Vector3       here(x, getHeightAt(x, z), z);
    Vector3       left(x - 1, getHeightAt(x - 1, z), z);
    if (left.x < 0.0)
    {
        flip *= -1;
        left  = Vector3(x + 1, getHeightAt(x + 1, z), z);
    }
    left    -= here;
    v3Return = flip * left;
    v3Return.normalise();
    return v3Return;
}


std::pair<bool, Vector3> TerrainInfo::rayIntersects(const Ray& ray) const
{
    AxisAlignedBox box   = getExtents();
    Vector3        point = ray.getOrigin();
    Vector3        dir   = ray.getDirection();

    // first, does the ray start from inside the terrain extents?
    if (!boundingBox.contains(point))
    {
        // not inside the box, so let's see if we are actually
        // colliding with it
        pair<bool, Real> res = ray.intersects(boundingBox);
        if (!res.first)
        {
            return make_pair(false, Vector3::ZERO);
        }
        // update point to the collision position
        point = ray.getPoint(res.second);
    }

    // now move along the ray until we intersect or leave the bounding box
    while (true)
    {
        // have we arived at or under the terrain height?
        // note that this approach means that ray queries from below won't work
        // correctly, but then again, that shouldn't be a usual case...
        float height = getHeightAt(point.x, point.z);
        if (point.y <= height)
        {
            point.y = height;
            return make_pair(true, point);
        }

        // move further...
        point += dir;

        // check if we are still inside the boundaries
        if (point.x < box.getMinimum().x || point.z < box.getMinimum().z
            || point.x > box.getMaximum().x || point.z > box.getMaximum().z)
        {
            return make_pair(false, Vector3::ZERO);
        }
    }
}

// --------------------------------------------------------------------
static bool IsStrEqual(const char* str1, const char* str2)
{
    return (strcmp(str1, str2) == 0) ? true : false;
}


char* UnicodeToANSI(const wchar_t* str)
{
    char* result;
    int   textlen;
    textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    result  = (char*)malloc((textlen + 1) * sizeof(char));
    memset(result, 0, sizeof(char) * (textlen + 1));
    WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
    return result;
}


wchar_t* UTF8ToUnicode(const char* str)
{
    int      textlen;
    wchar_t* result;
    textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    result  = (wchar_t*)malloc((textlen + 1) * sizeof(wchar_t));
    memset(result, 0, (textlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
    return result;
}


char* UTF8ToANSI(const char* str)
{
    wchar_t* temp = UTF8ToUnicode(str);
    char*    res  = UnicodeToANSI(temp);
    delete[]temp;
    return res;
}

static void fixFloat(float& f)
{
    if (f < 0.01f)
    {
        f = 0.005f;
    }
    else if (f > 0.24f && f < 0.25f)
    {
        f = 0.245f;
    }
    else if (f > 0.25f && f < 0.26f)
    {
        f = 0.255f;
    }
    else if (f > 0.49f && f < 0.50f)
    {
        f = 0.495f;
    }
    else if (f > 0.5f && f < 0.51f)
    {
        f = 0.505f;
    }
    else if (f > 0.74f && f < 0.75f)
    {
        f = 0.745f;
    }
    else if (f > 0.75f && f < 0.76f)
    {
        f = 0.755f;
    }
    else if (f > 0.99f)
    {
        f = 0.995f;
    }
}
bool _saveTextureToLocal(Ogre::TexturePtr texPtr, const std::string& fileName)
{
    Ogre::HardwarePixelBufferSharedPtr tmpTexBuf = texPtr->getBuffer();
    int                                width     = texPtr->getWidth();
    int                                height    = texPtr->getHeight();

    char*                              tmpBuf = new char[width * height * 4];
    Ogre::PixelBox                     tmpBox(width,
                                              height,
                                              texPtr->getDepth(),
                                              texPtr->getFormat(),
                                              tmpBuf);
    tmpTexBuf->blitToMemory(tmpBox);

    TexturePtr                         tmpTexPtr = TextureManager::getSingleton().createManual(fileName, "General",
                                                                                               TEX_TYPE_2D, texPtr->getWidth(),
                                                                                               texPtr->getHeight(), 1, 0, PF_A8B8G8R8);
    tmpTexPtr->load();

    HRESULT                            hr;
    IDirect3DSurface9*                 pDstSurface = 0;
    Ogre::D3D9Texture*                 d3dTex      = reinterpret_cast<D3D9Texture*>(tmpTexPtr.get());
    RECT                               dstRC       = { 0, 0, d3dTex->getWidth(), d3dTex->getHeight() };

    if (FAILED(hr = d3dTex->getNormTexture()->GetSurfaceLevel(0, &pDstSurface)))
    {
        return false;
    }

    size_t                             rowWidth;
    if (PixelUtil::isCompressed(tmpBox.format))
    {
        // D3D wants the width of one row of cells in bytes
        if (tmpBox.format == PF_DXT1)
        {
            // 64 bits (8 bytes) per 4x4 block
            rowWidth = (tmpBox.rowPitch / 4) * 8;
        }
        else
        {
            // 128 bits (16 bytes) per 4x4 block
            rowWidth = (tmpBox.rowPitch / 4) * 16;
        }
    }
    else
    {
        rowWidth = tmpBox.rowPitch * PixelUtil::getNumElemBytes(tmpBox.format);
    }

    if (FAILED(hr = D3DXLoadSurfaceFromMemory(
                   pDstSurface, NULL, &dstRC, tmpBox.data,
                   Ogre::D3D9Mappings::_getPF(tmpBox.format),
                   rowWidth, 0,
                   &dstRC, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0)))
    {
        SAFE_RELEASE(pDstSurface);
        return false;
    }

    Ogre::HardwarePixelBufferSharedPtr tmpDesTexBuf = tmpTexPtr->getBuffer();
    char*                              tmpDesBuf    = new char[width * height * 4];
    Ogre::PixelBox                     tmpDesBox(width,
                                                 height,
                                                 tmpTexPtr->getDepth(),
                                                 tmpTexPtr->getFormat(),
                                                 tmpDesBuf);
    tmpDesTexBuf->blitToMemory(tmpDesBox);
    Ogre::Image                        image;
    image.loadDynamicImage((Ogre::uchar*)tmpDesBox.data, tmpDesBox.getWidth(), tmpDesBox.getHeight(), tmpDesBox.format);
    image.save(fileName);

    delete[] tmpDesBuf;
    delete[] tmpBuf;

    tmpTexPtr.setNull();

    return true;
}
// --------------------------------------------------------------------
void TileTerrainInfo::LoadTerrain(const std::string& sceneDir, const std::string& terrainFileName)
{
    TiXmlDocument    terrainDoc;
    bool             bLoadSuccess = terrainDoc.LoadFile(sceneDir + terrainFileName);

    if (!bLoadSuccess)
    {
        OGRE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "load terrain file failed", "");
        return;
    }

    TiXmlElement*    root = terrainDoc.FirstChildElement();

    root->QueryIntAttribute("xsize", &width);
    root->QueryIntAttribute("zsize", &height);
    root->QueryIntAttribute("tileSize", &tileSize);

    const char*      elementValue;

    // scale
    TiXmlElement*    element = root->FirstChildElement("scale");

    element->QueryIntAttribute("x", &scale_x);
    element->QueryIntAttribute("y", &scale_y);
    element->QueryIntAttribute("z", &scale_z);

    // heightmap
    element = root->FirstChildElement("heightmap");
    const char*      heightmapFileName = element->Attribute("filename");
    const char*      heightmapType     = element->Attribute("type");
    LoadHightMap((sceneDir + heightmapFileName).c_str(), heightmapType);


    // gridinfo
    element = root->FirstChildElement("gridInfo");
    const char*      gridInfoFileName = element->Attribute("filename");
    const char*      gridInfoType     = element->Attribute("type");
    LoadGridInfo((sceneDir + gridInfoFileName).c_str(), gridInfoType);

    // lightmap
    element = root->FirstChildElement("lightmap");
    const char*      lightMapFileName = element->Attribute("filename");
    const char*      lightMapType     = element->Attribute("type");
    LoadLightMap((sceneDir + lightMapFileName).c_str(), lightMapType);

    // textures
    element = root->FirstChildElement("textures");

    TiXmlElement*    pTxtureElement = element->FirstChildElement();
    int              imageIndex     = 0;

    Ogre::TexturePtr layer0       = Ogre::TextureManager::getSingletonPtr()->getByName("<layer0>");
    Ogre::TexturePtr tileimageset = Ogre::TextureManager::getSingletonPtr()->getByName("<TileImagesetTexture>");
    assert(!layer0.isNull());

    while (pTxtureElement)
    {
        elementValue = pTxtureElement->Attribute("type");
        {
            if (IsStrEqual("image", elementValue))
            {
                size_t      imageWidth, imageHeight;

                elementValue = pTxtureElement->Attribute("filename");
				elementValue = UTF8ToANSI(elementValue);
                Ogre::Image image;
                ImageInfo   iInfo;

                image.load(elementValue, "General");

                imageWidth      = image.getWidth();
                imageHeight     = image.getHeight();
                iInfo.fileName  = elementValue;
                iInfo.col_scale = (float)imageWidth / SPLICE_IMAGE_SIZE;
                iInfo.row_scale = (float)imageHeight / SPLICE_IMAGE_SIZE;
                assert(iInfo.col_scale <= 1.01f);
                assert(iInfo.row_scale <= 1.01f);

                // 将贴图risize为256*256，把大贴图填满，这样似乎能缓解一些地形裂缝的问题。
                image.resize(256, 256);
                imageWidth  = image.getWidth();
                imageHeight = image.getHeight();

                // 手动生成mipmap
                imageArray.push_back(iInfo);
                AddImageIntoTexture(layer0, imageIndex, image, SPLICE_IMAGE_SIZE);
                image.resize(imageWidth / 2, imageHeight / 2, Ogre::Image::FILTER_BILINEAR);
                AddImageIntoTexture(layer0->getBuffer(0, 1), imageIndex, image, SPLICE_IMAGE_SIZE / 2);
                image.resize(imageWidth / 4, imageHeight / 4, Ogre::Image::FILTER_BILINEAR);
                AddImageIntoTexture(layer0->getBuffer(0, 2), imageIndex, image, SPLICE_IMAGE_SIZE / 4);
                image.resize(imageWidth / 8, imageHeight / 8, Ogre::Image::FILTER_BILINEAR);
                AddImageIntoTexture(layer0->getBuffer(0, 3), imageIndex, image, SPLICE_IMAGE_SIZE / 8);

                // 缩小后64x64的纹理，用于编辑器。
                if (!tileimageset.isNull())
                {
                    image.load(elementValue, "General");
                    int fixedHeight = 64;
                    int fixedWidth  = 64 * imageWidth / imageHeight;
                    image.resize(fixedWidth, fixedHeight);
                    AddImageIntoTexture(tileimageset, imageIndex, image, 64);
                }

                imageIndex++;
                delete[] elementValue;
            }
            else
            {
                OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "unknown type of textures", "");
            }
            pTxtureElement = pTxtureElement->NextSiblingElement();
        }
    }
    // add a transparent image.
    // 用于layer1未定义的格子。
    Ogre::Image   image;
    image.load("transparent.tga", "General");
    int           imagewidth = image.getWidth(), imageheight = image.getHeight();
    ImageInfo     iInfo;
    iInfo.col_scale = (float)image.getWidth() / SPLICE_IMAGE_SIZE;
    iInfo.row_scale = (float)image.getHeight() / SPLICE_IMAGE_SIZE;
    imageArray.push_back(iInfo);
    AddImageIntoTexture(layer0, imageIndex, image, SPLICE_IMAGE_SIZE);
    image.resize(imagewidth / 2, imageheight / 2);
    AddImageIntoTexture(layer0->getBuffer(0, 1), imageIndex, image, SPLICE_IMAGE_SIZE / 2);
    image.resize(imagewidth / 4, imageheight / 4);
    AddImageIntoTexture(layer0->getBuffer(0, 2), imageIndex, image, SPLICE_IMAGE_SIZE / 4);
    image.resize(imagewidth / 8, imageheight / 8);
    AddImageIntoTexture(layer0->getBuffer(0, 3), imageIndex, image, SPLICE_IMAGE_SIZE / 8);

    element = root->FirstChildElement("pixmaps");

    TiXmlElement* pPixmapElement = element->FirstChildElement();
    while (pPixmapElement)
    {
        PixMap       pixmap;
        unsigned int res;
        res = pPixmapElement->QueryFloatAttribute("top", &pixmap.top);
        if (TIXML_NO_ATTRIBUTE == res)
        {
            pixmap.top = 0;
        }
        res = pPixmapElement->QueryFloatAttribute("bottom", &pixmap.bottom);
        if (TIXML_NO_ATTRIBUTE == res)
        {
            pixmap.bottom = 1.0f;
        }
        res = pPixmapElement->QueryFloatAttribute("left", &pixmap.left);
        if (TIXML_NO_ATTRIBUTE == res)
        {
            pixmap.left = 0.0f;
        }
        res = pPixmapElement->QueryFloatAttribute("right", &pixmap.right);
        if (TIXML_NO_ATTRIBUTE == res)
        {
            pixmap.right = 1.0f;
        }

        // [10/16/2009 KingMars]
        fixFloat(pixmap.left);
        fixFloat(pixmap.right);
        fixFloat(pixmap.top);
        fixFloat(pixmap.bottom);

        res = pPixmapElement->QueryIntAttribute("textureId", &pixmap.id);

        pixMapArray.push_back(pixmap);

        pPixmapElement = pPixmapElement->NextSiblingElement();
    }
    // 导出图片功能，just a play
    // _saveTextureToLocal(layer0,"First.dds");
    //  _saveTextureToLocal(tileimageset,"UI.dds");
}

void TileTerrainInfo::LoadGridInfo(const char* fileName, const char* type)
{
    FILE*  pf = fopen(fileName, "rb");

    DWORD  nMagic;
    // 版本号
    DWORD  nVersion;
    // 地表宽度(横向格子数)

    // 7字节版本
    bool   bLargeVersion = false;

    fread(&nMagic, 4, 1, pf);
    fread(&nVersion, 4, 1, pf);

    int    size_x, size_y;
    fread(&size_x, 4, 1, pf);
    fread(&size_y, 4, 1, pf);

    if (nVersion >= 0x00100001)
    {
        fread(&bLargeVersion, sizeof(bool), 1, pf);
    }

    uchar  op1, op0;
    ushort layer1 = 0, layer0 = 0;
    uchar  tr;

    for (int i = 0; i < size_x * size_y; ++i)
    {
        if (!bLargeVersion)
        {
            fread(&layer0, 1, 1, pf);
        }
        else
        {
            fread(&layer0, 2, 1, pf);
        }
        fread(&op0, 1, 1, pf);
        if (!bLargeVersion)
        {
            fread(&layer1, 1, 1, pf);
        }
        else
        {
            fread(&layer1, 2, 1, pf);
        }
        fread(&op1, 1, 1, pf);
        fread(&tr, 1, 1, pf);

        g_info g;
        g.layer0      = layer0 - 1;
        g.layer1      = layer1 - 1;
        g.op0         = op0;
        g.op1         = op1;
        g.tri         = tr;
        g.iBrushCount = 0;
        if (tr != 0)
        {
            // What?这是做甚？
            int a = 0;
        }
        gridInfoArray.push_back(g);
    }
    fclose(pf);
    // 保存到TGA文件中
    /*
     const std::string tgaFileName="MyMap.TGA";
     {
     size_t uCount = size_x*size_y;
     RGBA *rgb = new RGBA[uCount];

     std:: vector<g_info>::iterator iter;
     int i=0;
     for(iter= gridInfoArray.begin() ; iter!=gridInfoArray.end() ; iter ++)
     {
     rgb[i++] = ((ulong)iter->op0 << 16l) | (ulong)iter->op1;
     }
     Image image;
     image.loadDynamicImage((uchar*)rgb , size_x , size_y , 1 , PF_A8R8G8B8);
     image.save(tgaFileName);
     }*/
}

void TileTerrainInfo::LoadLightMap(const char* fileName, const char* type)
{
    Ogre::Image      ligthMapImage;
    ligthMapImage.load(fileName, "General");

    Ogre::TexturePtr layerlightmap = TextureManager::getSingletonPtr()->createManual(
        "<lightmap>", "General", TEX_TYPE_2D, ligthMapImage.getWidth() / 2, ligthMapImage.getHeight() / 2, 1, PF_BYTE_RGBA);

    layerlightmap->getBuffer()->blitFromMemory(ligthMapImage.getPixelBox());
}


void TileTerrainInfo::LoadHightMap(const char* fileName, const char* type)
{
    FILE* pf = fopen(fileName, "rb");
    fseek(pf, 8, SEEK_SET);
    int   height, width;
    fread(&width, 4, 1, pf);
    fread(&height, 4, 1, pf);

    assert(height == this->height + 1);
    assert(width == this->width + 1);


    if (heightMapData)
    {
        delete[]heightMapData;
    }

    heightMapData = new float[height * width];
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            float data;
            fread(&data, 4, 1, pf);
            heightMapData[i * width + j] = data;
        }
    }

    fclose(pf);
}



void TileTerrainInfo::Clear()
{
    gridInfoArray.clear();
    imageArray.clear();
    pixMapArray.clear();
    availableArray.clear();
}

void TileTerrainInfo::AddImageIntoTexture(Ogre::TexturePtr pTexture, int index, const Ogre::Image& image, int gridSize)
{
    HardwarePixelBufferSharedPtr pixptr = pTexture->getBuffer();
    AddImageIntoTexture(pixptr, index, image, gridSize);
    /*
    pixptr->lock(HardwareBuffer::HBL_DISCARD);
    const PixelBox&              pb      = pixptr->getCurrentLock();
    uchar*                       dstData = static_cast<uchar*>(pb.data);
    const uchar*                 srcData = image.getData();

    int                          rowSize = pixptr->getWidth() / gridSize;
    int                          row     = index / rowSize;
    int                          col     = index % rowSize;

    size_t                       pitch = pb.rowPitch; // Skip between rows of image

    size_t                       dst_bpp = 4; // RGBA
    size_t                       src_bpp = Ogre::PixelUtil::getNumElemBytes(image.getFormat());

    size_t                       srcPitch = image.getRowSpan();
    size_t                       dstPitch = pitch * dst_bpp;

    int                          ii = 0, jj = 0;
    for (size_t i = row * gridSize; i < row * gridSize + image.getHeight(); i++)
    {
    ii = 0;
    for (size_t j = col * gridSize; j < col * gridSize + image.getWidth(); j++)
    {
    //memcpy(dstData + (i * dstPitch) + j * dst_bpp, srcData + jj * srcPitch + ii * src_bpp, src_bpp);
    if ( src_bpp < dst_bpp )
    {
    dstData[i * dstPitch + j * dst_bpp + 3] = 255;                  // alpha
    }
    ii++;
    }
    jj++;
    }
    pixptr->unlock();*/
}

void TileTerrainInfo::AddImageIntoTexture(Ogre::HardwarePixelBufferSharedPtr pTexturePtr, int index, const Ogre::Image& image, int gridSize)
{
    HardwarePixelBufferSharedPtr pixptr = pTexturePtr;

    int                          rowSize = pixptr->getWidth() / gridSize;
    int                          maxRow  = pixptr->getHeight() / gridSize - 1;
    int                          row     = index / rowSize;
    // For Hack
    row = min(row, maxRow);
    int                          col = index % rowSize;

    size_t                       top  = row * gridSize;
    size_t                       left = col * gridSize;

    Image::Box                   dstBox(left, top, left + gridSize, top + gridSize);

    /*
    pixptr->blitFromMemory(image.getPixelBox(0, 0), dstBox);

    return;
    */


    pixptr->lock(HardwareBuffer::HBL_DISCARD);
    const PixelBox&              pb      = pixptr->getCurrentLock();
    uchar*                       dstData = static_cast<uchar*>(pb.data);
    const uchar*                 srcData = image.getData();



    size_t                       pitch = pb.rowPitch;             // Skip between rows of image

    size_t                       dst_bpp = Ogre::PixelUtil::getNumElemBytes(pb.format);            // 4; // RGBA
    size_t                       src_bpp = Ogre::PixelUtil::getNumElemBytes(image.getFormat());

    size_t                       srcPitch = image.getRowSpan();
    size_t                       dstPitch = pitch * dst_bpp;

    int                          ii = 0, jj = 0;
    // 周围空一圈像素
    for (size_t i = row * gridSize; i < row * gridSize + gridSize; i++)
    {
        ii = 0;
        for (size_t j = col * gridSize; j < col * gridSize + gridSize; j++)
        {
            memcpy(dstData + (i * dstPitch) + j * dst_bpp, srcData + jj * srcPitch + ii * src_bpp, src_bpp);
            if (src_bpp < dst_bpp)
            {
                dstData[i * dstPitch + j * dst_bpp + 3] = 255;                                          // alpha
            }
            ii++;
        }
        jj++;
    }
    pixptr->unlock();
}
}
