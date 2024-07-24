#define N_IMPLEMENTS nFogNode
//-------------------------------------------------------------------
//  nfog_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/ngfxserver.h"
#include "gfx/nscenegraph2.h"
#include "node/nfognode.h"

//-------------------------------------------------------------------
//  nFogNode()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
nFogNode::nFogNode()
: ref_gs(ks,this)
{
    this->ref_gs = "/sys/servers/gfx";
    this->fog_mode    = N_FOGMODE_LINEAR;
    this->fog_start   = 0.0f;
    this->fog_end     = 1.0f;
    this->fog_density = 1.0f;
    this->fog_color[0] = 1.0f;
    this->fog_color[1] = 1.0f;
    this->fog_color[2] = 1.0f;
    this->fog_color[3] = 1.0f;
}

//-------------------------------------------------------------------
//  ~nFogMode()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
nFogNode::~nFogNode()
{ }

//-------------------------------------------------------------------
//  SetMode()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
void nFogNode::SetMode(nFogMode fm)
{
    this->fog_mode = fm;
}

//-------------------------------------------------------------------
//  GetMode()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
nFogMode nFogNode::GetMode(void)
{
    return this->fog_mode;
}

//-------------------------------------------------------------------
//  SetRange()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
void nFogNode::SetRange(float start, float end)
{
    this->fog_start = start;
    this->fog_end   = end;
}

//-------------------------------------------------------------------
//  GetRange()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
void nFogNode::GetRange(float& start, float& end)
{
    start = this->fog_start;
    end   = this->fog_end;
}

//-------------------------------------------------------------------
//  SetDensity()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
void nFogNode::SetDensity(float density)
{
    n_assert(density >= 0.0f);
    n_assert(density <= 1.0f);
    this->fog_density = density;
}

//-------------------------------------------------------------------
//  GetDensity()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
float nFogNode::GetDensity(void)
{
    return this->fog_density;
}

//-------------------------------------------------------------------
//  SetColor()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
void nFogNode::SetColor(float r, float g, float b, float a)
{
    this->fog_color[0] = r;
    this->fog_color[1] = g;
    this->fog_color[2] = b;
    this->fog_color[3] = a;
}

//-------------------------------------------------------------------
//  GetColor()
//  23-Mar-00   floh    created
//-------------------------------------------------------------------
void nFogNode::GetColor(float& r, float& g, float& b, float& a)
{
    r = this->fog_color[0];
    g = this->fog_color[1];
    b = this->fog_color[2];
    a = this->fog_color[3];
}

//-------------------------------------------------------------------
//  Attach()
//  23-Mar-00   floh    created
//  22-Jun-01   floh    new scene graph stuff
//-------------------------------------------------------------------
bool nFogNode::Attach(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);
    if (nVisNode::Attach(sceneGraph)) 
    {
        sceneGraph->AttachAmbientFogNode(this);
        return true;
    } 
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  Scenegraph calls Compute() to ask me to render.
//  23-Mar-00   floh    created
//  22-Jun-01   floh    new scene graph stuff
//-------------------------------------------------------------------
void nFogNode::Compute(nSceneGraph2 *sceneGraph)
{
    nVisNode::Compute(sceneGraph);

    nGfxServer *gs = this->ref_gs.get();
    nRState rs;
    rs.Set(N_RS_FOGMODE,this->fog_mode);        gs->SetState(rs);
    rs.Set(N_RS_FOGSTART,this->fog_start);      gs->SetState(rs);
    rs.Set(N_RS_FOGEND,this->fog_end);          gs->SetState(rs);
    rs.Set(N_RS_FOGDENSITY,this->fog_density);  gs->SetState(rs);
    rs.Set(N_RS_FOGCOLOR,&(this->fog_color));   gs->SetState(rs);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------


