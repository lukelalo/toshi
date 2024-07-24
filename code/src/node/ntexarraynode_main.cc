#define N_IMPLEMENTS nTexArrayNode
//------------------------------------------------------------------------------
//  ntexarraynode_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//------------------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "node/ntexarraynode.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass(nTexArrayNode, "nvisnode");


//------------------------------------------------------------------------------
/**
*/
nTexArrayNode::nTexArrayNode() :
    refFileServer(kernelServer, this),
    tex_array(this)
{
    this->refFileServer = "/sys/servers/file2";
    int i;
    for (i=0; i<N_MAXNUM_TEXSTAGES; i++) 
    {
        this->tex_valid[i] = false;
        this->tex_dirty[i] = false;
        this->gen_mipmaps[i] = true;
        this->highQuality[i] = false;
    };
}

//------------------------------------------------------------------------------
/**
    16-Nov-00   floh    created
*/
nTexArrayNode::~nTexArrayNode()
{ }

//------------------------------------------------------------------------------
/**
    @brief Reloads all invalid textures.
*/
void
nTexArrayNode::checkLoadTextures()
{
    int i;
    nGfxServer* gfxServ = this->refGfx.get();
    for (i=0; i<N_MAXNUM_TEXSTAGES; i++) 
    {
        if (this->tex_dirty[i] || 
           (this->tex_valid[i] && (!this->tex_array.GetTexture(i))))
        {
            this->tex_array.SetTexture(gfxServ, 
                                       i,
                                       this->tex_pixel_desc[i].GetAbsPath(),
                                       this->tex_alpha_desc[i].GetAbsPath(),
                                       this->gen_mipmaps[i],
                                       this->highQuality[i]);
            this->tex_dirty[i] = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Preload the textures
*/
void
nTexArrayNode::Preload()
{
    this->checkLoadTextures();
    nVisNode::Preload();
}

//------------------------------------------------------------------------------
/**
    16-Nov-00   floh    created
    31-May-01   floh    new behaviour
*/
bool 
nTexArrayNode::Attach(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);
    if (nVisNode::Attach(sceneGraph))
    {
        sceneGraph->AttachTexArrayNode(this);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Called by scene graph to get embedded texture objects uptodate.
    Will (re-)validate textures on demand (either for the first time
    of if they have become invalid for some reason.
    
    16-Nov-00   floh    created
    31-May-01   floh    new behaviour
*/
void
nTexArrayNode::Compute(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);

    nVisNode::Compute(sceneGraph);

    // (re-)validate textures
    this->checkLoadTextures();

    // set current texture
    sceneGraph->SetTextureArray(&(this->tex_array));
}

//-----------------------------------------------------------------------------
/**
   @brief Define pixel and alpha channel files for a texture stage.
   @param stage Texture stage.
   @param pname Texture map file name.
   @param aname Alpha map file name.
      
   A NULL alpha name pointer indicates that the texture
   has no alpha channel, a NULL pixel name indicates that 
   the texture stage should be cleared.
   
   Note that if alpha name pointer is not NULL it is assumed that both the
   pixel and alpha files are BMPs.
   
   16-Nov-00   floh    created
*/
void 
nTexArrayNode::SetTexture(int stage, const char *pname, const char *aname) 
{
    n_assert((stage>=0) && (stage<N_MAXNUM_TEXSTAGES));
    
    // initialize new filename paths
    const char* cwd = this->refFileServer->GetCwd();
    this->tex_pixel_desc[stage].Set(this->refFileServer.get(), pname, cwd);
    this->tex_alpha_desc[stage].Set(this->refFileServer.get(), aname, cwd);
    
    // initialize the valid and dirty flags
    this->tex_valid[stage] = pname ? true : false;
    this->tex_dirty[stage] = true;
    
    // the rest will happen inside Attach() and Compute()
};

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
