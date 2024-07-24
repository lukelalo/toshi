#define N_IMPLEMENTS nShaderNode
//-------------------------------------------------------------------
//  nshadernode_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "node/nshadernode.h"
#include "gfx/ngfxserver.h"
#include "gfx/npixelshader.h"
#include "gfx/nscenegraph2.h"

nNebulaScriptClass(nShaderNode, "nvisnode");

//-------------------------------------------------------------------
//  ~nShaderNode()
//  21-Aug-00   floh    created
//-------------------------------------------------------------------
nShaderNode::~nShaderNode()
{
    if (this->ref_ps.isvalid()) {
        this->ref_ps->Release();
        this->ref_ps.invalidate();
    }
}

//-------------------------------------------------------------------
//  SetColorOp()
//  25-Aug-00   floh    created
//-------------------------------------------------------------------
bool nShaderNode::SetColorOp(int stage, const char *str)
{
    n_assert(str);

    char buf[N_MAXNAMELEN];
    n_strncpy2(buf,str,sizeof(buf));

    // get the opcode
    nPSI::nOp op = nPSI::NOP;
    nPSI::nScale scale = nPSI::ONE;
    nPSI::nArg arg0 = nPSI::NOARG;
    nPSI::nArg arg1 = nPSI::NOARG;
    nPSI::nArg arg2 = nPSI::NOARG;
    char *op_str = strtok(buf," ");
    n_assert(op_str);
    char *arg_str;

    // translate it and get number of expected args
    int num_args = this->ps_desc.Str2Op(op_str,op,scale);
    if (num_args == 0) {
        n_error("nShaderNode::SetColorOp(): opcode '%s' not accepted.\n",op_str);
    }
    if (num_args > 0) {
        arg_str = strtok(NULL," ");
        if (!this->ps_desc.Str2Arg(arg_str,arg0)) {
            n_error("nShaderNode::SetColorOp(): invalid arg0 string '%s' on stage '%d'.\n",arg_str,stage);
        }
    }
    if (num_args > 1) {
        arg_str = strtok(NULL," ");
        if (!this->ps_desc.Str2Arg(arg_str,arg1)) {
            n_error("nShaderNode::SetColorOp(): invalid arg1 string '%s' on stage '%d'.\n",arg_str,stage);
        }
    }
    if (num_args > 2) {
        arg_str = strtok(NULL," ");
        if (!this->ps_desc.Str2Arg(arg_str,arg2)) {
            n_error("nShaderNode::SetColorOp(): invalid arg2 string '%s' on stage '%d'.\n",arg_str,stage);
        }
    }
    this->ps_desc.SetColorOp(stage,op,arg0,arg1,arg2,scale);

    return true;
}

//-------------------------------------------------------------------
//  SetAlphaOp()
//  25-Aug-00   floh    created
//-------------------------------------------------------------------
bool nShaderNode::SetAlphaOp(int stage, const char *str)
{
    n_assert(str);

    char buf[N_MAXNAMELEN];
    n_strncpy2(buf,str,sizeof(buf));

    // get the opcode
    nPSI::nOp op = nPSI::NOP;
    nPSI::nScale scale = nPSI::ONE;
    nPSI::nArg arg0 = nPSI::NOARG;
    nPSI::nArg arg1 = nPSI::NOARG;
    nPSI::nArg arg2 = nPSI::NOARG;
    char *op_str = strtok(buf," ");
    n_assert(op_str);
    char *arg_str;

    // translate it and get number of expected args
    int num_args = this->ps_desc.Str2Op(op_str,op,scale);
    if (num_args > 0) {
        arg_str = strtok(NULL," ");
        if (!this->ps_desc.Str2Arg(arg_str,arg0)) {
            n_error("nShaderNode::SetAlphaOp(): invalid arg0 string '%s' on stage '%d'.\n",arg_str,stage);
        }
    }
    if (num_args > 1) {
        arg_str = strtok(NULL," ");
        if (!this->ps_desc.Str2Arg(arg_str,arg1)) {
            n_error("nShaderNode::SetAlphaOp(): invalid arg1 string '%s' on stage '%d'.\n",arg_str,stage);
        }
    }
    if (num_args > 2) {
        arg_str = strtok(NULL," ");
        if (!this->ps_desc.Str2Arg(arg_str,arg2)) {
            n_error("nShaderNode::SetAlphaOp(): invalid arg2 string '%s' on stage '%d'.\n",arg_str,stage);
        }
    }
    this->ps_desc.SetAlphaOp(stage,op,arg0,arg1,arg2,scale);

    return true;
}

//-------------------------------------------------------------------
//  GetColorOp()
//  22-Aug-00   floh    created
//-------------------------------------------------------------------
void nShaderNode::GetColorOp(int stage, char *buf, int /*buf_size*/) 
{
    nPSI::nOp op;
    nPSI::nScale scale;
    nPSI::nArg arg0,arg1,arg2;
    
    char tmp_buf[N_MAXNAMELEN];

    this->ps_desc.GetColorOp(stage,op,arg0,arg1,arg2,scale);
    this->ps_desc.Op2Str(op,scale,tmp_buf,sizeof(tmp_buf));
    strcpy(buf,tmp_buf);
    
    if (arg0 != nPSI::NOARG) {
        this->ps_desc.Arg2Str(arg0,tmp_buf,sizeof(tmp_buf));
        strcat(buf," ");
        strcat(buf,tmp_buf);
    }
    if (arg1 != nPSI::NOARG) {
        this->ps_desc.Arg2Str(arg1,tmp_buf,sizeof(tmp_buf));
        strcat(buf," ");
        strcat(buf,tmp_buf);
    }
    if (arg2 != nPSI::NOARG) {
        this->ps_desc.Arg2Str(arg2,tmp_buf,sizeof(tmp_buf));
        strcat(buf," ");
        strcat(buf,tmp_buf);
    }
}

//-------------------------------------------------------------------
//  GetAlphaOp()
//  22-Aug-00   floh    created
//-------------------------------------------------------------------
void nShaderNode::GetAlphaOp(int stage, char *buf, int /*buf_size*/) 
{
    nPSI::nOp op;
    nPSI::nScale scale;
    nPSI::nArg arg0,arg1,arg2;
    
    char tmp_buf[N_MAXNAMELEN];

    this->ps_desc.GetAlphaOp(stage,op,arg0,arg1,arg2,scale);
    this->ps_desc.Op2Str(op,scale,tmp_buf,sizeof(tmp_buf));
    strcpy(buf,tmp_buf);
    
    if (arg0 != nPSI::NOARG) {
        this->ps_desc.Arg2Str(arg0,tmp_buf,sizeof(tmp_buf));
        strcat(buf," ");
        strcat(buf,tmp_buf);
    }
    if (arg1 != nPSI::NOARG) {
        this->ps_desc.Arg2Str(arg1,tmp_buf,sizeof(tmp_buf));
        strcat(buf," ");
        strcat(buf,tmp_buf);
    }
    if (arg2 != nPSI::NOARG) {
        this->ps_desc.Arg2Str(arg2,tmp_buf,sizeof(tmp_buf));
        strcat(buf," ");
        strcat(buf,tmp_buf);
    }
}

//-------------------------------------------------------------------
//  init_pixelshader()
//  05-Oct-00   floh    created
//-------------------------------------------------------------------
void 
nShaderNode::init_pixelshader(void)
{
    n_assert(!this->ref_ps.isvalid());
    nPixelShader *ps = this->refGfx->NewPixelShader(NULL);
    if (ps) {
        this->ref_ps = ps;
        ps->SetShaderDesc(&(this->ps_desc));
    }
}

//-------------------------------------------------------------------
//  Preload()
//  28-Sep-01   floh    created
//-------------------------------------------------------------------
void
nShaderNode::Preload()
{
    if (!this->ref_ps.isvalid()) 
    {
        this->init_pixelshader();
    }
    nVisNode::Preload();
}

//-------------------------------------------------------------------
//  Attach()
//  22-Aug-00   floh    created
//  10-Oct-00   floh    + added transparency and renderpri
//  31-May-01   floh    + new behaviour
//-------------------------------------------------------------------
bool 
nShaderNode::Attach(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);

    if (nVisNode::Attach(sceneGraph))
    {
        sceneGraph->AttachShaderNode(this);
        sceneGraph->AttachRenderPri(this->render_pri);
        sceneGraph->AttachOpaqueness(!this->GetAlphaEnable());
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  Compute()
//  22-Aug-00   floh    created
//  31-May-01   floh    new behaviour
//-------------------------------------------------------------------
void
nShaderNode::Compute(nSceneGraph2 *sceneGraph)
{
    n_assert(sceneGraph);
    nVisNode::Compute(sceneGraph);

    // initialize pixel shader on demand (pixel shaders may
    // become invalid during runtime, for instance when
    // switching gfx servers)
    if (!this->ref_ps.isvalid()) 
    {
        this->init_pixelshader();
    }

    // the actual rendering of the pixel shader will happen
    // inside the mesh rendering
    sceneGraph->SetPixelShader(this->ref_ps.get());
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

