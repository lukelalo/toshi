#define N_IMPLEMENTS nTrailRender

//------------------------------------------------------------------------------
//  © 2001 Radon Labs GmbH

#include "particle/ntrailrender.h"
#include "gfx/nscenegraph2.h"
#include "gfx/nchannelcontext.h"
#include "particle/npemitter.h"

nNebulaClass(nTrailRender, "nprender");

//------------------------------------------------------------------------------
/**
*/
nTrailRender::nTrailRender() :
    refGfx(kernelServer, this),
    refIBuf(this),
    numQuads(0),
    curQuad(0),
    curVBuf(0),
    coordPtr(0),
    colorPtr(0),
    uvPtr(0),
    stride4(0),
    dynVBuf(kernelServer, this)
{
    refGfx = "/sys/servers/gfx";
}

//------------------------------------------------------------------------------
/**
*/
nTrailRender::~nTrailRender()
{
    // release the nIndexBuffer object if it is valid
    if (refIBuf.isvalid())
    {
        refIBuf->Release();
        refIBuf.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Initialize the dynamic vertex buffer and the index buffer. Should
    only be called when nDynVertexBuffer::IsValid() returns false.
*/
void
nTrailRender::InitBuffers()
{
    n_assert(!refIBuf.isvalid());
    n_assert(!dynVBuf.IsValid());

    // initialize the dynamic vertex buffer object
    dynVBuf.Initialize((N_VT_COORD|N_VT_RGBA|N_VT_UV0), 0);

    // get the number of vertices in the dynamic vertex buffer,
    // and compute the number of triangles which will fit in there
    numQuads = dynVBuf.GetNumVertices() / 4;

    // create and initialize a shared index buffer (all instances
    // of nTrailRender will share one index buffer)
    nIndexBuffer* indexBuffer = refGfx->FindIndexBuffer("ntrailrender_ibuf");
    if (!indexBuffer) {
        // not here yet, create one
        indexBuffer = refGfx->NewIndexBuffer("ntrailrender_ibuf");
        n_assert(indexBuffer);

        // fill the index buffer
        int numIndices = numQuads * 6;
        indexBuffer->Begin(N_IBTYPE_STATIC, N_PTYPE_TRIANGLE_LIST, numIndices);
        int indexIndex  = 0; 
        int vertexIndex = 0;
        for (; vertexIndex < (numQuads * 4); vertexIndex += 4) 
        {
            indexBuffer->Index(indexIndex++, vertexIndex);   
            indexBuffer->Index(indexIndex++, vertexIndex + 1); 
            indexBuffer->Index(indexIndex++, vertexIndex + 2);
            
            indexBuffer->Index(indexIndex++, vertexIndex + 3); 
            indexBuffer->Index(indexIndex++, vertexIndex + 2); 
            indexBuffer->Index(indexIndex++, vertexIndex + 1);
        }
        indexBuffer->End();
    }

    refIBuf = indexBuffer;
}

//------------------------------------------------------------------------------
/**
    Prepare dynamic vertex buffer rendering for this frame.
    Initializes
        curVBuf
        colorPtr
        uvPtr
        stride4
        curQuad

    @param  pixelShader     the nPixelShader object to use for rendering
    @param  textureArray    the nTextureArray object
*/
void
nTrailRender::BeginRender(nPixelShader* pixelShader, nTextureArray* textureArray)
{
    curVBuf  = dynVBuf.Begin(refIBuf.get(), pixelShader, textureArray);
    coordPtr = curVBuf->coord_ptr;
    colorPtr = curVBuf->color_ptr;
    uvPtr    = curVBuf->uv_ptr[0];
    stride4  = curVBuf->stride4;
    curQuad  = 0;
}

//------------------------------------------------------------------------------
/**
    Calls swap on the dynamic vertex buffer. Must be called if the
    current vertex buffer has been filled completely.
*/
void
nTrailRender::SwapRender()
{
    int numVertices = curQuad * 4;
    int numIndices  = curQuad * 6;
    curVBuf  = dynVBuf.Swap(numVertices, numIndices);
    coordPtr = curVBuf->coord_ptr;
    colorPtr = curVBuf->color_ptr;
    uvPtr    = curVBuf->uv_ptr[0];
    stride4  = curVBuf->stride4; 
    curQuad  = 0;
}

//------------------------------------------------------------------------------
/**
    Finishes rendering the dynamic vertex buffer. Must be called once
    a frame at the end of the rendering loop.
*/
void
nTrailRender::EndRender()
{
    int numVertices = curQuad * 4;
    int numIndices  = curQuad * 6;
    dynVBuf.End(numVertices, numIndices);
    curVBuf  = 0;
    coordPtr = 0;
    colorPtr = 0;
    stride4  = 0;
    curQuad  = 0;
}

//------------------------------------------------------------------------------
/**
    Writes a single vertex to the dynamic vertex buffer.

    @param  pos     a vector3, describing view space position
    @param  color   a 32 bit RGBA or BGRA color
    @param  u       texture u coord
    @param  v       texture v coord
*/
void
nTrailRender::WriteVertex(const vector3& pos, ulong color, float u, float v)
{
    coordPtr[0] = pos.x; 
    coordPtr[1] = pos.y; 
    coordPtr[2] = pos.z; 
    coordPtr   += stride4;
    
    colorPtr[0] = color;
    colorPtr   += stride4;

    uvPtr[0] = u;
    uvPtr[1] = v;
    uvPtr   += stride4;
}

//------------------------------------------------------------------------------
/** 
    calculates the positions and colors used by the vertex coords to render this
    crosssection of the trail.

    @param particle         calculations are based on this particle
    @param timeStamp        the time stamp to determine lifetime, age, etc.
    @param view             the view matrix
    @param segmentPosRt     out: the "right" coord position of this section of trail
    @param segmentPosLt     out: the "left" coord pos. of this section of trail
    @param segmentV         out: the age-based V coordinate of this segment
    @param segmentColor     out: the age-based color of the segment


    @return                 true if valid particle generated, else false
*/
bool
nTrailRender::CalcSegmentData(
    nPCorn* particle,
    float timeStamp,
    const matrix44& view,
    vector3& segmentPosRt,
    vector3& segmentPosLt,
    float& segmentV,
    ulong& segmentColor)
{
    nPEmitter* emitter = ref_emitter.get();
    float lifetime = emitter->GetLifetime();
    float age = timeStamp - particle->tstamp;
    if (age <= lifetime)
    {
        float normalizedAge = age / lifetime;
       
        // get interpolated particle attributes
        nPAttrs particleAttributes;
        getParticleAttrs(normalizedAge, particleAttributes);

        // init attributes
        float halfSize = 0.5f * particleAttributes.f[nPAttrs::N_SIZE];
        nColorFormat colorFormat = dynVBuf.GetColorFormat();
        segmentColor =
            (N_COLOR_RGBA == colorFormat) ? particleAttributes.rgba : particleAttributes.bgra;

        // transform the current particle's position and
        // orientation from world space into view space.
        segmentPosRt = particle->p + (particle->xvec * halfSize);
        segmentPosLt = particle->p - (particle->xvec * halfSize);

        segmentPosRt = view * segmentPosRt;
        segmentPosLt = view * segmentPosLt;

        segmentV = normalizedAge;

        return true;
    }
    // particle age not valid
    return false;
}

//------------------------------------------------------------------------------
/**
    given 2 particles, construct the quad between them for the trail.

    @param from         particle one
    @param to           particle two
    @param timeStamp    the time stamp to determine lifetime, age, etc.
    @param view         the view matrix
*/
void
nTrailRender::ConstructQuad(
    nPCorn* from,
    nPCorn* to,
    float timeStamp,
    const matrix44& view)
{
    vector3 fromPosRt, fromPosLt, toPosRt, toPosLt;
    float fromV = 0.0f, toV = 0.0f;
    ulong fromColor = 0, toColor = 0;
    bool fromValid, toValid;

    fromValid = CalcSegmentData(from, timeStamp, view, fromPosRt, fromPosLt, fromV, fromColor);
    toValid   = CalcSegmentData(to,   timeStamp, view, toPosRt,   toPosLt,   toV,   toColor);

    if (fromValid && toValid)
    {
        WriteVertex(fromPosRt, fromColor, 0.0f, fromV);
        WriteVertex(toPosRt,   toColor,   0.0f, toV);
        WriteVertex(fromPosLt, fromColor, 1.0f, fromV);
        WriteVertex(toPosLt,   toColor,   1.0f, toV);

        curQuad++;
        if (curQuad >= numQuads) SwapRender();
    }
}

//------------------------------------------------------------------------------
/**
    Actually render the particles. Pulls particles from our emitter (in
    multiple passes) and build the trail object.

    @param  sceneGraph  pointer to sceneGrap object used to render scene
    @param  timeStamp   current time stamp
*/
void
nTrailRender::RenderParticles(nSceneGraph2* sceneGraph, float timeStamp)
{
    nPEmitter* emitter = ref_emitter.get();

    if (emitter->BeginPullParticles(sceneGraph->GetRenderContext(), timeStamp)) 
    {
        BeginRender(sceneGraph->GetPixelShader(), sceneGraph->GetTextureArray());

        //----------------------------------------------------------------------
        // FIXME: nebula bug: the inverse view and view matrices returned are 
        //        flipped (i.e. the view is the inv view and vice versa).
        //----------------------------------------------------------------------

        // get the view matrix
        nGfxServer *gfxServ = refGfx.get();
        matrix44 view;
        matrix44 invview;
        gfxServ->PushMatrix(N_MXM_MODELVIEW);
        gfxServ->GetMatrix(N_MXM_INVVIEWER, view);
        gfxServ->GetMatrix(N_MXM_VIEWER, invview);
        gfxServ->SetMatrix(N_MXM_MODELVIEW, identity);

        nPCorn* particles = 0;
        nPCorn* olderParticle = 0;
        nPCorn* currentParticle = 0;

        int numParticles = 0;
        while ((numParticles = emitter->PullParticles(particles)))
        {
            //  keep the last value of olderParticle, otherwise initialize.
            int i;
            if (!olderParticle)
            {
                olderParticle = &(particles[0]);
                i = 1;
            } 
            else
            {
                i = 0;
            }

            for (; i < numParticles; i++)
            {
                currentParticle = &(particles[i]);
                ConstructQuad(olderParticle, currentParticle, timeStamp, view);
                olderParticle = currentParticle;
            }
        }
        //  construct a virtual origin particle to connect the youngest particle
        //  to the emitter.
        n_assert(olderParticle);
        nPCorn origin = *olderParticle;
        origin.p = invview * sceneGraph->GetTransform().pos_component();
        origin.tstamp = timeStamp;

        ConstructQuad(olderParticle, &origin, timeStamp, view);

        // flush the vertex buffer
        EndRender();
        emitter->EndPullParticles();

        // restore modelview matrix
        gfxServ->PopMatrix(N_MXM_MODELVIEW);
    }
}

//------------------------------------------------------------------------------
/**
    nTrailRender acts as a complex node, so that the scene graph will call us
    back to do actual rendering.

    @param  sceneGraph  the scene graph to attach to.
    @return             success
*/
bool
nTrailRender::Attach(nSceneGraph2* sceneGraph)
{
    if (nVisNode::Attach(sceneGraph))
    {
        if (!dynVBuf.IsValid())
        {
            InitBuffers();
        }
        sceneGraph->AttachVisualNode(this);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Pull particles from emitter and render them through the dynamic 
    vertex buffer. Actual rendering happens here

    @param  sceneGraph  the scene graph instance we are attached to
    @return             success
*/
void
nTrailRender::Compute(nSceneGraph2* sceneGraph)
{
    nVisNode::Compute(sceneGraph);
    if (ref_emitter.isvalid()) 
    {
        nChannelContext* chnContext = sceneGraph->GetChannelContext();
        n_assert(chnContext);
        float timeStamp = chnContext->GetChannel1f(this->localChannelIndex);
        RenderParticles(sceneGraph, timeStamp);
    }
}
