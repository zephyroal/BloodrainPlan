//---------------------------------------------------------------
// modified or deveoloped by Shen Yuqing 
// HUST CS 06
// syq.myth@gmail.com
// 2009
//---------------------------------------------------------------
#ifndef __ETTERRAININFO_H__
#define __ETTERRAININFO_H__

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

#include "ETPrerequisites.h"

#include <vector>
using std::vector;
#include <iosfwd>
#include <OgreVector3.h>
#include <OgreString.h>
//  [10/7/2009 KingMars]
#include <OgreAxisAlignedBox.h>

// forward declarations
namespace Ogre
{
  class AxisAlignedBox;
  class Image;
  class ColourValue;
  class DataStream;
  class Ray;
}


namespace ET
{
  /**
   * This class stores all basic information about the terrain, i. e. its heightmap,
   * its extents and the scaling.
   */
	
	//  [10/14/2009 KingMars]
	// 拼接地形的纹理每块长宽为256
	const unsigned int SPLICE_IMAGE_SIZE = 256;
	// 拼接地形纹理有SPLICE_ROW*SPLICE_COL块。
	const unsigned int SPLICE_ROW = 8;//行
	const unsigned int SPLICE_COL = 4;//列

	typedef unsigned char uchar;
	typedef unsigned short ushort;

	typedef struct 
	{
		float left;
		float right;
		float top;
		float bottom;
		int id;
	} PixMap;//纹理Brush在Atlas中的uv坐标及其ID(用了哪一张地表纹理)
	
	typedef struct 
	{
		Ogre::String fileName;
		float row_scale;
		float col_scale;
	} ImageInfo;//相应brush的缩放值，如tl的大多数原图为128*256，这里则为（0.5 1），很蛋疼是不？要改！

	typedef struct 
	{
		ushort layer0;//对应一个PixMap
		ushort layer1;
		uchar op0;
		uchar op1;
		uchar tri;
		int iBrushCount;
	}g_info;//对应记录每个grid所用的bursh及相应的混合值

	class _ETManagerExport TileTerrainInfo
	{
	public:
		TileTerrainInfo()
			:heightMapData( NULL )
		{}

		void LoadTerrain( const Ogre::String& sceneDir, const Ogre::String& terrainFileName);

		int width;
		int height;
		int tileSize;
		 std:: vector<g_info> gridInfoArray;
		 std:: vector<ImageInfo> imageArray;
		 std:: vector<PixMap> pixMapArray;
		 std:: vector<bool> availableArray;
		Ogre::String sceneDir;
		float* heightMapData;
		int scale_x,scale_y,scale_z;		
		
	private:

		void Clear();
		void LoadGridInfo( const char* fileName, const char* type );
		void LoadLightMap( const char* fileName, const char* type );
		void LoadHightMap( const char* fileNaem, const char* type );
		void AddImageIntoTexture( Ogre::TexturePtr pTexture, int index, const Ogre::Image& image, int size );
		void AddImageIntoTexture( Ogre::HardwarePixelBufferSharedPtr pTexturePtr, int index, const Ogre::Image& image, int size );
	};

  class _ETManagerExport TerrainInfo
  {
  public:
    /** Default constructor */
    TerrainInfo();
    /** Constructs with a heightmap given as an array of floats. */
    TerrainInfo(size_t width, size_t height, const float* heightmap = 0);
    /** Constructs with a heightmap given as a vector of floats. */
    TerrainInfo(size_t width, size_t height, const std::vector<float>& heightmap);

    /** Sets a new heightmap from an array of floats. */
    void setHeightmap(size_t width, size_t height, const float* heightmap = 0);
    /** Sets a new heightmap from a vector of floats. */
    void setHeightmap(size_t width, size_t height, const std::vector<float>& heightmap);

    /** Retrieves the raw heightmap data. */
    const std::vector<float>& getHeightmapData() const { return mHeightmap; }

    /** Retrieves a value from the heightmap. */
    float& at(size_t x, size_t y) { return mHeightmap[x + y*mWidth]; }
    /** Retrieves a const value from the heightmap. */
    const float at(size_t x, size_t y) const { return mHeightmap[x + y*mWidth]; }

    /** Retrieves the width of the heightmap. */
    size_t getWidth() const { return mWidth; }
    /** Retrieves the height of the heightmap. */
    size_t getHeight() const { return mHeight; }

    /** Sets the extents of the terrain in Ogre units. */
    void setExtents(const Ogre::AxisAlignedBox& extents);
    /** Retrieves the terrain extents in Ogre units. */
    Ogre::AxisAlignedBox getExtents() const;
	
	//  [10/7/2009 KingMars]
	void setBounding( const Ogre::AxisAlignedBox& box ) { boundingBox = box; }

    /** Retrieves the scaling of one vertex in Ogre units. */
    const Ogre::Vector3& getScaling() const { return mScale; }

    /** Retrieves the offset point of the terrain. */
    const Ogre::Vector3& getOffset() const { return mOffset; }

    /** Scales from Ogre positions to vertex indexes. */
    int posToVertexX(float x) const { return (int) ((x-mOffset.x)/mScale.x); }
    /** Scales from Ogre positions to vertex indexes. */
    int posToVertexZ(float z) const { return (int) ((z-mOffset.z)/mScale.z); }
    /** Scales from vertex indexes to Ogre positions. */
    float vertexToPosX(int x) const { return mOffset.x + x*mScale.x; }
    /** Scales from vertex indexes to Ogre positions. */
    float vertexToPosZ(int z) const { return mOffset.z + z*mScale.z; }

    /** Calculates the height at the given coordinates. */
    float getHeightAt(float x, float z) const;

    /** Calculates the terrain normal at the given coordinates. */
    Ogre::Vector3 getNormalAt(float x, float z) const;

    /** Calculates the terrain tangent at the given coordinates. */
    Ogre::Vector3 getTangentAt(float x, float z) const;

    /**
     * Checks if a ray intersects with the terrain.
     * @param ray  The ray to check for intersection with the terrain.
     * @return (true, point of intersection) if the terrain is intersected.
     */
    std::pair<bool, Ogre::Vector3> rayIntersects(const Ogre::Ray& ray) const;

  private:
    /** extents of the heightmap */
    size_t mWidth, mHeight;
    /** the heightmap */
    std::vector<float> mHeightmap; // why does this NOT generate warnings?! it did with Brush

    /** offset at which the (0, 0) point of the terrain is placed in Ogre */
    Ogre::Vector3 mOffset;
    /** scale of a terrain vertex */
    Ogre::Vector3 mScale;

	// used to ray trace [10/7/2009 KingMars]
	Ogre::AxisAlignedBox boundingBox;
  };


  /** Loads a heightmap from an image and stores it in the given TerrainInfo. */
  void _ETManagerExport loadHeightmapFromImage(TerrainInfo& info, const Ogre::Image& image);

  /** Saves a heightmap to image. */
  void _ETManagerExport saveHeightmapToImage(const TerrainInfo& info, Ogre::Image& image, unsigned int bpp = 2);

  /** Loads a heightmap from a raw data file and stores it in the given TerrainInfo. */
  void _ETManagerExport loadHeightmapFromRawData(TerrainInfo& info, Ogre::DataStream& stream,
    size_t width, size_t height);

  /** Saves a heightmap to a raw data file. */
  void _ETManagerExport saveHeightmapToRawData(const TerrainInfo& info, std::ostream& stream, unsigned int bpp = 2);



  /**
   * Derives a lightmap from the terrain described by the given TerrainInfo.
   * The lightmap is stored in the passed image.
   * @param info      Terrain info to generate the lightmap for
   * @param image     The image to store the lightmap into
   * @param width     The width of the resulting lightmap
   * @param height    The height of the resulting lightmap
   * @param lightDir  The direction from which the light is coming
   * @param lightCol  The colour of the light
   * @param ambient   Ambient colour
   * @param shadowed  Whether to include terrain self-shadows in the lightmap
   */
  void _ETManagerExport createTerrainLightmap(const TerrainInfo& info, Ogre::Image& image,
    size_t width, size_t height, Ogre::Vector3 lightDir, const Ogre::ColourValue& lightCol,
    const Ogre::ColourValue& ambient, bool shadowed = true);

}


#endif
