// ---------------------------------------------------------------
// modified or developed by Shen Yuqing
// HUST CS 06
// syq.myth@gmail.com
// 2009
// ---------------------------------------------------------------

#ifndef TerrainLiquid_h__
#define TerrainLiquid_h__

#include <list>
using std::list;
#include <OgreSimpleRenderable.h>

class TerrainInfo;
const int                    LIQUID_TEX_SIZE = 4;

static bool isFloatEqual(float a, float b)
{
    return (a - b > -0.001f && a - b < 0.001f) ? true : false;
}

class TerrainLiquid : public Ogre::SimpleRenderable
{
public:
    TerrainLiquid();
    ~TerrainLiquid();

    // --------------------------------------------------------------------
    // overridden member from MoveableObject
    // --------------------------------------------------------------------

    /** Get the 'type flags' for this MovableObject.
    @remarks
    A type flag identifies the type of the MovableObject as a bitpattern.
    This is used for categorical inclusion / exclusion in SceneQuery
    objects. By default, this method returns all ones for objects not
    created by a MovableObjectFactory (hence always including them);
    otherwise it returns the value assigned to the MovableObjectFactory.
    Custom objects which don't use MovableObjectFactory will need to
    override this if they want to be included in queries.
    */
    virtual Ogre::uint32 getTypeFlags() const;

    /** Retrieves the radius of the origin-centered bounding sphere
    for this object.
    */
    virtual Ogre::Real getBoundingRadius(void) const;


    // --------------------------------------------------------------------
    // overridden members from Renderable
    // --------------------------------------------------------------------

    /** Returns the camera-relative squared depth of this renderable.
    @remarks
    Used to sort transparent objects. Squared depth is used rather than
    actual depth to avoid having to perform a square root on the result.
    */
    virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

    // --------------------------------------------------------------------

    // create TerrainLiquid based on the TerrainInfo
    void createTerrainLiquid(const Ogre::Vector3&   seedPos,
                             float                  fTextureScale,
                             bool                   bDepthEnable,
                             float                  fDepthScale,
                             const ET::TerrainInfo& terrainInfo);


protected:
    const Ogre::Quaternion& getWorldOrientation() const;
    const Ogre::Vector3& getWorldPosition() const;


private:
    enum
    {
        LEFT_TOP,
        LEFT_BOTTOM,
        RIGHT_TOP,
        RIGHT_BOTTOM
    };

    enum
    {
        FOUR_DIR,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    struct POINT
    {
        int x;
        int z;

        POINT()
            : x(0), z(0)
        {
        }

        POINT(int x, int z)
            : x(x), z(z)
        {
        }

        bool operator<(const POINT& p)
        {
            if ( z < p.z )
            {
                return true;
            }
            else if ( z > p.z )
            {
                return false;
            }
            else if ( x < p.x )
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool operator ==(const POINT& p)
        {
            return (p.x == x && p.z == z) ? true : false;
        }
    };

    void                    __createVertexData();
    void                    __createIndexData();
    void                    __addGrid(int x, int z);
    bool                    __isValidGrid(int x, int z, int direction);
    Ogre::Vector2   __getTexCoord(int x, int z, int type);
    Ogre::Real              __getDepthTexCoord(int x, int z);
    void                    __spreed(int x, int z, int direction);
    void                    __spreedOneStep(int x, int z);
    bool                    __isGridContained(int x, int z);

    Ogre::Vector3   __getPos(int x, int z)
    {
        return Ogre::Vector3(mSeedPos.x + x * mStep.x, mSeedPos.y, mSeedPos.z + z * mStep.z);
    }

    Ogre::VertexData*                m_pVertex;
    Ogre::IndexData*                 m_pIndex;
    Ogre::Vector3                    mSeedPos;
    Ogre::Vector3                    mStep;
    const ET::TerrainInfo*           mTerrainInfo;
    float                            m_fTextureScale;
    float                            m_fDepthScale;
    bool                             m_bDepthEnable;

    std::list<TerrainLiquid::POINT>* m_pTempGridArray;
};


#endif // TerrainLiqud_h__
