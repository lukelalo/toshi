#define N_IMPLEMENTS nRemoteServer
#define N_KERNEL
//--------------------------------------------------------------------
//  IMPLEMENTATION
//      Cheap hack to try to get remote server in the original
//      Build system
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nremoteserver.h"

nClass *nRemoteServer::local_cl  = NULL;
nKernelServer *nRemoteServer::kernelServer = NULL;
extern char *nkernel_version;

extern char *nkernel_version;
extern "C" bool N_EXPORT nremoteserver_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT nremoteserver_fini(void);
extern "C" N_EXPORT void *nremoteserver_create(void);
extern "C" N_EXPORT char *nremoteserver_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT nremoteserver_init(nClass *cl, nKernelServer *ks)
{
    nRemoteServer::local_cl = cl;
    nRemoteServer::kernelServer = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nRemoteServer::local_cl);
    else n_error("Could not open superclass nroot!");

    return TRUE;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT nremoteserver_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nRemoteServer::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nRemoteServer::local_cl);
        nRemoteServer::kernelServer->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_create()
//--------------------------------------------------------------------
N_EXPORT void *nremoteserver_create(void)
{
    return n_new nRemoteServer;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *nremoteserver_version(void)
{
    return nkernel_version;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------
