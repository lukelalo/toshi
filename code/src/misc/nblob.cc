#define N_IMPLEMENTS nBlob
//--------------------------------------------------------------------
//  IMPLEMENTATION
//  nblob.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nblob.h"

nClass *nBlob::local_cl  = NULL;
nKernelServer *nBlob::ks = NULL;

extern char *nBlob_version;
extern "C" bool N_EXPORT n_init(nClass *, nKernelServer *);
extern "C" void N_EXPORT n_fini(void);
extern "C" N_EXPORT void *n_create(void);
extern "C" N_EXPORT char *n_version(void);

//--------------------------------------------------------------------
//  n_init()
//--------------------------------------------------------------------
bool N_EXPORT n_init(nClass *cl, nKernelServer *ks)
{
    nBlob::local_cl = cl;
    nBlob::ks       = ks;
    
    // mit Superklasse verketten
    nClass *super = ks->OpenClass("nroot");
    if (super) super->AddSubClass(nBlob::local_cl);
    else n_error("Could not open superclass nroot!");
    return true;
}

//--------------------------------------------------------------------
//  n_fini()
//--------------------------------------------------------------------
void N_EXPORT n_fini(void)
{
    // von Superklasse abkoppeln
    nClass *super = nBlob::local_cl->GetSuperClass();
    n_assert(super);
    if (super) {
        super->RemSubClass(nBlob::local_cl);
        nBlob::ks->CloseClass(super);
    }
}

//--------------------------------------------------------------------
//  n_new()
//--------------------------------------------------------------------
N_EXPORT void *n_create(void)
{
    return new nBlob;
}

//--------------------------------------------------------------------
//  n_version()
//--------------------------------------------------------------------
N_EXPORT char *n_version(void)
{
    return nBlob_version;
}

//--------------------------------------------------------------------
/**
    - 07-Feb-99   floh    created
*/
//--------------------------------------------------------------------
nBlob::nBlob()
{
    this->blob = NULL;
    this->size = 0;
}

//--------------------------------------------------------------------
/**
    - 07-Feb-99   floh    created
*/
//--------------------------------------------------------------------
nBlob::~nBlob()
{
    if (this->blob) n_free(this->blob);
}

//--------------------------------------------------------------------
/**
    @brief Copy the data from the address given.

    - 07-Feb-99   floh    created
    - 15-May-00   floh    + bugfix: if null pointer blob came in,
                            previous blob pointer has not been 
                            set to NULL
*/
//--------------------------------------------------------------------
bool nBlob::Set(void *b, int s)
{
    bool retval = true;
    if (this->blob) {
        n_free(this->blob);
        this->blob = NULL;
    }
    if (b) {
        this->blob = n_malloc(s);
        if (this->blob) memcpy(this->blob,b,s);
        else            retval = false;
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    @brief Return the pointer to the stored data.
    
    - 07-Feb-99   floh    created
*/
//--------------------------------------------------------------------
void *nBlob::Get(void)
{
    return this->blob;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

