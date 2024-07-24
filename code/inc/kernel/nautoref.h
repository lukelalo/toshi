#ifndef N_AUTOREF_H
#define N_AUTOREF_H
//-------------------------------------------------------------------
/**
  @class nAutoRef
  @ingroup SmartPointers
  @brief nRef with ability to resolve C++ pointer from object name

  See nRef for a more verbose explanation.
*/
#ifndef N_REF_H
#include "kernel/nref.h"
#endif

template<class TYPE> class nAutoRef : public nRef<TYPE> {
public:
	/// Simple class constructor
    nAutoRef();
	/// Class constructor
    nAutoRef(nKernelServer *_ks, nRoot *owner);

    /// initialize if default constructor is used
    void initialize(nKernelServer *_ks, nRoot *_owner);
    /// Sets the object name
	void set(const char *name);
    TYPE *get(void);
	/// Returns the object name
    const char *getname(void) const;
	/// Returns true if the object is valid
    bool isvalid(void);
	/// Reference operator
    TYPE *operator->();
	/// Assign operator
    void operator=(const char *name);

protected:
    TYPE *check(void);

	/// Pointer to the name of the objet
    const char *n;
	/// Pointer to the kernel server
    nKernelServer *ks;
};

//------------------------------------------------------------------------------
/**


  -  22-Jan-01   floh    created
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef()
: nRef<TYPE>() 
{
    ks = NULL;
    n  = NULL;
}

//------------------------------------------------------------------------------
/**

  
  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
nAutoRef<TYPE>::nAutoRef(nKernelServer *_ks, nRoot *owner)
: nRef<TYPE>(owner) 
{
    ks = _ks;
    n  = NULL;
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
void
nAutoRef<TYPE>::initialize(nKernelServer *_ks, nRoot *_owner)
{
    SetPtr(_owner);
    ks = _ks;
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
void
nAutoRef<TYPE>::set(const char *name)
{
    this->invalidate();
    n = name;
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
TYPE*
nAutoRef<TYPE>::get(void)
{
    if (!check()) {
        char ownerName[N_MAXPATH];
        nRoot *owner = (nRoot *) this->GetPtr();
        n_error("Null pointer access from: '%s' object '%s' not found.\n",
                  owner ? owner->GetFullName(ownerName, sizeof(ownerName)) : "UNKNOWN",
                  n ? n : "NOT INITIALIZED");
    }
    return (TYPE *) o;
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
const char*
nAutoRef<TYPE>::getname(void) const
{
    return n;
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
bool
nAutoRef<TYPE>::isvalid(void)
{
    return check() ? true : false;
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
TYPE*
nAutoRef<TYPE>::operator->()
{
    return get();
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
void
nAutoRef<TYPE>::operator=(const char *name)
{
    set(name);
}

//------------------------------------------------------------------------------
/**


  - 22-Jan-01   floh    created
*/
template<class TYPE>
inline
TYPE*
nAutoRef<TYPE>::check(void)
{
    if (!o) {
        n_assert(ks);
        if (!n) return NULL;
        nRoot *owner = (nRoot *) this->GetPtr();
        if (owner) ks->PushCwd(owner);
        o = ks->Lookup(n);
        if (owner) ks->PopCwd();
        if (o) o->AddObjectRef((nRef<nRoot> *)this);
    }
    return (TYPE *) o;
}    

//-------------------------------------------------------------------
#endif
