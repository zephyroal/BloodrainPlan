#include "StdAfx.h"
#include "GridInfo.h"


const uint32 TERRAIN_HEIGHTMAP_MAGIC   = 'FEHM';
const uint32 TERRAIN_HEIGHTMAP_VERSION = 0x00100000;

const uint32 TERRAIN_LIGHTMAP_MAGIC   = 'FELM';
const uint32 TERRAIN_LIGHTMAP_VERSION = 0x00100002;

const uint32 TERRAIN_GRIDINFO_MAGIC   = 'FEGI';
const uint32 TERRAIN_GRIDINFO_VERSION = 0x00100002;


WXGridInfo::WXGridInfo(void)
{
    mXSize = m_width;
    mZSize = m_height;
}

WXGridInfo::~WXGridInfo(void)
{
}


void WXGridInfo::_loadGridInfo(const string& filename)
{
    size_t gridInfoCopysIndex = 0;

    m_strInfomaiton = filename.c_str();
    // Allocate buffers
    mGridInfos.resize(mXSize * mZSize);

    if (filename.empty())
    {
        GridInfo gridInfo = { 0, 0, 0, 0, 0 };
        if (!mPixmaps.empty())
        {
            gridInfo.layers[0].pixmapId = 1;
        }
        std::fill(mGridInfos.begin(), mGridInfos.end(), gridInfo);
        return;
    }
    else
    {
        CFile  file;
        BOOL   bOpened = file.Open(filename.c_str(), CFile::modeRead | CFile::typeBinary);
        if (!bOpened)
        {
            // AfxMessageBox("can't create file");
            return;
        }


        uint32 header[4];
        file.Read(header, 4 * sizeof(uint32));
        if (header[0] != TERRAIN_GRIDINFO_MAGIC)
        {
            throw("TERRAIN_GRIDINFO_MAGIC");
        }
        if (header[1] > TERRAIN_GRIDINFO_VERSION)
        {
            throw("TERRAIN_GRIDINFO_VERSION");
        }
        if (header[1] < 0x00100000)
        {
            throw("0x00100000");
        }
        if (header[1] != TERRAIN_GRIDINFO_VERSION)
        {
            throw("TERRAIN_GRIDINFO_VERSION");
        }
        if (header[2] != mXSize || header[3] != mZSize)
        {
            throw("header[2] != mXSize || header[3] != mZSize");
        }

        bool   pixmapId16Bits = false;
        if (header[1] >= 0x00100002)
        {
            file.Read(&pixmapId16Bits, sizeof(bool));
        }

        //////////////////////////////////////////////////////////////////////////

        // ! pixmaps需要从terrain中读取
        size_t numPixmaps = mPixmaps.size();
        numPixmaps = 441;      // ! suzhou.terrain 441
        int    index = 0;



        m_strInfomaiton.AppendFormat("\n-------------------------------------------------------------------\n");
        m_strInfomaiton.AppendFormat("[id flags pixmapid0 orientaion0 pixmapid1 orientaion1]\n");
        m_strInfomaiton.AppendFormat("%s", "只显示前500个\n");
        for (GridInfoArray::iterator it = mGridInfos.begin(); it != mGridInfos.end(); ++it)
        {
            GridInfo& gridInfo = *it;
            for (size_t i = 0; i < NumLayers; ++i)
            {
                if (pixmapId16Bits)
                {
                    uint16        pixmapId;
                    unsigned char orientation;
                    file.Read(&pixmapId, sizeof(unsigned short));
                    file.Read(&orientation, sizeof orientation);
                    gridInfo.layers[i].pixmapId    = pixmapId;
                    gridInfo.layers[i].orientation = orientation;
                }
                else
                {
                    unsigned char v[2];
                    file.Read(&v, sizeof v);
                    gridInfo.layers[i].pixmapId    = v[0];
                    gridInfo.layers[i].orientation = v[1];
                }
                // pixmapId == 0 is means hole
                if (gridInfo.layers[i].pixmapId > numPixmaps)
                {
                    throw(" Invalid pixmapId in grid info ");
                    /*OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                    "Invalid pixmapId in grid info '" + filename + "'.",
                    "TerrainData::_loadGridInfo");*/
                }
            }

            gridInfo.flags = 0;
            if (header[1] >= 0x00100001)
            {
                unsigned char flags;
                file.Read(&flags, sizeof flags);
                gridInfo.flags = flags;
            }

            if (index < 500)
            {
                m_strInfomaiton.AppendFormat("[%d %d %d %d %d %d] ",
                                             ++index,
                                             gridInfo.flags,
                                             gridInfo.layers[0].pixmapId,
                                             gridInfo.layers[0].orientation
                                             ,
                                             gridInfo.layers[1].pixmapId,
                                             gridInfo.layers[1].orientation);
            }
        }

        //////////////////////////////////////////////////////////////////////////

        // ! check
        int filePos    = file.GetPosition();
        int fileLength = file.GetLength();
    }

    _sortGridInfoCopys();

    GridInfoCopy gridInfoCopy;
    gridInfoCopy.fileName   = filename;
    gridInfoCopy.gridInfos  = mGridInfos;
    gridInfoCopy.gridCount  = mGridInfos.size();
    gridInfoCopy.touchTimes = 1;

    mGridInfoArrayCopys.push_back(gridInfoCopy);
}

struct gridInfoCopyMore
{
    bool operator()(const GridInfoCopy& a, const GridInfoCopy& b) const
    {
        if ( a.touchTimes > b.touchTimes )
        {
            return true;
        }
        else if ( a.touchTimes < b.touchTimes )
        {
            return false;
        }
        else
        {
            return a.gridCount > b.gridCount;
        }
    }
};


void WXGridInfo::_sortGridInfoCopys(void)
{
    const uint8 MAX_GRIDINFOCOPY = 6;

    if (mGridInfoArrayCopys.size() > MAX_GRIDINFOCOPY)
    {
        std::sort(mGridInfoArrayCopys.begin(), mGridInfoArrayCopys.end(), gridInfoCopyMore());

        mGridInfoArrayCopys.pop_back();
    }
}
