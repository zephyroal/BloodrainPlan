#include "StdAfx.h"
#include "WXTerrain.h"


//  [10/14/2009 KingMars]
// 拼接地形的纹理每块长宽为256
const UINT SPLICE_IMAGE_SIZE = 256;
// 拼接地形纹理有SPLICE_ROW*SPLICE_COL块。
const UINT SPLICE_ROW = 8;
const UINT SPLICE_COL = 4;


WXTerrain::WXTerrain(void)
{
    width    = 0;
    height   = 0;
    tileSize = 0;
    scale_x  = scale_y = scale_z  = 0;
}

WXTerrain::~WXTerrain(void)
{
}

// --------------------------------------------------------------------
void WXTerrain::LoadTerrain(CONST CString& strFile)
{
    m_strInfomaiton = strFile;

    TiXmlDocument terrainDoc;
    bool          bLoadSuccess = false;
    bLoadSuccess = terrainDoc.LoadFile((LPCTSTR)strFile);

    if ( !bLoadSuccess )
    {
        throw("load terrain file failed", "");
        return;
    }

    TiXmlElement* root = terrainDoc.FirstChildElement();

    // !
    root->QueryIntAttribute("xsize", &width);
    root->QueryIntAttribute("zsize", &height);
    root->QueryIntAttribute("tileSize", &tileSize);

    const char*   elementValue;
    // scale
    TiXmlElement* element = root->FirstChildElement("scale");

    element->QueryIntAttribute("x", &scale_x);
    element->QueryIntAttribute("y", &scale_y);
    element->QueryIntAttribute("z", &scale_z);

    m_strInfomaiton.AppendFormat("\n\nwidth:%d,height:%d,tilesize:%d,scaleX:%d,scaleY:%d,scaleZ:%d\n", width, height, tileSize
                                 , scale_x, scale_y, scale_z);


    // heightmap
    element = root->FirstChildElement("heightmap");
    const char*   heightmapFileName = element->Attribute("filename");
    const char*   heightmapType     = element->Attribute("type");
    // LoadHightMap( (sceneDir + heightmapFileName).c_str(), heightmapType );

    m_strInfomaiton.AppendFormat("heightmap type:%s,file:%s\n", heightmapType, heightmapFileName);

    // gridinfo
    element = root->FirstChildElement("gridInfo");
    const char*   gridInfoFileName = element->Attribute("filename");
    const char*   gridInfoType     = element->Attribute("type");
    // LoadGridInfo( ( sceneDir + gridInfoFileName ).c_str(), gridInfoType );
    m_strInfomaiton.AppendFormat("gridInfo type:%s,file:%s\n", gridInfoType, gridInfoFileName);

    // ! lightmap
    element = root->FirstChildElement("lightmap");
    if (element)
    {
        const char* lightMapFileName = element->Attribute("filename");
        const char* lightMapType     = element->Attribute("type");
        // LoadLightMap( lightMapFileName, lightMapType );
        m_strInfomaiton.AppendFormat("lightmap type:%s,file:%s\n", lightMapType, lightMapFileName);
    }

    // textures
    element = root->FirstChildElement("textures");

    TiXmlElement* pTxtureElement = element->FirstChildElement();

    // Ogre::TexturePtr layer0 = Ogre::TextureManager::getSingletonPtr()->getByName( "<layer0>");
    // Ogre::TexturePtr tileimageset = Ogre::TextureManager::getSingletonPtr()->getByName("<TileImagesetTexture>");
    // assert( !layer0.isNull() );
    m_strInfomaiton.AppendFormat("textures\n");

    int           imageIndex = 0;
    while ( pTxtureElement )
    {
        elementValue = pTxtureElement->Attribute("type");
        {
            if ( IsStrEqual("image", elementValue))
            {
                size_t  imageWidth, imageHeight;

                elementValue = pTxtureElement->Attribute("filename");
                CString strFile = UTF8ToANSI(elementValue);                  // UTF8ToANSI( elementValue );
                m_imgs.push_back(strFile);

                if (imageIndex < 10)
                {
                    m_strInfomaiton.AppendFormat("texture[%2d] file:%s\n", ++imageIndex, strFile);
                }
                else
                {
                    ++imageIndex;
                }
            }
            else
            {
                throw("unknown type of textures");
            }
            pTxtureElement = pTxtureElement->NextSiblingElement();
        }
    }
    m_strInfomaiton.AppendFormat("texture images counts:%d\n", imageIndex);


    element = root->FirstChildElement("pixmaps");
    m_strInfomaiton.AppendFormat("\npixmaps\n");

    int           pixmapIndex    = 0;
    TiXmlElement* pPixmapElement = element->FirstChildElement();
    while ( pPixmapElement )
    {
        PixMap       pixmap;
        unsigned int res;
        res = pPixmapElement->QueryFloatAttribute("top", &pixmap.top);
        if (TIXML_NO_ATTRIBUTE == res )
        {
            pixmap.top = 0;
        }
        res = pPixmapElement->QueryFloatAttribute("bottom", &pixmap.bottom);
        if (TIXML_NO_ATTRIBUTE == res )
        {
            pixmap.bottom = 1.0f;
        }
        res = pPixmapElement->QueryFloatAttribute("left", &pixmap.left);
        if ( TIXML_NO_ATTRIBUTE == res )
        {
            pixmap.left = 0.0f;
        }
        res = pPixmapElement->QueryFloatAttribute("right", &pixmap.right);
        if ( TIXML_NO_ATTRIBUTE == res )
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

        if (pixmapIndex < 10 )
        {
            m_strInfomaiton.AppendFormat("pixMap[%d] id:%d , left:%f,right:%f,top:%f,bottom:%f\n",
                                         ++pixmapIndex,
                                         pixmap.id,
                                         pixmap.left,
                                         pixmap.right
                                         ,
                                         pixmap.top,
                                         pixmap.bottom);
        }
        else
        {
            ++pixmapIndex;
        }



        pPixmapElement = pPixmapElement->NextSiblingElement();
    }

    m_strInfomaiton.AppendFormat("pixmap counts:%d\n", pixmapIndex);
}


void WXTerrain::Draw(CDC& dc, int width, int height)
{
    CRect rcDraw(0, 0, width, height);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(RGB(255, 255, 0));
    dc.DrawText(m_strInfomaiton, rcDraw, DT_TOP | DT_LEFT | DT_WORDBREAK);
}
