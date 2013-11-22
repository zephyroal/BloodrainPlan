#pragma once

#define  string string
#define uint32 unsigned int
#define int unsigned int
#define ushort unsigned short

#define uint16 unsigned short

#define uint8 unsigned char

#define Real float
// typedef unsigned int uint32;
// typedef unsigned short uint16;
// typedef unsigned char uint8;



struct LayerInfo
{
    ushort pixmapId;                // !< the pixmap this layer info reference to, 0 means hole, otherwise access by mPixmaps[pixmapId-1]
    ushort orientation;             // !< combine flags of OrientationFlags

    bool operator==(const LayerInfo& rhs) const
    {
        return pixmapId == rhs.pixmapId &&
               orientation == rhs.orientation;
    }

    bool operator!=(const LayerInfo& rhs) const
    {
        return !(*this == rhs);
    }
};

enum
{
    NumLayers = 2,                  // !< number texture layers
};


struct GridInfo
{
    LayerInfo layers[NumLayers];
    int       flags;

    enum Flag
    {
        FlipDiagonal     = 1 << 0,
            DisableDecal = 1 << 1,
    };

    // const Corner* getCorners(void) const
    // {
    //	return msConnerMap[(flags & FlipDiagonal) ? 1 : 0];
    // }

    bool operator==(const GridInfo& rhs) const
    {
        return layers[0] == rhs.layers[0] &&
               layers[1] == rhs.layers[1] &&
               flags == rhs.flags;
    }

    bool operator!=(const GridInfo& rhs) const
    {
        return !(*this == rhs);
    }
};

typedef std::vector<GridInfo> GridInfoArray;

struct GridInfoCopy
{
    string        fileName;
    GridInfoArray gridInfos;
    uint32        touchTimes;
    uint32        gridCount;
};

enum OrientationFlags
{
    FlipHorizontal   = 1,               // !< Horizontal flip the pixmap
    FlipVertical     = 2,               // !< Vertical flip the pixmap
    RotateRightAngle = 4,               // !< Rotate the pixmap anticlockwise 90 degree
    MirrorDiagonal   = 8,           // !< Mirror the pixmap by diagonal
};

// NOTE: Some code dependence with this enum, search [Corner Map] for detail
enum Corner
{
    LEFT_TOP     = 0,
    RIGHT_TOP    = 1,
    LEFT_BOTTOM  = 2,
    RIGHT_BOTTOM = 3,
};



// NOTE: Some code dependence with fields order of this structure, search [Pixmap Fields Order] for detail
struct Pixmap
{
    size_t textureId;               // !< the texture this pixmap reference to, access by mTextures[textureId]
    Real   left;                    // !< defining the left of area on the texture that makes up this pixmap
    Real   top;                     // !< defining the top of area on the texture that makes up this pixmap
    Real   right;                   // !< defining the right of area on the texture that makes up this pixmap
    Real   bottom;                  // !< defining the bottom of area on the texture that makes up this pixmap

    Pixmap()
    {
        // null constructor
    }

    explicit Pixmap(size_t textureId)
        : textureId(textureId)
        , left(0)
        , top(0)
        , right(1)
        , bottom(1)
    {
    }

    Pixmap(size_t textureId, Real left, Real top, Real right, Real bottom)
        : textureId(textureId)
        , left(left)
        , top(top)
        , right(right)
        , bottom(bottom)
    {
    }

    bool operator==(const Pixmap& rhs) const
    {
        return textureId == rhs.textureId &&
               left == rhs.left && right == rhs.right &&
               top == rhs.top && bottom == rhs.bottom;
    }

    bool operator!=(const Pixmap& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<(const Pixmap& rhs) const
    {
        if (textureId < rhs.textureId)
        {
            return true;
        }
        if (textureId > rhs.textureId)
        {
            return false;
        }
        if (left < rhs.left)
        {
            return true;
        }
        if (left > rhs.left)
        {
            return false;
        }
        if (top < rhs.top)
        {
            return true;
        }
        if (top > rhs.top)
        {
            return false;
        }
        if (right < rhs.right)
        {
            return true;
        }
        if (right > rhs.right)
        {
            return false;
        }
        if (bottom < rhs.bottom)
        {
            return true;
        }
        if (bottom > rhs.bottom)
        {
            return false;
        }
        return false;
    }
};



typedef std::vector<GridInfoCopy> GridInfoArrayCopys;

typedef std::vector<Pixmap> PixmapArray;



class WXGridInfo : public FileInfoInterface
{
public:
    WXGridInfo(void);
    ~WXGridInfo(void);

    void _loadGridInfo(const string& filename);


    GridInfoArrayCopys mGridInfoArrayCopys;


    GridInfoArray      mGridInfos;      // !< Grid info array, (width * depth) elements

    PixmapArray        mPixmaps;        // !< The pixmaps the each layer reference to

    int                mXSize, mZSize;

    void _sortGridInfoCopys(void);
    BOOL LoadFile(CONST CString& strFile)
    {
        _loadGridInfo((LPCTSTR)strFile);

        return TRUE;
    }
};
