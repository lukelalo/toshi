#define N_IMPLEMENTS nOpcodeShape
//------------------------------------------------------------------------------
//  nopcodeshape.cc
//  (C) 2001 RadonLabs GmbH
//
// 16-Jul-03  gjh   Modified to work with opcode 1.3 
//------------------------------------------------------------------------------
#include "mathlib/vector.h"
#include "mathlib/triangle.h"
#include "mathlib/line.h"
#include "gfx/ngfxserver.h"
#include "opcode/nopcodeserver.h"
#include "opcode/nopcodeshape.h"
#include "opcode/OPC_OptimizedTree.h"

//------------------------------------------------------------------------------
/**
*/
nOpcodeShape::nOpcodeShape(const char* name) :
    nCollideShape(name),
    numVertices(0),
    numFaces(0),
    vertexData(0),
    faceData(0)
{
    // initialize pointers to global OPCODE objects
    this->collServer = (nOpcodeServer*) nOpcodeServer::kernelServer->Lookup("/sys/servers/collide");
    n_assert(this->collServer);
    this->opcTreeCache    = &(collServer->opcTreeCache);
    this->opcFaceCache    = &(collServer->opcFaceCache);
}

//------------------------------------------------------------------------------
/**
*/
nOpcodeShape::~nOpcodeShape()
{
    if (this->vertexData)
    {
        n_free(this->vertexData);
        this->vertexData = 0;
    }
    if (this->faceData)
    {
        n_free(this->faceData);
        this->faceData = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nOpcodeShape::Begin(int numVerts, int numTris)
{
    n_assert(!this->vertexData);
    n_assert(!this->faceData);
    
    this->numVertices = numVerts;
    this->numFaces    = numTris;
    this->vertexData  = (float*) n_malloc(numVerts * 3 * sizeof(float));
    this->faceData    = (int*)   n_malloc(numTris  * 3 * sizeof(int));

    nCollideShape::Begin(numVerts, numTris);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpcodeShape::SetVertex(int index, vector3& v)
{
    n_assert(this->vertexData);
    n_assert((index >= 0) && (index < this->numVertices));

    float* ptr = this->vertexData + 3 * index;
    ptr[0] = v.x;
    ptr[1] = v.y;
    ptr[2] = v.z;
    nCollideShape::SetVertex(index, v);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpcodeShape::SetTriangle(int index, int p0Index, int p1Index, int p2Index)
{
    n_assert(this->faceData);
    n_assert((index >= 0) && (index < this->numFaces));
    n_assert((p0Index >= 0) && (p0Index < this->numVertices));
    n_assert((p1Index >= 0) && (p1Index < this->numVertices));
    n_assert((p2Index >= 0) && (p2Index < this->numVertices));

    int* ptr = this->faceData + 3 * index;
    ptr[0] = p0Index;
    ptr[1] = p1Index;
    ptr[2] = p2Index;

    nCollideShape::SetTriangle(index, p0Index, p1Index, p2Index);
}

//------------------------------------------------------------------------------
/**
*/
void
nOpcodeShape::End()
{
  n_assert((this->numVertices > 0) && (this->numFaces > 0));
  n_assert(this->faceData && this->vertexData);

  opcMeshAccess.SetNbTriangles(this->numFaces);
  opcMeshAccess.SetNbVertices(this->numVertices);
  // not using callbacks anymore in order to comply with ODE tri-collider
  //opcMeshAccess.SetCallback(nOpcodeShape::collCallback, this);
  opcMeshAccess.SetPointers((IndexedTriangle*)this->faceData, (Point*)this->vertexData);
  opcMeshAccess.SetStrides(sizeof(int) * 3, sizeof(float) * 3);

  // Build tree
  BuildSettings Settings;
  Settings.mRules = SPLIT_BEST_AXIS; // dunno what this means, stole it from ODE :)

  OPCODECREATE opcc;
  opcc.mIMesh = &opcMeshAccess;
  opcc.mSettings.mRules = SPLIT_SPLATTER_POINTS;   // split by splattering primitive centers (???)
  opcc.mSettings.mLimit = 1;              // build a complete tree, 1 primitive/node
  opcc.mNoLeaf    = false; // true;
  opcc.mQuantized = false; // true;
  this->opcModel.Build(opcc);

  nCollideShape::End();
}

//------------------------------------------------------------------------------
/**
    OPCODE uses a callback function to actually get triangle data for the
    collision test.
*/
/**
void
nOpcodeShape::collCallback(udword triangleIndex, VertexPointers& triangle, void * userData)
{
    nOpcodeShape* self = (nOpcodeShape*) userData;
    int *indexPtr = &(self->faceData[3 * triangleIndex]);
    triangle.Vertex[0] = (Point*) &(self->vertexData[3 * indexPtr[0]]);
    triangle.Vertex[1] = (Point*) &(self->vertexData[3 * indexPtr[1]]);
    triangle.Vertex[2] = (Point*) &(self->vertexData[3 * indexPtr[2]]);
}
*/
//------------------------------------------------------------------------------
/**
*/
bool
nOpcodeShape::Collide(nCollType ctype,
                      matrix44& m0, 
                      nCollideShape* otherShape, 
                      matrix44& m1, 
                      nCollideReport& collReport)
{
    n_assert(otherShape);
    n_assert((ctype == COLLTYPE_EXACT) || (ctype == COLLTYPE_CONTACT));

    nOpcodeShape* opcodeOther = (nOpcodeShape*) otherShape;
    AABBTreeCollider& collider = this->collServer->opcTreeCollider;

    // setup tree collider callbacks and user data
  // --callbacks for opcode 1.3 now included in the models/cache, set below-- GJH

    if (ctype == COLLTYPE_EXACT)
    {
        collider.SetFirstContact(false);
    }
    else
    {
        collider.SetFirstContact(true);
    }

    // setup the bvt cache
    this->opcTreeCache->Model0 = &(this->opcModel);
    this->opcTreeCache->Model1 = &(opcodeOther->opcModel);

    // perform collision test (the type casting between matrix44 and Matrix4x4 is a bit adventurous)
    Matrix4x4* m0Ptr = (Matrix4x4*) &m0;
    Matrix4x4* m1Ptr = (Matrix4x4*) &m1;
    collider.Collide(*(this->opcTreeCache), m0Ptr, m1Ptr);

		bool collided = false;
		
    // get the number of collided triangle pairs
    int numPairs = collider.GetNbPairs();
    
    if (numPairs > 0)
    {
				collided = true;
				
        // get the list of collided triangles 
        const Pair* pairs = collider.GetPairs();

        // clamp number of collisions to a reasonable amount
        if (numPairs > 10) numPairs = 10;
        int i;
        vector3 tp[2][3];
        line3 l[2][3];
        triangle t[2];
        bool intersects[2][3];
        float ipos[2][3];
        for (i = 0; i < numPairs; i++)
        {
            // get the current contact triangle coordinates
            this->GetTriCoords(pairs[i].id0, tp[0][0], tp[0][1], tp[0][2]);
            opcodeOther->GetTriCoords(pairs[i].id1, tp[1][0], tp[1][1], tp[1][2]);

            // transform triangle coords into world space
            int j;
            for (j = 0; j < 3; j++)
            {
                tp[0][j] = m0 * tp[0][j];
                tp[1][j] = m1 * tp[1][j];
            }

            // build mathlib triangles...
            t[0].set(tp[0][0], tp[0][1], tp[0][2]);
            t[1].set(tp[1][0], tp[1][1], tp[1][2]);

            // build the 3 lines for each triangles and do intersection
            l[0][0].set(t[0].point(0), t[0].point(1));
            l[0][1].set(t[0].point(0), t[0].point(2));
            l[0][2].set(t[0].point(1), t[0].point(2));

            l[1][0].set(t[1].point(0), t[1].point(1));
            l[1][1].set(t[1].point(0), t[1].point(2));
            l[1][2].set(t[1].point(1), t[1].point(2));

            // test triangle 0 lines against triangle 1
            // test triangle 1 lines against triangle 0
            for (j = 0; j < 3; j++) 
            {
                intersects[0][j] = t[1].intersect_both_sides(l[0][j], ipos[0][j]);
                intersects[1][j] = t[0].intersect_both_sides(l[1][j], ipos[1][j]);
            }

            // get averaged intersection position, we use this as the contact point
            int numIsects = 0;
            int k;
            vector3 contact(0.0f, 0.0f, 0.0f);
            for (j = 0; j < 2; j++) 
            {
                for (k = 0; k < 3; k++) 
                {
                    if (intersects[j][k]) 
                    {
                        contact += l[j][k].ipol(ipos[j][k]);
                        numIsects++;
                    }
          else
          {
            contact += l[j][k].start();
            numIsects++;
          }
                }
            }

            if (numIsects>0) 
            {
                contact /= float(numIsects);
            } 
        
            // fill the contact point into the collision report
            collReport.contact    += contact;
            collReport.co1_normal += t[0].normal();
            collReport.co2_normal += t[1].normal();
        }

        // average collide results
        if (numPairs > 0) 
        {
            float div = 1.0f / float(numPairs);
            collReport.contact    *= div;
            collReport.co1_normal *= div;
            collReport.co2_normal *= div;
            collReport.co1_normal.norm();
            collReport.co2_normal.norm();
            return true;
        } 
        else 
        {
            return false;
        }
    }
    
    return collided;
}

//------------------------------------------------------------------------------
/**
    Check contact of line with shape. The collType is interpreted as
    follows:

     - COLLTYPE_IGNORE:        illegal (makes no sense)
     - COLLTYPE_QUICK:         occlusion check only
     - COLLTYPE_CONTACT:       return closest contact
     - COLLTYPE_EXACT:         same as closest contact

    @param  collType        see above
    @param  ownMatrix       position/orientation of this shape
    @param  line            line definition in world space
    @param  collReport      will be filled with result
    @return                 true if line intersects shape
*/
bool
nOpcodeShape::LineCheck(nCollType collType, 
                        const matrix44& ownMatrix, 
                        const line3& line, 
                        nCollideReport& collReport)
{
    n_assert(COLLTYPE_IGNORE != collType);
  
    // get normed direction and length of line
    vector3 dir = line.m;
    dir.norm();
    float len = line.len();

    // setup ray collider
    RayCollider& collider = this->collServer->opcRayCollider;
// callbacks in opcode 1.3 go through the model's meshinterface - GJH
//    collider.SetCallback(collCallback, udword(this));
    collider.SetMaxDist(len);
    collider.SetClosestHit(false);
    switch (collType)
    {
        case COLLTYPE_QUICK:
            collider.SetFirstContact(true);
            break;

        case COLLTYPE_CONTACT:
        case COLLTYPE_EXACT:
            collider.SetFirstContact(false);
            break;

        default:
            break;
    }

    // convert matrix44 to Opcode Matrix4x4
    Matrix4x4* opcMatrix = (Matrix4x4*) &ownMatrix;

    // build Opcode ray from line
    Ray ray;
    ray.mOrig.Set(line.b.x, line.b.y, line.b.z);
    ray.mDir.Set(dir.x, dir.y, dir.z);

    // perform collision
    collider.Collide(ray, this->opcModel, opcMatrix);

    // get collision result
    if (collider.GetContactStatus())
    {
        // fill out contact point and collision normal of closest contact
        const CollisionFace* collFaces = this->opcFaceCache->GetFaces();
        int numFaces = this->opcFaceCache->GetNbFaces();
        if (numFaces > 0)
        {
            // if in closest hit mode, find the contact with the smallest distance
            int collFaceIndex = 0;
            if (COLLTYPE_CONTACT)
            {
                int i;
                for (i = 0; i < numFaces; i++)
                {
                    if (collFaces[i].mDistance < collFaces[collFaceIndex].mDistance)
                    {
                        collFaceIndex = i;
                    }
                }
            }
            int triangleIndex = collFaces[collFaceIndex].mFaceID;
            float dist        = collFaces[collFaceIndex].mDistance;

            // build triangle from from faceIndex
            vector3 v0,v1,v2;
            this->GetTriCoords(triangleIndex, v0, v1, v2);
            triangle tri(v0, v1, v2);

            // get 3x3 matrix to transform normal into global space
            matrix33 m33(ownMatrix.M11, ownMatrix.M12, ownMatrix.M13, 
                         ownMatrix.M21, ownMatrix.M22, ownMatrix.M23, 
                         ownMatrix.M31, ownMatrix.M32, ownMatrix.M33); 

            // fill collide report
            collReport.contact    = line.b + (dir * dist);
            collReport.co1_normal = m33 * tri.normal();
            collReport.co2_normal = collReport.co1_normal;
        
            return true;
        }
        else
        {
            n_printf("nOpcodeShape::RayCheck(): Contact but no faces!\n");
            return false;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Check contact of a sphere with shape. The collType is interpreted as
    follows:

     - COLLTYPE_IGNORE:        illegal (makes no sense)
     - COLLTYPE_QUICK:         first contact check only
     - COLLTYPE_CONTACT:       return closest contact
     - COLLTYPE_EXACT:         same as closest contact

    Currently, sphere checks always work in first constact mode
    (COLLTYPE_QUICK).

    @param  collType        see above
    @param  ownMatrix       position/orientation of this shape
    @param  sphere          sphere definition in world space
    @param  collReport      will be filled with result
    @return                 true if line intersects shape
*/
bool
nOpcodeShape::SphereCheck(nCollType collType,
                          const matrix44& ownMatrix,
                          const sphere& ball,
                          nCollideReport& collReport)
{
    n_assert(COLLTYPE_IGNORE != collType);

    // setup sphere collider
    SphereCollider& collider = this->collServer->opcSphereCollider;
    SphereCache& cache = this->collServer->opcSphereCache;

// opcode 1.3 uses callbacks in the model's meshinterface -GJH
//    collider.SetCallback(collCallback, (udword)this);

    // SPHERE CHECK ALWAYS WORKS IN FIRST CONTACT MODE!!!
    collider.SetFirstContact(true);

/*
    THIS CODE SWITCHES BETWEEN FIRST AND ALL CONTACTS

    switch (collType)
    {
        case COLLTYPE_QUICK:
            collider.SetFirstContact(true);
            break;

        case COLLTYPE_CONTACT:
        case COLLTYPE_EXACT:
            collider.SetFirstContact(false);
            break;

        default:
            break;
    }
*/

    // convert matrix44 to Opcode Matrix4x4
    const Matrix4x4* opcMatrix = (Matrix4x4*) &ownMatrix;

    // build identity matrix because sphere is already in world space
    Matrix4x4 identity;
    identity.Identity();

    // build an Opcode Sphere from Nebula sphere object
    const Sphere opcSphere(Point(ball.p.x, ball.p.y, ball.p.z), ball.r);

    // perform collision
    collider.Collide(cache, opcSphere, this->opcModel, &identity, opcMatrix);

    // get collision result
    if (collider.GetContactStatus())
    {
        // fill out contact point and collision normal of closest contact
        const udword* collFaces = collider.GetTouchedPrimitives();
        int numFaces = collider.GetNbTouchedPrimitives();
        if (numFaces > 0)
        {
            n_assert(1 == numFaces);

            // build triangle from from faceIndex
            vector3 v0,v1,v2;
            this->GetTriCoords(collFaces[0], v0, v1, v2);
            triangle tri(v0, v1, v2);

            // get 3x3 matrix to transform normal into global space
            matrix33 m33(ownMatrix.M11, ownMatrix.M12, ownMatrix.M13,
                         ownMatrix.M21, ownMatrix.M22, ownMatrix.M23,
                         ownMatrix.M31, ownMatrix.M32, ownMatrix.M33);

            // fill collide report
            const float div = 1.0f / 3.0f;
            vector3 midpoint = (v0 + v1 + v2) * div;
            collReport.contact    = ownMatrix * midpoint;
            collReport.co1_normal = m33 * tri.normal();
            collReport.co2_normal = collReport.co1_normal;
            return true;
        }
        else
        {
            n_printf("nOpcodeShape::SphereCheck(): Contact but no faces!\n");
            return false;
        }
    }

    // FIXME!
    return false;
}

//------------------------------------------------------------------------------
/**
    Render a AABBCollisionNode and recurse.
*/
void
nOpcodeShape::VisualizeAABBCollisionNode(nGfxServer* gs, const AABBCollisionNode* node)
{
    n_assert(gs && node);

    vector3 center(node->mAABB.mCenter.x, node->mAABB.mCenter.y, node->mAABB.mCenter.z);
    vector3 extent(node->mAABB.mExtents.x, node->mAABB.mExtents.y, node->mAABB.mExtents.z);

    vector3 v00(center.x - extent.x, center.y - extent.y, center.z - extent.z);
    vector3 v01(center.x - extent.x, center.y - extent.y, center.z + extent.z);
    vector3 v02(center.x + extent.x, center.y - extent.y, center.z + extent.z);
    vector3 v03(center.x + extent.x, center.y - extent.y, center.z - extent.z);

    vector3 v10(center.x - extent.x, center.y + extent.y, center.z - extent.z);
    vector3 v11(center.x - extent.x, center.y + extent.y, center.z + extent.z);
    vector3 v12(center.x + extent.x, center.y + extent.y, center.z + extent.z);
    vector3 v13(center.x + extent.x, center.y + extent.y, center.z - extent.z);

    // render ground rect
    gs->Coord(v00.x, v00.y, v00.z); gs->Coord(v01.x, v01.y, v01.z);
    gs->Coord(v01.x, v01.y, v01.z); gs->Coord(v02.x, v02.y, v02.z);
    gs->Coord(v02.x, v02.y, v02.z); gs->Coord(v03.x, v03.y, v03.z);
    gs->Coord(v03.x, v03.y, v03.z); gs->Coord(v00.x, v00.y, v00.z);

    // render top rect
    gs->Coord(v10.x, v10.y, v10.z); gs->Coord(v11.x, v11.y, v11.z);
    gs->Coord(v11.x, v11.y, v11.z); gs->Coord(v12.x, v12.y, v12.z);
    gs->Coord(v12.x, v12.y, v12.z); gs->Coord(v13.x, v13.y, v13.z);
    gs->Coord(v13.x, v13.y, v13.z); gs->Coord(v10.x, v10.y, v10.z);

    // render vertical lines
    gs->Coord(v00.x, v00.y, v00.z); gs->Coord(v10.x, v10.y, v10.z);
    gs->Coord(v01.x, v01.y, v01.z); gs->Coord(v11.x, v11.y, v11.z);
    gs->Coord(v02.x, v02.y, v02.z); gs->Coord(v12.x, v12.y, v12.z);
    gs->Coord(v03.x, v03.y, v03.z); gs->Coord(v13.x, v13.y, v13.z);

    if (!node->IsLeaf())
    {
        const AABBCollisionNode* neg = node->GetNeg();
        const AABBCollisionNode* pos = node->GetPos();
        this->VisualizeAABBCollisionNode(gs, neg);
        this->VisualizeAABBCollisionNode(gs, pos);
    }
}

//------------------------------------------------------------------------------
/**
    Renders the collide model's triangle soup through the provided 
    gfx server.
*/
void
nOpcodeShape::Visualize(nGfxServer* gs)
{
    n_assert(gs);
    n_assert(this->vertexData && this->faceData);

    // render the triangle soup
    gs->Begin(N_PTYPE_LINE_LIST);
    int i;
    for (i = 0; i < this->numFaces; i++)
    {
        int* indexPtr = this->faceData + 3 * i;
        float* v0 = this->vertexData + 3 * indexPtr[0];
        float* v1 = this->vertexData + 3 * indexPtr[1];
        float* v2 = this->vertexData + 3 * indexPtr[2];
        
        gs->Coord(v0[0], v0[1], v0[2]); gs->Coord(v1[0], v1[1], v1[2]);
        gs->Coord(v1[0], v1[1], v1[2]); gs->Coord(v2[0], v2[1], v2[2]);
        gs->Coord(v2[0], v2[1], v2[2]); gs->Coord(v0[0], v0[1], v0[2]);
    }
    gs->End();

    // render the AABB tree
/*    gs->Rgba(0.5f, 0.0f, 0.0f, 1.0f);
    gs->Begin(N_PTYPE_LINE_LIST);
    const AABBCollisionTree* tree = (const AABBCollisionTree*) this->opcModel.GetTree();
    this->VisualizeAABBCollisionNode(gs, tree->GetNodes());
    gs->End();
*/
}

//------------------------------------------------------------------------------
