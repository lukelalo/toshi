#define N_IMPLEMENTS nBumpMeshNode
//------------------------------------------------------------------------------
/* Copyright (c) 2002 Dirk Ulbricht of Insane Software
 *
 * See the file "nbump_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "node/nbumpmeshnode.h"
#include "gfx/nscenegraph2.h"
#include "gfx/ngfxserver.h"
#include "shadow/nshadowserver.h"
#include "shadow/nshadowcaster.h"

nNebulaScriptClass(nBumpMeshNode, "nvisnode");

//------------------------------------------------------------------------------
/**
*/
nBumpMeshNode::nBumpMeshNode() :
    refGfx(kernelServer, this),
    refShadowServer(kernelServer, this),
    refMesh(kernelServer, this),
    dynVBuf(kernelServer, this),
    refLight(kernelServer, this),
    castShadow(false),
    refShadowCaster(this)
{
    this->refGfx = "/sys/servers/gfx";
    this->refShadowServer = "/sys/servers/shadow";
    this->SetFlags(N_FLAG_SAVEUPSIDEDOWN);
    dot3IsDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
nBumpMeshNode::~nBumpMeshNode()
{
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nBumpMeshNode::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph))
    {
        if (this->dynVBuf.GetReadOnly())
        {
            // in readonly mode, attach as mesh node
            sceneGraph->AttachMeshNode(this);
        }
        else
        {
            // render mode: attach as complex visual node
            sceneGraph->AttachVisualNode(this);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Evaluates and renders the mesh.
*/
void
nBumpMeshNode::RenderMesh(nSceneGraph2* sceneGraph, nVertexBuffer* srcSkin)
{
    n_assert(sceneGraph && srcSkin);
    nIndexBuffer* indexBuffer = sceneGraph->GetIndexBuffer();
    n_assert(indexBuffer);

    if (dot3IsDirty) setupTangentSpace(srcSkin, indexBuffer);

    matrix44 view_matrix;
    refGfx->GetMatrix (N_MXM_VIEWER, view_matrix);

    // get light position in world space
    vector3 light_world(0,0,0);
    if (refLight.isvalid())
      {
      refLight->Compute(sceneGraph);
      light_world = (sceneGraph->GetTransform() * view_matrix).pos_component();
      }

    // put light position in object space
    vector3 light_obj(0,0,0);

    nVisNode* p = (nVisNode*)GetParent();
    p->Compute(sceneGraph);
    matrix44 obj_inv = sceneGraph->GetTransform() * view_matrix;
    obj_inv.invert();

    light_obj = obj_inv * light_world;

    // get render vertex buffer and lock for writing
    nVertexBuffer* dstSkin = this->dynVBuf.Begin(
        indexBuffer,
        sceneGraph->GetPixelShader(),
        sceneGraph->GetTextureArray());
    srcSkin->LockVertices();

    // get src and dst vertex component pointers
    float* srcCoord = srcSkin->coord_ptr;
    float* dstCoord = dstSkin->coord_ptr;
    float* srcNorm  = srcSkin->norm_ptr;
    float* dstNorm  = dstSkin->norm_ptr;
    ulong* dstColor = dstSkin->color_ptr;

    // we assure that the source vertex buffer has at least coords and uvs
    n_assert(srcCoord && srcSkin->uv_ptr);

    float *srcUV[N_MAXNUM_TEXCOORDS];
    float *dstUV[N_MAXNUM_TEXCOORDS];
    int i;
    int numTexCoordSets = 0;
    for (i = 0; i < N_MAXNUM_TEXCOORDS; i++) 
    {
        srcUV[i] = srcSkin->uv_ptr[i];
        dstUV[i] = dstSkin->uv_ptr[i];
        if (srcUV[i])
        {
            numTexCoordSets++;
        }
        else break;
    }
    int numVertices  = srcSkin->GetNumVertices();
    int srcStride = srcSkin->stride4;
    int dstStride = dstSkin->stride4;

    // shadow caster coordinate pointer
    vector3* shadowCoordPtr = 0;
    if (this->refShadowCaster.isvalid() && this->castShadow)
    {
        this->refShadowCaster->Lock();
        shadowCoordPtr = this->refShadowCaster->GetCoordPtr();
    }

    // for each vertex... (-> important to write target buffer continously!)
    for (i = 0; i < numVertices; i++)
    {
        vector3 v(srcCoord[0],srcCoord[1],srcCoord[2]);
        
        // write dst coord
        dstCoord[0] = srcCoord[0];
        dstCoord[1] = srcCoord[1];
        dstCoord[2] = srcCoord[2];
        dstCoord += dstStride;

        // write dst norm
        dstNorm[0] = srcNorm[0];
        dstNorm[1] = srcNorm[1];
        dstNorm[2] = srcNorm[2];
        dstNorm += dstStride;

        srcCoord += srcStride;
        srcNorm  += srcStride;

        // update shadow caster coordinate
        if (shadowCoordPtr)
        {
            shadowCoordPtr->set(v);
            shadowCoordPtr++;
        }

        // generate colors
        vector3 dir, tangentLight;

        dir = light_obj - v;
        dir.norm();
        tangentLight.x = dir % dot3info[i].T;
        tangentLight.y = dir % dot3info[i].S;
        tangentLight.z = dir % dot3info[i].SxT;
      
        tangentLight.norm();

        // each component of tangentlight is now in range [-1, 1]
        // colors only allow a range of [0, 1], so we need to adjust the normal
        float r,g,b;
        r = tangentLight.x * 0.5f + 0.5f;
        g = tangentLight.y * 0.5f + 0.5f;
        b = tangentLight.z * 0.5f + 0.5f;
        dstColor[0] = n_f2rgba(r,g,b,1.0f);
        dstColor += dstStride;
        
        // copy uv coords
        int k;
        for (k = 0; k < numTexCoordSets; k++) 
        {
            dstUV[k][0] = srcUV[k][0];
            dstUV[k][1] = srcUV[k][1];
            srcUV[k] += srcStride;
            dstUV[k] += dstStride;
        }
    }

    // instead of keeping track of the exact bounding box,
    // we create a very conservative bounding box (twice
    // the diameter of the original bounding box)
    vector3 bbdiam = srcSkin->GetBBox().vmax - srcSkin->GetBBox().vmin;
    float l = 2.0f * bbdiam.len();
    vector3 vmin(-l, -l, -l);
    vector3 vmax(+l, +l, +l);
    bbox3 bbox(vmin, vmax);
    dstSkin->SetBBox(bbox);

    // unlock and finish
    if (shadowCoordPtr)
    {
        this->refShadowCaster->Unlock();
        this->refShadowServer->AttachCaster(sceneGraph->GetTransform(),
                                            this->refShadowCaster.get());
    }
    srcSkin->UnlockVertices();
    this->dynVBuf.End(numVertices, indexBuffer->GetNumIndices());

    // if readonly, hand vertex buffer up to whoever wants it
    if (this->dynVBuf.GetReadOnly())
    {
        sceneGraph->SetVertexBuffer(dstSkin);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nBumpMeshNode::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    if (!refMesh.isvalid())
    {
        return;
    }

    // invoke Compute() on mesh and get pointer to source vertex buffer
    this->refMesh->Compute(sceneGraph);
    nVertexBuffer* srcSkin = sceneGraph->GetVertexBuffer();
    n_assert(srcSkin);
    sceneGraph->SetVertexBuffer(0);

    // initialize dynamic vertex buffer if not yet happened
    if (!this->dynVBuf.IsValid())
    {
        // the destination vertex type is the same as the
        // source vertex type plus vertex colors
        int srcVType = srcSkin->GetVertexType();
        int dstVType = srcVType | N_VT_RGBA;
        this->dynVBuf.Initialize(dstVType, srcSkin->GetNumVertices());
    
        // if shadow casting enabled, also initialize a shadow caster
        // (NOTE: the source mesh must be a shadow caster as well!)
        nShadowCaster* srcCaster = sceneGraph->GetShadowCaster();
        if (this->castShadow && srcCaster)
        {
            this->refShadowCaster = this->refShadowServer->NewCaster(0);
            this->refShadowCaster->Initialize(srcCaster->GetNumCoords(),
                                              srcCaster->GetNumEdges());
            this->refShadowCaster->CopyEdges(srcCaster, 0,
                                             srcCaster->GetNumEdges());
        }
    }
    sceneGraph->SetShadowCaster(0);

    // make sure dest vertex buffer is large enough
    n_assert(srcSkin->GetNumVertices() <= this->dynVBuf.GetNumVertices());

    // render the skin
    this->RenderMesh(sceneGraph, srcSkin);
}

//------------------------------------------------------------------------------

// used in the next method, stl doesnt like local structs
struct nTempVertex
{
  vector3 p;
  nArray<int> indices;
};

/// set up tangent, binormal and normal for every vertex
/// this code is based on dot3_util from nvidias nvsdk
void nBumpMeshNode::setupTangentSpace(nVertexBuffer* mesh, nIndexBuffer* tris)
{
  int i;
  int ui;
  dot3IsDirty = false;
  n_assert(tris->GetPrimType() == N_PTYPE_TRIANGLE_LIST);

  // init dot3info list
  dot3info.Clear();
  dot3info.Reallocate(mesh->GetNumVertices(), 0);
  for (i = 0; i < mesh->GetNumVertices(); i++)
    dot3info.PushBack(nDot3Info(vector3(0,0,0),
                                vector3(0,0,0),
                                vector3(0,0,0)));

  tris->Lock();
  ushort* indices = tris->GetIndexPointer();

  mesh->LockVertices();

  for (i = 0; i < tris->GetNumIndices(); i += 3)
  {
    ushort TriIndex[3];
    vector3 edge01;
    vector3 edge02;
    vector3 cp;
  
    TriIndex[0] = indices[i+0];
    TriIndex[1] = indices[i+1];
    TriIndex[2] = indices[i+2];
  
    nDot3Info& v0 = dot3info[TriIndex[0]];
    nDot3Info& v1 = dot3info[TriIndex[1]];
    nDot3Info& v2 = dot3info[TriIndex[2]];

    float* p;
  
    p = mesh->coord_ptr + TriIndex[0]*mesh->stride4;
    vector3 v0_pos(p[0],p[1], p[2]);
    p = mesh->coord_ptr + TriIndex[1]*mesh->stride4;
    vector3 v1_pos(p[0], p[1], p[2]);
    p = mesh->coord_ptr + TriIndex[2]*mesh->stride4;
    vector3 v2_pos(p[0], p[1], p[2]);

    p = mesh->uv_ptr[0] + TriIndex[0]*mesh->stride4;
    vector2 v0_tex(p[0], p[1]);
    p = mesh->uv_ptr[0] + TriIndex[1]*mesh->stride4;
    vector2 v1_tex(p[0], p[1]);
    p = mesh->uv_ptr[0] + TriIndex[2]*mesh->stride4;
    vector2 v2_tex(p[0], p[1]);
  
    // x, s, t
    edge01.set( v1_pos.x - v0_pos.x, v1_tex.x - v0_tex.x, v1_tex.y - v0_tex.y );
    edge02.set( v2_pos.x - v0_pos.x, v2_tex.x - v0_tex.x, v2_tex.y - v0_tex.y );

    cp = edge01 * edge02;
    if ( fabsf(cp.x) > 0.00001f )
    {
      v0.S.x += -cp.y / cp.x;
      v0.T.x += -cp.z / cp.x;

      v1.S.x += -cp.y / cp.x;
      v1.T.x += -cp.z / cp.x;
      
      v2.S.x += -cp.y / cp.x;
      v2.T.x += -cp.z / cp.x;
    }

    // y, s, t
    edge01.set( v1_pos.y - v0_pos.y, v1_tex.x - v0_tex.x, v1_tex.y - v0_tex.y );
    edge02.set( v2_pos.y - v0_pos.y, v2_tex.x - v0_tex.x, v2_tex.y - v0_tex.y );

    cp = edge01 * edge02;
    if ( fabsf(cp.x) > 0.00001f )
    {
      v0.S.y += -cp.y / cp.x;
      v0.T.y += -cp.z / cp.x;

      v1.S.y += -cp.y / cp.x;
      v1.T.y += -cp.z / cp.x;
      
      v2.S.y += -cp.y / cp.x;
      v2.T.y += -cp.z / cp.x;
    }

    // z, s, t
    edge01.set( v1_pos.z - v0_pos.z, v1_tex.x - v0_tex.x, v1_tex.y - v0_tex.y );
    edge02.set( v2_pos.z - v0_pos.z, v2_tex.x - v0_tex.x, v2_tex.y - v0_tex.y );

    cp = edge01 * edge02;
    if ( fabsf(cp.x) > 0.00001f )
    {
      v0.S.z += -cp.y / cp.x;
      v0.T.z += -cp.z / cp.x;

      v1.S.z += -cp.y / cp.x;
      v1.T.z += -cp.z / cp.x;
      
      v2.S.z += -cp.y / cp.x;
      v2.T.z += -cp.z / cp.x;
    }
  }

  // next step is to average the tangent, binormal and normal for all
  // vertices that share the same spatial position. This eliminates
  // visible seams when vertices have the same position, but different
  // uvs.
  nArray<nTempVertex> tempList;
  for (i = 0; i < mesh->GetNumVertices(); i++)
  {
    float* p = mesh->coord_ptr + i*mesh->stride4;
    vector3 pos(p[0], p[1], p[2]);
  
    for (int j = i; j < mesh->GetNumVertices(); j++)
    {
      p = mesh->coord_ptr + j*mesh->stride4;
      vector3 npos(p[0], p[1], p[2]);
    
      if (pos.isequal(npos, 0.00001f))
      {
        nTempVertex* tv=NULL;
      
        // see if that position is already in the list
        for (int klaus=0; klaus < tempList.Size(); klaus++)
        {
          if (tempList[klaus].p.isequal(pos, 0.00001f))
          {
            tv = &tempList[klaus];
            break;
          }
        }
      
        if (!tv)
        {
          tempList.PushBack(nTempVertex());
          tv = &tempList[tempList.Size()-1];
        }
        tv->p = pos;
        tv->indices.PushBack(j);
      }
    }
  }

  for (ui=0; ui < tempList.Size(); ui++)
  {
    int j;
    nTempVertex* v = &tempList[ui];
    nDot3Info ti(vector3(0,0,0), vector3(0,0,0), vector3(0,0,0));
  
    for (j=0; j < v->indices.Size(); j++)
    {
      nDot3Info& di = dot3info[v->indices[j]];
      ti.S += di.S;
      ti.T += di.T;
    }
  
    ti.S.norm();
    ti.T.norm();

    for (j=0; j < v->indices.Size(); j++)
    {
      nDot3Info& di = dot3info[v->indices[j]];
      di.S = ti.S;
      di.T = ti.T;
    }
  }

  // now we are nearly done, just calculate a proper normal vector
  for(ui = 0; ui < dot3info.Size(); ui++)
  {    
    nDot3Info& v = dot3info[ui];
    // Normalize the S, T vectors
    v.S.norm();
    v.T.norm();

    // Get the cross product of the S and T vectors
    v.SxT = v.S * v.T;

    // Need a normalized normal
    float *p = mesh->norm_ptr + i*mesh->stride4;
    vector3 normal(p[0], p[1], p[2]);
    normal.norm();
        
    // Get the direction of the SxT vector
    if (v.SxT % normal < 0.0f)
    {
      v.SxT = -v.SxT;
    }
  }

  mesh->UnlockVertices();
  tris->Unlock();
}
