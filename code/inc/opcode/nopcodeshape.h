#ifndef N_OPCODESHAPE_H
#define N_OPCODESHAPE_H
//------------------------------------------------------------------------------
/**
    @class nOpcodeShape
    @ingroup NebulaOpcodeModule

    @brief Implements nCollideShape subclass for use with OPCODE collision
    detection package.

    OPCODE (C) by

    Pierre Terdiman
    May 03, 2001

    p.terdiman@wanadoo.fr
    p.terdiman@codercorner.com
 
    http://www.codercorner.com
    http://www.codercorner.com/Opcode.htm

    Nebula wrapper (C) 2001 RadonLabs GmbH
*/
#ifndef N_COLLIDESHAPE_H
#include "collide/ncollideshape.h"
#endif

#ifndef __OPCODE_H__
#include "opcode/opcode.h"
#endif

using namespace Opcode;

//------------------------------------------------------------------------------
class N_PUBLIC nOpcodeShape : public nCollideShape 
{
public:
    /// constructor
    nOpcodeShape(const char* name);
    /// destructor
    virtual ~nOpcodeShape();

    /// begin defining collide mesh
    virtual void Begin(int numVertices, int numTriangles);
    /// add a vertex
    virtual void SetVertex(int index, vector3& v);
    /// add a triangle
    virtual void SetTriangle(int index, int p0Index, int p1Index, int p2Index);
    /// finish defining geometry
    virtual void End();

    /// perform collision with other nOpcodeShape
    virtual bool Collide(nCollType ctype, matrix44& ownMatrix, nCollideShape* otherShape, matrix44& otherMatrix, nCollideReport& collReport);
    /// perform collision with line
    virtual bool LineCheck(nCollType collType, const matrix44& ownMatrix, const line3& line, nCollideReport& collReport);
    /// perform a sphere check
    virtual bool SphereCheck(nCollType collType, const matrix44& ownMatrix, const sphere& ball, nCollideReport& collReport);
    /// visualize collide mesh
    virtual void Visualize(nGfxServer* gfxServer);

private:
    /// triangle coordinate callback function
    //static void collCallback(udword triangleIndex, VertexPointers& triangle, void * userData);
    /// get tri coords from tri index
    void GetTriCoords(int index, vector3& v0, vector3& v1, vector3& v2);
    /// visualize the AABBTree of the opcode model
    void VisualizeAABBCollisionNode(nGfxServer* gs, const AABBCollisionNode* node);

    BVTCache*         opcTreeCache;
    CollisionFaces*   opcFaceCache;

    nOpcodeServer* collServer;

    MeshInterface opcMeshAccess;
    Model opcModel;
    int numVertices;
    int numFaces;
    float* vertexData;
    int*   faceData;
};

//------------------------------------------------------------------------------
/**
    Extract triangle coordinates from triangle index.
*/
inline
void
nOpcodeShape::GetTriCoords(int index, vector3& v0, vector3& v1, vector3& v2)
{
    int* indexPtr = &(this->faceData[3 * index]);
    float* vp0 = &(this->vertexData[3 * indexPtr[0]]);
    float* vp1 = &(this->vertexData[3 * indexPtr[1]]);
    float* vp2 = &(this->vertexData[3 * indexPtr[2]]);
    v0.set(vp0[0], vp0[1], vp0[2]);
    v1.set(vp1[0], vp1[1], vp1[2]);
    v2.set(vp2[0], vp2[1], vp2[2]);
}

//------------------------------------------------------------------------------
#endif
    



