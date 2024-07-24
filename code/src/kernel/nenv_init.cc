#define N_IMPLEMENTS nEnv
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      nEnv -- Env-Variablen-Objekt fuer Nebula
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"

nClass *nEnv::local_cl  = NULL;
nKernelServer *nEnv::ks = NULL;

extern void nenv_initcmds(nClass *);
extern char *nkernel_version;
extern "C" bool N_EXPORT nenv_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT nenv_fini(void);
extern "C" N_EXPORT void *nenv_new(void);
extern "C" N_EXPORT char *nenv_version(void);

//--------------------------------------------------------------------
//  nenv_init()
//--------------------------------------------------------------------
bool N_EXPORT nenv_init(nClass *cl, nKernelServer *ks)
{
    nEnv::local_cl = cl;
    nEnv::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nEnv::local_cl);
    else n_error("Could not open superclass nroot");
    
    nenv_initcmds(cl);
    
    return TRUE;
}

//--------------------------------------------------------------------
//  nenv_fini()
//--------------------------------------------------------------------
void N_EXPORT nenv_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nEnv::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nEnv::local_cl);
        nEnv::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  nenv_new()
//--------------------------------------------------------------------
N_EXPORT void *nenv_new(void)
{
    return n_new nEnv;
}

//--------------------------------------------------------------------
//  nenv_version()
//--------------------------------------------------------------------
N_EXPORT char *nenv_version(void)
{
    return nkernel_version;
}

//--------------------------------------------------------------------
//  GetType()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
nArg::ArgType nEnv::GetType(void)
{
    return this->arg.GetType();
}

//--------------------------------------------------------------------
//  SetI()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
void nEnv::SetI(int i)
{
    this->arg.SetI(i);
}

//--------------------------------------------------------------------
//  SetB()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
void nEnv::SetB(bool b)
{
    this->arg.SetB(b);
}

//--------------------------------------------------------------------
//  SetF()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
void nEnv::SetF(float f)
{
    this->arg.SetF(f);
}

//--------------------------------------------------------------------
//  SetS()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
void nEnv::SetS(const char *s)
{
    this->arg.SetS(s);
}

//--------------------------------------------------------------------
//  SetO()
//  21-Jun-99   floh    created
//--------------------------------------------------------------------
void nEnv::SetO(nRoot *o)
{
    this->arg.SetO(o);
}

//--------------------------------------------------------------------
//  GetI()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
int nEnv::GetI(void)
{
    return this->arg.GetI();
}

//--------------------------------------------------------------------
//  GetB()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
bool nEnv::GetB(void)
{
    return this->arg.GetB();
}

//--------------------------------------------------------------------
//  GetF()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
float nEnv::GetF(void)
{
    return this->arg.GetF();
}

//--------------------------------------------------------------------
//  GetS()
//  16-Nov-98   floh    created
//--------------------------------------------------------------------
const char *nEnv::GetS(void)
{
    return this->arg.GetS();
}

//--------------------------------------------------------------------
//  GetO()
//  21-Jun-99   floh    created
//--------------------------------------------------------------------
nRoot *nEnv::GetO(void)
{
    return (nRoot *) this->arg.GetO();
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

