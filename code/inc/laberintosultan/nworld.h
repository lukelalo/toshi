#ifndef N_WORLD_H
#define N_WORLD_H
//------------------------------------------------------------------------------
/**
    @class nWorld

    @brief a brief description of the class

    a detailed description of the class

    (C)	2002	ling
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#undef N_DEFINES
#define N_DEFINES nWorld
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class n3DNode;
class nChannelServer;
class nCollideContext;
class nCollideObject;
class nCollideServer;
class nCollideShape;
class nEntity;
class nMapNode;
class nGfxServer; // Debug

class nWorld : public nRoot
{
public:
    /// constructor
    nWorld();
    /// destructor
    virtual ~nWorld();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

    /// Simulate world up to time
    void Run(float time);

    /// Set the visual terrain node, expects nFlatTerrainNode beneath
    void SetTerrain(const char* terrain_path);
    /// Get the visual terrain node
    n3DNode* GetTerrainVisNode();
    /// Get the height at a specific point using x, y
    float GetHeight(const vector3& position) const;
	float GetGravity();
    // Collide methods
    nCollideContext* GetCollideContext();
    nCollideServer* GetCollideServer();
    nCollideShape* NewCollideShape(const char* name, const char* file, bool reload = false);
    nCollideObject* NewCollideObject(nEntity* entity);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    void Step(float dt);

    nAutoRef<nChannelServer> channelServer;
    nAutoRef<nCollideServer> collideServer;
    nCollideContext* collideContext;

    nAutoRef<n3DNode> terrainNode;
    nMapNode* mapTerrainNode;

    /// Current world time
    float gameTime;
    /// Time channel
    int timeChannel;

    static const float stepSize;
};

//---------------------------------------------------------------------------
 inline
nCollideServer*
nWorld::GetCollideServer()
{
    return collideServer.get();
}

//---------------------------------------------------------------------------
/**
*/
inline
nCollideContext*
nWorld::GetCollideContext()
{
    return collideContext;
}
//---------------------------------------------------------------------------

 inline
float
nWorld::GetGravity()
{
    return -100.0f;
}

#endif
