#define N_IMPLEMENTS nSceneGraph2
//------------------------------------------------------------------------------
//  nscenegraph2_render.cc
//  (C) 2001 A.Weissflog
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "gfx/ntexturearray.h"
#include "gfx/npixelshader.h"
#include "gfx/nvertexbuffer.h"
#include "node/nmeshnode.h"
#include "node/nshadernode.h"
#include "shadow/nshadowserver.h"

//------------------------------------------------------------------------------
/**
    If mipmap LOD is enabled, reset all texture's mip levels to the 
    least detailed. The mip levels will then be bumped up individually
    in AdjustMipLevel().
*/
void
nSceneGraph2::ResetMipLevel(nTextureArray* texArray)
{
    n_assert(texArray);

    if (this->mipLodEnabled)
    {
        int i;
        for (i = 0; i < N_MAXNUM_TEXSTAGES; i++)
        {
            nTexture* tex = texArray->GetTexture(i);
            if (tex)
            {
                int numMipLevels = tex->GetNumMipLevels();
                tex->SetMipLod(numMipLevels - 1);
                tex->SetUseCount(0);
            }
        }
    }
    else
    {
        // no texture mipmap LOD: reset textures to most detailed mip lod
        int i;
        for (i = 0; i < N_MAXNUM_TEXSTAGES; i++)
        {
            nTexture* tex = texArray->GetTexture(i);
            if (tex)
            {
                tex->SetMipLod(0);
                tex->SetUseCount(0);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Adjust the maximum mipmap level for a vertex buffer / texture array
    set, modify the texture mip LOD if it is greater then before.
    Simply takes the distance to the view plane as indicator for the
    required miplevel.

    Also resets the per-frame-usecount in the textures.
*/
void
nSceneGraph2::AdjustMipLevel(const matrix44& modelView, nTextureArray* texArray, int mipLodBias)
{
    n_assert(this->mipLodEnabled);
    n_assert(texArray);
    n_assert(this->mipLodTable);

    // get distance to view plane
    float dist = modelView.pos_component().len();

    // find mipmap table entry
    int mipLod;
    for (mipLod = 0; mipLod < this->numMipLods; mipLod++)
    {
        if (dist < this->mipLodTable[mipLod])
        {
            break;
        }
    }

    // set mipmap lod's
    int i;
    for (i = 0; i < N_MAXNUM_TEXSTAGES; i++)
    {
        nTexture* tex = texArray->GetTexture(i);
        if (tex)
        {
            int curLod = tex->GetMipLod();
            int lod = mipLod + mipLodBias;
            if (lod < curLod)
            {
                tex->SetMipLod(lod);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Transform nodes, and split them into different sets based on the
    contained visnode objects into:
    - geometry nodes
    - light nodes
    - sound nodes
*/
void
nSceneGraph2::TransformAndSplitNodes()
{
    this->numGeoNodes   = 0;
    this->numLightNodes = 0;
    this->numSoundNodes = 0;
    short i;
    for (i = 0; i < this->curNodePoolIndex; i++)
    {
        nSceneNode* node = &(this->nodePool[i]);
        
        // split nodes
        if (node->lightNode)
        {
            if (this->numLightNodes < MAX_LIGHTNODES)
            {
                this->lightNodes[this->numLightNodes++] = i;
            }
        }
        if (node->meshNode || node->visualNode)
        {
            if (this->numGeoNodes < MAX_GEONODES)
            {
                this->geoNodes[this->numGeoNodes++] = i;
            }
        }
        if (node->soundNode)
        {
            if (this->numSoundNodes < MAX_SOUNDNODES)
            {
                this->soundNodes[this->numSoundNodes++] = i;
            }
        }
    }
    this->curNode = -1;
}

//------------------------------------------------------------------------------
/**
    qsort() hook for geometry nodes to minimize state switches.
    Sorting priority is as follows:

    -# render priority
    -# opacity
    -# texture
    -# pixel shader
    -# vertex shader

    Within transparent block strictly by Z.
*/
int
SortHook(const short *i1, const short *i2)
{
    n_assert(nSceneGraph2::uglyNodePoolPtr);
    nSceneGraph2::nSceneNode* n1 = &(nSceneGraph2::uglyNodePoolPtr[*i1]);
    nSceneGraph2::nSceneNode* n2 = &(nSceneGraph2::uglyNodePoolPtr[*i2]);
    int res;

    // render priority
    res = n1->renderPri - n2->renderPri;
    if (res != 0) return res;

    // opacity
    int t1 = (n1->flags & nSceneGraph2::nSceneNode::ISOPAQUE) ? 1:0;
    int t2 = (n2->flags & nSceneGraph2::nSceneNode::ISOPAQUE) ? 1:0;
    res = t2 - t1;
    if (res != 0) return res;

    if (n1->flags & nSceneGraph2::nSceneNode::ISOPAQUE)
    {
        // within opaque block: by texture array objects
        nTextureArray *ta1 = (nTextureArray *) n1->texArray;
        nTextureArray *ta2 = (nTextureArray *) n2->texArray;
        if (ta1 || ta2)
        {
            if (ta1)
            {
                if (ta2)
                {
                    res = ta1->Compare(ta2);
                    if (res != 0) return res;
                }
                else return +1;
            }
            else return -1;
        }

        // within opaque block: by pixel shader
        nShaderNode *s1 = (nShaderNode *) n1->shaderNode;
        nShaderNode *s2 = (nShaderNode *) n2->shaderNode;
        if (s1 || s2)
        {
            if (s1)
            {
                if (s2)
                {
                    res = s1->GetPixelShader() - s2->GetPixelShader();
                    if (res != 0) return res;
                }
                else return +1;
            }
            else return -1;
        }

        // within opaque block: by mesh node
        nMeshNode *m1 = (nMeshNode *) n1->meshNode;
        nMeshNode *m2 = (nMeshNode *) n2->meshNode;
        if (m1 || m2)
        {
            if (m1)
            {
                if (m2)
                {
                    res = m1->GetVertexBuffer() - m2->GetVertexBuffer();
                    if (res != 0) return res;
                }
                else return +1;
            }
            else return -1;
        }
        return res;

    }
    else
    {
        // within transparency block sort by Z
        float dz = n1->matrix.M43 - n2->matrix.M43;
        if (dz > 0.0f)      return +1;
        else if (dz < 0.0f) return -1;
        else                return 0;
    }
}

//------------------------------------------------------------------------------
/*
    Update and render all light sources in the scene.

    @param  gfxServ     pointer to gfx server to render light source to
    @param  shadowServ  pointer to shadow server
*/
void
nSceneGraph2::RenderLights(nGfxServer* gfxServ, nShadowServer* shadowServ)
{
    n_assert(gfxServ && shadowServ);
    bool doShadows = shadowServ->GetCastShadows();

    // for each light node...
    short i;
    for (i = 0; i < this->numLightNodes; i++)
    {
        this->curNode = this->lightNodes[i];
        nSceneNode* node = &(this->nodePool[this->curNode]);
        nVisNode* lightNode = node->lightNode;
        n_assert(lightNode);

        // update light node and get pointer to light object
        node->light = 0;
        lightNode->Compute(this);
        if (node->light)
        {
            // render the light
            gfxServ->SetMatrix(N_MXM_MODELVIEW, node->matrix);
            gfxServ->SetLight(*(node->light));

            // attach the light to the shadow system
            if (doShadows && node->light->GetCastShadows())
            {
                shadowServ->AttachLight(node->matrix, node->light);
            }
        }
    }
    this->curNode = -1;
}

//------------------------------------------------------------------------------
/*
    Update and render all sound emitters and the listener in the scene.

    @param  audioServ   pointer to audio server
*/
void
nSceneGraph2::RenderSounds(nAudioServer2 *audioServ)
{
    n_assert(audioServ);

    // for each sound emitter...
    short i;
    for (i = 0; i < this->numSoundNodes; i++)
    {
        this->curNode = this->soundNodes[i];
        nSceneNode* node = &(this->nodePool[this->curNode]);
        nVisNode* soundNode = node->soundNode;
        n_assert(soundNode);

        // let sound node update and render itself
        soundNode->Compute(this);
    }
    this->curNode = -1;

    // render the listener node
    // (FIXME: render listener inside nListenerNode::Compute())
    if (this->ambientListener)
    {
        this->curNode = this->ambientListenerNode;
        this->ambientListener->Compute(this);
    }
}

//------------------------------------------------------------------------------
/*
    Sort the geometry nodes for minimal state switches.
*/
void
nSceneGraph2::SortGeoNodes()
{
    int i;

    // update texture nodes (we need the textures for sorting)
    for (i = 0; i < this->numGeoNodes; i++)
    {
        this->curNode = this->geoNodes[i];
        nSceneNode* node = &(this->nodePool[this->curNode]);
        if (node->texArrayNode)
        {
            nVisNode* ta = node->texArrayNode;
            node->texArray = 0;
            ta->Compute(this);
            
            // reset mipmap LOD levels
            if (node->texArray)
            {
                this->ResetMipLevel(node->texArray);
            }
        }
    }
    this->curNode = -1;

    // adjust texture mipmap levels
    if (this->mipLodEnabled)
    {
        for (i = 0; i < this->numGeoNodes; i++)
        {
            this->curNode = this->geoNodes[i];
            nSceneNode* node = &(this->nodePool[this->curNode]);
            if (node->texArray)
            {
                // adjust mipmap LOD
                int mipLodBias = 0;
                nShaderNode* shaderNode = (nShaderNode*) node->shaderNode;
                if (shaderNode)
                {
                    mipLodBias = shaderNode->GetMipLodBias();
                }
                this->AdjustMipLevel(node->matrix, node->texArray, mipLodBias);
            }
        }
    }

    // do the qsort
    uglyNodePoolPtr = this->nodePool;
    qsort(&(this->geoNodes), this->numGeoNodes, sizeof(short),
         (int(*)(const void *, const void *)) SortHook);
    uglyNodePoolPtr = 0;
}

//------------------------------------------------------------------------------
/**
    Render the scene, called from nSceneGraph2::EndScene(). This will
    sort the attached nodes to ensure minimal render state switches,
    and finally make them visible.
*/
void
nSceneGraph2::RenderScene()
{
    nGfxServer* gfxServ       = this->refGfx.get();
    nShadowServer* shadowServ = this->refShadow.get();
    nAudioServer2* audioServ  = 0;
    if (this->refAudio.isvalid())
    {
        audioServ = this->refAudio.get();
    }

    // shadows enabled?
    bool doShadows = shadowServ->GetCastShadows();

    // split nodes by type
    this->TransformAndSplitNodes();

    // begin shadow casting
    if (doShadows)
    {
        shadowServ->BeginScene();
    }

    // render lights
    this->RenderLights(gfxServ, shadowServ);

    // render audio
    if (audioServ)
    {
        this->RenderSounds(audioServ);
    }

    // sort geometry nodes
    this->SortGeoNodes();

    // render optional fog node
    if (this->ambientFog)
    {
        this->curNode = this->ambientFogNode;
        this->ambientFog->Compute(this);
    }

    // render the geometry nodes
    int i;
    for (i = 0; i < this->numGeoNodes; i++)
    {
        this->curNode = this->geoNodes[i];
        nSceneNode* node = &(this->nodePool[this->curNode]);

        // compute current state of pixel shader
        if (node->shaderNode)
        {
            nVisNode* shaderNode = node->shaderNode;
            node->pixelShader = 0;
            shaderNode->Compute(this);
        }

        // compute current state of mesh and optionally shadow caster
        if (node->meshNode)
        {
            nVisNode* meshNode = node->meshNode;
            node->vertexBuffer = 0;
            meshNode->Compute(this);
        }

        // set modelview matrix for this node
        gfxServ->SetMatrix(N_MXM_MODELVIEW, node->matrix);

        // render mesh
        if (node->vertexBuffer)
        {
            // make sure we don't render any ReadOnly vertex buffers
            n_assert(N_VBTYPE_READONLY != node->vertexBuffer->GetVBufType());
            node->vertexBuffer->Render(node->indexBuffer, node->pixelShader, node->texArray);
        }

        // render shadow caster
        if (doShadows && node->shadowCaster)
        {
            shadowServ->AttachCaster(node->matrix, node->shadowCaster);
        }

        // update and compute optional complex visual node
        // (if the visual node is a shadow caster it must itself attach to
        // the shadow system)
        if (node->visualNode)
        {
            node->visualNode->Compute(this);
        }
    }

    // finish shadow casting scene
    if (doShadows)
    {
        shadowServ->EndScene();
    }

    this->curNode = -1;
}

//------------------------------------------------------------------------------
