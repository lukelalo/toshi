#ifndef N_REF_H
#define N_REF_H
//-------------------------------------------------------------------
/**
  @defgroup SmartPointers Smart Pointers
  @ingroup NebulaDataTypes
*/
/** 
  @class nRef
  @ingroup SmartPointers
  @brief nRef, nAutoRef, and nDynAutoRef implement safe pointers to objects.
    
  If one object keeps a pointer to another object,
  and the pointed-to object goes away, the first object is
  left with an invalid object pointer. An nRef creates
  a wire between the 2 objects, if the referenced object goes
  away, the nRef will be invalidated and the object
  holding the reference will be notified.

  When creating a nRef you need to specify the type of object it
  will be referencing.

  @code
      nRef<nVertexBuffer> ref_vb;
  @endcode

  Here a nRef is checked for validity and assigned to as if it
  were a pointer.

  @code
      if (!ref_vb.isvalid()) {
          ref_vb = meshLoader.ObtainVertexBuffer();
      }
  @endcode

  A nRef can be dereferenced just like a pointer.

  @code
      ref_vb->LockVertices();
  @endcode

  Use nRef::get() to get a pointer to the object

  @code
      nVertexBuffer* vb = ref_vb.get();
  @endcode

  A nRef can have an optional nRoot derived owner that will be notified,
  if the nRef becomes invalid.

  @code
      nRef<nVertexBuffer> ref_vb(this);
  @endcode

  nAutoRef builds on nRef and adds the ability to revalidate itself by
  looking up the referenced object by its path name. It is up to the nAutoRef
  holder to ensure that the path name string stays in scope.

  @code
      // nAutoRef needs access to the kernel server
      // in order to find objects by path name
      nAutoRef<nGfxServer>  ref_gs(kernelServer, this);

      ref_gs = "/sys/servers/gfx";
  @endcode

  An nDynAutoRef is the same as an nAutoRef except that it
  maintains its own copy of the name string.

  Technically, it's a class template implementing a smart pointer.

  See also nHardRef & nHardRefServer for more information.

  (C) 1999 A.Weissflog

    
  - 29-Sep-00   floh    + nAutoRef can now handle paths relative
                          to owner object
                        + generally enabled default constructor
                          and added initialize() method (simplifies
                          putting nRefs into arrays)
  - 30-Aug-00   floh    removed the requirement to provide an owner
                        object
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

template<class TYPE> class nRef : public nNode {
public:
    nRef();
    nRef(nRoot *owner);
    ~nRef();

    void initialize(nRoot *owner);
    nRoot *getowner(void) const;
    void invalidate(void);
    void set(TYPE *obj);
    TYPE *get(void) const;
    bool isvalid(void) const;
    TYPE *operator->();
    void operator=(TYPE *obj);

protected:
    nRoot *o;
};

/**
  @brief Standard constructor

  Sets nRoot reference 'o' to NULL

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE> 
inline
nRef<TYPE>::nRef()
{
    o = NULL;
}

/**
  @brief Standard constructor with owner parameter
  @param owner The owner of this nRef.

  Sets nNode data pointer to 'owner'

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
nRef<TYPE>::nRef(nRoot *owner)
{
    SetPtr(owner);
    o = NULL;
}

/**
  @brief Simple class destructor

  Removes the references to this object
  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
nRef<TYPE>::~nRef()
{
    if (o) o->RemObjectRef((nRef<nRoot> *)this);
}

/**
  @brief Initializes the the reference.
  @param owner The owner of this nRef.

  Sets nNode data pointer to 'owner'

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
void
nRef<TYPE>::initialize(nRoot *owner)
{
    SetPtr(owner);
}

/**
  @brief Returns the owner of this refernce as a nRoot pointer

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
nRoot*
nRef<TYPE>::getowner(void) const
{
    return (nRoot *) GetPtr();
}

/**
  @brief Invalidates the nRef object

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
void 
nRef<TYPE>::invalidate(void)
{
    if (o) 
    {
        o->RemObjectRef((nRef<nRoot> *)this);
    }
    o = NULL;
}

/**

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
void
nRef<TYPE>::set(TYPE* obj)
{
    this->invalidate();
    o = (nRoot *) obj;
    if (o) o->AddObjectRef((nRef<nRoot> *)this);
}

/**

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
TYPE*
nRef<TYPE>::get(void) const
{
    if (!o) {
        char buf[N_MAXPATH];
        nRoot *owner = (nRoot *) this->GetPtr();
        n_error("Null pointer access from: '%s'\n",
                  owner ? owner->GetFullName(buf,sizeof(buf)) : "UNKNOWN");
    }
    return (TYPE *) o;
}

/**
  @brief Checks for validity of this reference

  Checks if the member 'o' is not null and returns false if so, 
  otherwise returns true (object is valid)

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
bool
nRef<TYPE>::isvalid(void) const
{
    return o ? true : false;
}

/**

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
TYPE*
nRef<TYPE>::operator->()
{
    return get();
}

/*

  - 22-Jan-01   floh    created
*/
//-------------------------------------------------------------------
template<class TYPE>
inline
void 
nRef<TYPE>::operator=(TYPE *obj)
{
    set(obj);
}

//-------------------------------------------------------------------
#endif
