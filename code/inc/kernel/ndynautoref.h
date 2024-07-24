#ifndef N_DYNAUTOREF_H
#define N_DYNAUTOREF_H

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

/**
    @class nDynAutoRef
    @ingroup SmartPointers
    @brief nAutoRef with dynamically allocated name string.

    See nRef for a more verbose explanation.
*/
//------------------------------------------------------------------------------
template<class TYPE> class nDynAutoRef : public nAutoRef<TYPE> {
public:
    /// default constructor
    nDynAutoRef();
    /// constructor
    nDynAutoRef(nKernelServer *_ks, nRoot *owner);
    /// destructor
    ~nDynAutoRef();

    /// set path name
    void set(const char *name);
    /// path name assignemnt operatir
    void operator=(const char *name);
    /// object pointer assignment operator (leaves path name invalid)
    void operator=(TYPE* obj);
};

//------------------------------------------------------------------------------
/**
    22-Jan-01   floh    created
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>::nDynAutoRef() 
: nAutoRef<TYPE>()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    22-Jan-01   floh    created
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>::nDynAutoRef(nKernelServer *_ks, nRoot *owner)
: nAutoRef<TYPE>(_ks, owner)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    22-Jan-01   floh    created
*/
template<class TYPE>
inline
nDynAutoRef<TYPE>::~nDynAutoRef() 
{
    if (n) n_free((void *)n);
}

//------------------------------------------------------------------------------
/**
    Set path name to referenced object. The referenced object must not exist
    at the time set() is called. The path will be resolved when the nDynAutoRef
    is first accessed (with get() or the -> operator).

    22-Jan-01   floh    created
*/
template<class TYPE>
inline
void
nDynAutoRef<TYPE>::set(const char *name) 
{
    this->invalidate();
    if (n) n_free((void *)n);
    if (name) n = n_strdup(name);
    else      n = NULL;
}

//------------------------------------------------------------------------------
/**
    Path name assignment operator (same functionality as set).
  
    22-Jan-01   floh    created
*/
template<class TYPE>
inline
void 
nDynAutoRef<TYPE>::operator=(const char *name) 
{
    this->set(name);
}

//------------------------------------------------------------------------------
/**
    Sometimes its useful to use a nDynAutoRef object just like a nRef
    (assign an object pointer but no name). This assignment operator does
    just that. Note that there is no valid path name after this method
    returns.
*/
template<class TYPE>
inline
void 
nDynAutoRef<TYPE>::operator=(TYPE* obj) 
{
    this->invalidate();
    nRef<TYPE>::set(obj);
}

//-------------------------------------------------------------------
#endif
