#define N_IMPLEMENTS nPRender
//-------------------------------------------------------------------
//  nprender_main.cc
//  (C) 2000 A.Weissflog
//-------------------------------------------------------------------
#include "particle/npemitter.h"
#include "particle/nprender.h"
#include "gfx/ngfxtypes.h"

nNebulaScriptClass(nPRender, "nanimnode");

//-------------------------------------------------------------------
//  ~nPRender()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
nPRender::~nPRender()
{
    if (this->keys) n_free(this->keys);
}

//-------------------------------------------------------------------
//  BeginKeys()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
void nPRender::BeginKeys(int num)
{
    if (this->keys) {
        n_free(this->keys);
        this->keys = NULL;
    }
    this->i_num_keys = num;
    this->f_num_keys = (float) num;
    int size = num * sizeof(nPAttrs);
    this->keys = (nPAttrs *) n_malloc(size);
    memset(this->keys,0,size);
}

//-------------------------------------------------------------------
//  EndKeys()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
void nPRender::EndKeys(void)
{
    n_assert(this->keys);
}

//-------------------------------------------------------------------
//  SetKey()
//  20-Mar-00   floh    created
//  18-Oct-00   floh    + convert color to ulong in rgba and bgra
//                      format
//-------------------------------------------------------------------
void nPRender::SetKey(int i, float size, float bounce, float r, float g, float b, float a)
{
    n_assert(i<this->i_num_keys);
    n_assert(this->keys);
    this->keys[i].f[nPAttrs::N_SIZE]   = size;
    this->keys[i].f[nPAttrs::N_BOUNCE] = bounce;
    this->keys[i].f[nPAttrs::N_RED]    = r;
    this->keys[i].f[nPAttrs::N_GREEN]  = g;
    this->keys[i].f[nPAttrs::N_BLUE]   = b;
    this->keys[i].f[nPAttrs::N_ALPHA]  = a;
    this->keys[i].rgba = n_f2rgba(r,g,b,a);
    this->keys[i].bgra = n_f2bgra(r,g,b,a);
}

//-------------------------------------------------------------------
//  GetNumKeys()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
int nPRender::GetNumKeys(void)
{
    return this->i_num_keys;
}

//-------------------------------------------------------------------
//  GetKey()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
void nPRender::GetKey(int i, float& size, float& bounce, float& r, float& g, float& b, float& a)
{
    n_assert(i < this->i_num_keys);
    n_assert(this->keys);
    size   = this->keys[i].f[nPAttrs::N_SIZE];
    bounce = this->keys[i].f[nPAttrs::N_BOUNCE];
    r = this->keys[i].f[nPAttrs::N_RED];
    g = this->keys[i].f[nPAttrs::N_GREEN];
    b = this->keys[i].f[nPAttrs::N_BLUE];
    a = this->keys[i].f[nPAttrs::N_ALPHA];
}

//-------------------------------------------------------------------
//  SetEmitter()
//  20-Mar-00   floh    created
//  28-Sep-00   floh    + PushCwd()/PopCwd()
//-------------------------------------------------------------------
void nPRender::SetEmitter(const char *n)
{
    n_assert(n);
    kernelServer->PushCwd(this);
    this->ref_emitter = (nPEmitter *) kernelServer->Lookup(n);
    
    // exists?
    if (!this->ref_emitter.isvalid()) {
        n_error("Could not find npemitter object '%s'\n",n);
    }

    // of class nPEmitter?
    if (!this->ref_emitter->IsA(kernelServer->FindClass("npemitter"))) {
        n_error("Emitter object '%s' not of class 'npemitter'\n",n);
    }

    kernelServer->PopCwd();
}

//-------------------------------------------------------------------
//  GetEmitter()
//  20-Mar-00   floh    created
//-------------------------------------------------------------------
char *nPRender::GetEmitter(char *buf, int buf_size)
{
    if (this->ref_emitter.isvalid()) {
        this->GetRelPath((nRoot *)this->ref_emitter.get(),buf,buf_size);
        return buf;
    } else {
        return NULL;
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
