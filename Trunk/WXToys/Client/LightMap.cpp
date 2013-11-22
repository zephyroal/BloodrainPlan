#include "StdAfx.h"
#include "LightMap.h"

LightMap::LightMap(void)
    : m_image(NULL)
{
}

LightMap::~LightMap(void)
{
}

void LightMap::_loadLightmap(const string& filename)
{
    if (filename.empty())
    {
        return;
    }

    // Image img((CStringW)(CStringA)filename.c_str() );

    // img.GetType();
    //
    m_image.reset(new Image((CStringW)(CStringA)filename.c_str()));


    UINT        width       = m_image->GetWidth();
    UINT        height      = m_image->GetHeight();
    PixelFormat PixelFormat = m_image->GetPixelFormat();
    int         frames      = m_image->GetFrameDimensionsCount();

    m_strInfomaiton = filename.c_str();
    m_strInfomaiton.AppendFormat(TEXT("\npixel format:%d , w:%d,h:%d,frames:%d"), PixelFormat, width, height, frames);

    int         i = 0;

    // if (mLightmapImage->getDepth() != 1 ||
    //	mLightmapImage->getNumFaces() != 1 ||
    //	mLightmapImage->getWidth() != mXSize * 8 ||
    //	mLightmapImage->getHeight() != mZSize * 8)
    // {
    //	delete mLightmapImage;
    //	mLightmapImage = 0;
    //	Ogre::LogManager::getSingleton().logMessage("============================NOTICE===============================");
    //	Ogre::LogManager::getSingleton().logMessage("Terrain lightmap out of date, please update terrain lightmap.");
    //	Ogre::LogManager::getSingleton().logMessage("=================================================================");
    //	return;
    // }


    // String grp = groupName;
    // if (!Ogre::ResourceGroupManager::getSingleton().resourceExists(grp, filename))
    //	return;

    // if (type == "image")
    // {
    //	mLightmapImage = new Ogre::Image;
    //	try
    //	{
    //		mLightmapImage->load(filename, grp);
    //	}
    //	catch (const Ogre::Exception&)
    //	{
    //		// In case fail to load image for some reason, just ignore here.
    //		delete mLightmapImage;
    //		mLightmapImage = 0;
    //		return;
    //	}

    //	if (mLightmapImage->getDepth() != 1 ||
    //		mLightmapImage->getNumFaces() != 1 ||
    //		mLightmapImage->getWidth() != mXSize * 8 ||
    //		mLightmapImage->getHeight() != mZSize * 8)
    //	{
    //		delete mLightmapImage;
    //		mLightmapImage = 0;
    //		Ogre::LogManager::getSingleton().logMessage("============================NOTICE===============================");
    //		Ogre::LogManager::getSingleton().logMessage("Terrain lightmap out of date, please update terrain lightmap.");
    //		Ogre::LogManager::getSingleton().logMessage("=================================================================");
    //		return;
    //	}
    // }
}

void LightMap::Draw(CDC& dc, int width, int height)
{
    Image* img = m_image.get();
    if (img)
    {
        Graphics graphics(dc.m_hDC);
        Status   status = graphics.GetLastStatus();
        if (Ok == status)
        {
            graphics.DrawImage(img, 0, 0, width, height);
            // enjoy of bitmap;
        }
    }

    __super::Draw(dc, width, height);
}
