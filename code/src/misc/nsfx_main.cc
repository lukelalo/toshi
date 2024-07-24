#define N_IMPLEMENTS nSpecialFxServer
//-------------------------------------------------------------------
//  nsfx_main.cc
//  (C) 2000 RadonLabs -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nscenegraph2.h"
#include "node/n3dnode.h"
#include "node/noverlayplane.h"
#include "misc/nspecialfxserver.h"

//-------------------------------------------------------------------
/**
    - 09-May-00   floh    created
*/
//-------------------------------------------------------------------
nSpecialFxServer::nSpecialFxServer()
: ref_root(ks,this)
{
    this->ref_root = "/sys/servers/specialfx/root";

    // alpha overlay parameters
    this->ovr_activated = false;
    this->ovr_red   = 0.0f;
    this->ovr_green = 0.0f;
    this->ovr_blue  = 0.0f;
    this->ovr_alpha = 0.0f;
}

//-------------------------------------------------------------------
/**
    - 09-May-00   floh    created
*/
//-------------------------------------------------------------------
nSpecialFxServer::~nSpecialFxServer()
{
    // empty
}

//-------------------------------------------------------------------
/**
    @brief Must be called BEFORE nSceneGraph::BeginScene().

    - 09-May-00   floh    created
*/
//-------------------------------------------------------------------
void nSpecialFxServer::Begin(void)
{
    // reset one shot effects...
    this->ovr_activated = false;
    this->ovr_red   = 0.0f;
    this->ovr_green = 0.0f;
    this->ovr_blue  = 0.0f;
    this->ovr_alpha = 0.0f;
}

//-------------------------------------------------------------------
/**
    @brief Activate/Deactivate special fx nodes.

    - 09-May-00   floh    
*/
//-------------------------------------------------------------------
void nSpecialFxServer::End(nSceneGraph2* sceneGraph)
{
    n_assert(sceneGraph);

    if (this->ref_root.isvalid()) 
    {
        sceneGraph->Attach(this->ref_root.get(), 0);
    }
}

//-------------------------------------------------------------------
/**
    - 09-May-00   floh    created
*/
//-------------------------------------------------------------------
void nSpecialFxServer::AddOverlay(float r, float g, float b, float a)
{
    this->ovr_activated = true;
    this->ovr_red   += r;
    this->ovr_green += g;
    this->ovr_blue  += b;
    this->ovr_alpha += a;
    if (this->ovr_red > 1.0f)        this->ovr_red   = 1.0f;
    else if (this->ovr_red < 0.0f)   this->ovr_red   = 0.0f;
    if (this->ovr_green > 1.0f)      this->ovr_green = 1.0f;
    else if (this->ovr_green < 0.0f) this->ovr_green = 0.0f;
    if (this->ovr_blue > 1.0f)       this->ovr_blue  = 1.0f;
    else if (this->ovr_blue < 0.0f)  this->ovr_blue  = 0.0f;
    if (this->ovr_alpha > 1.0f)      this->ovr_alpha = 1.0f;
    else if (this->ovr_alpha < 0.0f) this->ovr_alpha = 0.0f;
}

//-------------------------------------------------------------------
/**
    - 05-Jun-00   floh    created
*/
//-------------------------------------------------------------------
bool nSpecialFxServer::GetOverlay(float& r, float& g, float& b, float& a)
{
    r = this->ovr_red;
    g = this->ovr_green;
    b = this->ovr_blue;
    a = this->ovr_alpha;
    return this->ovr_activated;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

