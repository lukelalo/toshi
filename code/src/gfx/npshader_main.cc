#define N_IMPLEMENTS nPixelShader
//-------------------------------------------------------------------
//  npshader_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "gfx/npixelshader.h"
#include "gfx/npixelshaderdesc.h"
#include "gfx/ntexture.h"
#include "gfx/ngfxserver.h"

nNebulaClass(nPixelShader, "nroot");

//-------------------------------------------------------------------
/**
    - 23-Aug-00   floh    created
*/
//-------------------------------------------------------------------
nPixelShader::nPixelShader()
: ref_gs(kernelServer,this)
{
    this->ref_gs = "/sys/servers/gfx";
    this->in_begin_render = false;
    this->tarray          = NULL;
    this->ps_desc         = NULL;
}

//-------------------------------------------------------------------
/**
    - 21-Aug-00   floh    created
    - 14-Sep-00   floh    if gfx server's current pixel shader is
                          identical with us, clear it...
*/
//-------------------------------------------------------------------
nPixelShader::~nPixelShader()
{
    if (this == this->ref_gs->GetCurrentPixelShader()) {
        n_printf("nPixelShader::~nPixelShader(): clearing current pixel shader\n");
        this->ref_gs->SetCurrentPixelShader(NULL);
    }
}

//-------------------------------------------------------------------
/**
    @brief  Compile the static state of the externel pixel shader desc into
    a display list.

    - 21-Aug-00   floh    created
*/
//-------------------------------------------------------------------
bool nPixelShader::Compile(void)
{
    n_error("Pure virtual function called.\n");
    return false;
}

//-------------------------------------------------------------------
/**
    @brief  Begin a (possibly multipass) render operation
    
    @return the number of passes required to render the pixel shader.

    - 28-Aug-00   floh    created
    -  16-Nov-00   floh    + nTextureArray parameter
*/
//-------------------------------------------------------------------
int nPixelShader::BeginRender(nTextureArray *)
{
    n_error("Pure virtual function called.\n");
    return 0;
}

//-------------------------------------------------------------------
/**
    @brief Render the pixel shader display list, and the "other"
    render states.

    - 21-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nPixelShader::Render(int)
{
    n_error("Pure virtual function called.\n");
}

//-------------------------------------------------------------------
/**
    @brief  Finish rendering the pixel shader.

    - 28-Aug-00   floh    created
*/
//-------------------------------------------------------------------
void nPixelShader::EndRender(void)
{
    n_error("Pure virtual function called.\n");
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
