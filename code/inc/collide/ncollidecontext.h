#ifndef N_COLLIDECONTEXT_H
#define N_COLLIDECONTEXT_H
//------------------------------------------------------------------------------
/**
    @class nCollideContext
    @ingroup NebulaCollideModule
    @brief defines a collision space

    A nCollideContext creates a collision space, defined by
    a collection of nCollideObjects which can collide with 
    each other. nCollideObjects can be added and remove
    from the context at any time.
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

#ifndef N_SPHERE_H
#include "mathlib/sphere.h"
#endif

#ifndef N_COLLTYPE_H
#include "collide/ncolltype.h"
#endif

#ifndef N_COLLIDEREPORTHANDLER_H
#include "collide/ncollidereporthandler.h"
#endif

//-------------------------------------------------------------------
class nCollideObject;
class nGfxServer;
typedef int nCollClass;
class nCollideServer;
class N_PUBLIC nCollideContext : public nNode 
{
public:
    /// constructor
    nCollideContext(nCollideServer* _cs);
    /// destructor
    virtual ~nCollideContext();
    /// create a collide object
    virtual nCollideObject *NewObject(void);
    /// release a collide object
    virtual void ReleaseObject(nCollideObject *);
    /// add collide object to context
    virtual void AddObject(nCollideObject *);
    /// remove collide object from context
    virtual void RemObject(nCollideObject *);
    /// compute contacts between collision objects in context
    virtual int Collide(void);
    /// debug visualization of the collide context
    virtual void Visualize(nGfxServer *);
    /// get the collide reports for the collisions computed inside Collide()
    virtual int GetCollissions(nCollideObject *, nCollideReport **&);
    /// do a "moving sphere" check against collide object radii in the context
    virtual int MovingSphereCheck(const vector3& p0, const vector3& v0, float radius, nCollClass cc, nCollideReport **& cr_ptr);
    /// do a line-model check
    virtual int LineCheck(const line3& line, nCollType collType, nCollClass collClass, nCollideReport**& crPtr);
    /// do a sphere-model check
    virtual int SphereCheck(const sphere& ball, nCollType collType, nCollClass collClass, nCollideReport**& crPtr);
    /// get the collide server who owns this context
    nCollideServer* GetCollideServer();
    /// reset position and timestamp of all objects
    void Reset();

private:
    friend class nCollideObject;

    enum {
        MAXNUM_COLLISSIONS = 4096,
    };

    nCollideReportHandler collideReportHandler;     // collide reports for Collide()
    nCollideReportHandler checkReportHandler;       // collide reports for Check() functions
    nCollideServer *cs;     // the collide server who owns us
    nList xdim_list;        // the x-dimension sorted list (2 nodes per object)

protected:
    nList owned_list;       // list of nCollideObjects created by this context
    nList attached_list;    // the list of objects currently attached to the context
    int unique_id;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCollideContext::nCollideContext(nCollideServer* _cs) :
    unique_id(0),
    cs(_cs)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nCollideServer* 
nCollideContext::GetCollideServer()
{
    return this->cs;
}

//-------------------------------------------------------------------
#endif
