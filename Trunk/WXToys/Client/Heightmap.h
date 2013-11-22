#pragma once

#define  string string


typedef std::vector<float> RealArray;

#define  Real float
#define  uint32 unsigned int

const uint32             TERRAIN_HEIGHTMAP_MAGIC   = 'FEHM';
const uint32             TERRAIN_HEIGHTMAP_VERSION = 0x00100000;

const uint32             TERRAIN_LIGHTMAP_MAGIC   = 'FELM';
const uint32             TERRAIN_LIGHTMAP_VERSION = 0x00100002;

const uint32             TERRAIN_GRIDINFO_MAGIC   = 'FEGI';
const uint32             TERRAIN_GRIDINFO_VERSION = 0x00100002;


class Heightmap : public FileInfoInterface
{
public:
    Heightmap(void);
    ~Heightmap(void);
    void _loadHeightmap(const string& filename);

    RealArray mHeightmap;               // !< Height of each end point, ((width+1) * (depth+1)) elements

    int       mXSize;                                                   // !< X dimension of the map (in grids)
    int       mZSize;                                                   // !< Z dimension of the map (in grids)
    int       mTileSize;                                        // !< Dimensions of each tile (in grids)
    BOOL LoadFile(CONST CString& strFile)
    {
        _loadHeightmap((LPCTSTR)strFile);
        return TRUE;
    }


    void Draw(CDC& dc, int width, int height);


    float     m_nTop;
    float     m_bottom;
};
