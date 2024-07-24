#ifndef N_OPCODESERVER_H
#define N_OPCODESERVER_H
//------------------------------------------------------------------------------
/**
    @class nOpcodeServer
    @ingroup NebulaOpcodeModule

    @brief Collision server using the OPCODE collision system by 
    Pierre Terdiman
    May 03, 2001

    p.terdiman@wanadoo.fr
    p.terdiman@codercorner.com
 
    http://www.codercorner.com
    http://www.codercorner.com/Opcode.htm

    Nebula wrapper (C) 2001 RadonLabs GmbH
*/
#ifndef N_COLLIDESERVER_H
#include "collide/ncollideserver.h"
#endif

#ifndef __OPCODE_H__
#include "opcode/opcode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nOpcodeServer
#include "kernel/ndefdllclass.h"

using namespace Opcode;

//------------------------------------------------------------------------------
class N_PUBLIC nOpcodeServer : public nCollideServer
{
    friend class nOpcodeShape;

public:
    /// default constructor.
    nOpcodeServer();
    /// default destructor.
    virtual ~nOpcodeServer();

    /// create a collide shape object
    virtual nCollideShape* NewShape(const char* id);

    /// contains this class' type information.
    static nClass* clazz;
    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

private:
    AABBTreeCollider opcTreeCollider;
    RayCollider      opcRayCollider;
    SphereCollider   opcSphereCollider;
    BVTCache         opcTreeCache;
    CollisionFaces   opcFaceCache;
    SphereCache      opcSphereCache;
};
//------------------------------------------------------------------------------
#endif

