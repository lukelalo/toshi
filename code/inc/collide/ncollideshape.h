#ifndef N_COLLIDESHAPE_H
#define N_COLLIDESHAPE_H
//------------------------------------------------------------------------------
/**
    @class nCollideShape 
    @ingroup NebulaCollideModule
    @brief describes shapes for collision system

    Holds a triangle list describing a collision shape.
    One nCollideShape object may be shared between several
    nCollideObject objects. 2 nCollideShape objects may also
    be queried directly whether they intersect.

    nCollideShape objects are also able to load themselves
    from a Wavefront file.

    The nCollideShape base class only can do sphere checks.
    Subclasses may link to 3rd party collision systems
    implementing triangle exact collision detection.

    (C) 2001 RadonLabs GmbH
*/  
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#ifndef N_COLLTYPE_H
#include "collide/ncolltype.h"
#endif

#undef N_DEFINES
#define N_DEFINES nCollideShape
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class vector3;
class matrix44;
class nFileServer2;
class nGfxServer;
class nCollideReport;
class nFile;

//------------------------------------------------------------------------------
class N_PUBLIC nCollideShape : public nHashNode {
public:
    /// constructor
    nCollideShape(const char* name);
    /// destructor
    virtual ~nCollideShape();

    /// increment refcount by one
    int AddRef();
    /// decrement refcount by one
    int RemRef();
    /// get current refcount
    int GetRef();
    /// has object been initialized?
    bool IsInitialized();
    /// get radius of collide mesh
    float GetRadius();
    /// begin defining collide mesh
    virtual void Begin(int numVertices, int numTriangles);
    /// set vertex in collide mesh
    virtual void SetVertex(int index, vector3& v);
    /// set triangle in collide mesh
    virtual void SetTriangle(int index, int v0Index, int v1Index, int v2Index);
    /// finish defining the geometry
    virtual void End();
    /// load collide geometry from file
    virtual bool Load(nFileServer2* fileServer, const char* filename);
    /// perform collision with other nCollideShape
    virtual bool Collide(nCollType ctype, matrix44& ownMatrix, nCollideShape* otherShape, matrix44& otherMatrix, nCollideReport& collReport);
    /// perform collision with line
    virtual bool LineCheck(nCollType collType, const matrix44& ownMatrix, const line3& line, nCollideReport& collReport);
    /// perform a sphere check
    virtual bool SphereCheck(nCollType collType, const matrix44& ownMatrix, const sphere& ball, nCollideReport& collReport);
    /// visualize the collide shape
    virtual void Visualize(nGfxServer* gfxServer);

private:
    bool isInitialized;
    int refCount;
    float radius;

    /// prevent default construction
    nCollideShape();
    /// load n3d file
    bool LoadN3D(nFileServer2* fileServer, const char* filename);
    /// load nvx file
    bool LoadNVX(nFileServer2* fileServer, const char* filename);
    /// read 32 bit integer from file
    int readInt(nFile* file);
};

//------------------------------------------------------------------------------
/**
*/
inline
nCollideShape::nCollideShape(const char* name) :
    nHashNode(name),
    refCount(0),
    isInitialized(false),
    radius(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    *** PRIVATE ***
*/
inline
nCollideShape::nCollideShape()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
inline
int
nCollideShape::AddRef()
{
    return ++refCount;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCollideShape::RemRef()
{
    n_assert(refCount > 0);
    return --refCount;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCollideShape::GetRef()
{
    return refCount;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCollideShape::IsInitialized()
{
    return isInitialized;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nCollideShape::GetRadius()
{
    return this->radius;
}

//------------------------------------------------------------------------------
#endif
