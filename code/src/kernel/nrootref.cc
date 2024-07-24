#define N_IMPLEMENTS nRoot
#define N_KERNEL
//-------------------------------------------------------------------
//  nrootref.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/nref.h"

//-------------------------------------------------------------------
/**
    @brief Add a reference to this object.

    - 15-Jul-99   floh    created
    - 30-Aug-00   floh    + removed call to nRoot::ValidRef()
*/
//-------------------------------------------------------------------
void nRoot::AddObjectRef(nRef<nRoot> *r)
{
    this->ref_list.AddTail((nNode *)r);
}

//-------------------------------------------------------------------
/**
    @brief A reference to this object has become invalid.

    - 15-Jul-99   floh    created
    - 30-Aug-00   floh    + removed call to nRoot::InvalidRef()
*/
//-------------------------------------------------------------------
void nRoot::RemObjectRef(nRef<nRoot> *r)
{
    r->Remove();
}

//-------------------------------------------------------------------
/**
    @brief Invalidate all references to this object.

    This is done so that the object may safely die and is usually
    done when the object has gone away after Release() was called.

    - 15-Jul-99   floh    created
*/
//-------------------------------------------------------------------
void nRoot::InvalidateAllRefs(void)
{
    nRef<nRoot> *r;
    while ((r = (nRef<nRoot> *) this->ref_list.GetHead())) {
        r->invalidate();
    }
}

//-------------------------------------------------------------------
/**
    - 15-Jul-99   floh    created
*/
//-------------------------------------------------------------------
nList *nRoot::GetRefs(void)
{
    return &(this->ref_list);
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

