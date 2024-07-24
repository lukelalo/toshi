#ifndef N_ROOT_H
#define N_ROOT_H
/*!
  \file
*/
//--------------------------------------------------------------------
/** 
  @class nRoot
  @ingroup NebulaObjectSystem
  @brief Root class for Nebula classes
    
  The nRoot class is the superclass of all higher level Nebula C++ classes.

  It defines the following basic mechanisms:

   - linkage into the hierarchical name space (See @ref NebulaObjectSystem.)
   - a reference tracking mechanism that implements safe
     pointers to other C++ objects (See @ref SmartPointers.)
   - runtime type information, objects can be asked whether they
     belong to or are inherited from a specific class
   - object persistency, objects can save their current state into
     a command stream and can restore their state from such a
     command stream (the command stream can go into a file,
     directly to another object, or over a network, this depends on
     the fileserver object that handles the stream) (See nFileServer2.)
   - a scripting interface in the form of a single Dispatch() method
     that receives incoming commands and translates them into a
     native C++ method call, care has been taken to minimize the
     required number of lines per supported script command (ca. 3..6)

  Rules for subclasses:
   - only the default constructor is allowed
   - never create or destroy nRoot objects directly,
     use nKernelServer::New() to create an object and
     the objects Release() method to destroy it
        
  (C) 1999 A.Weissflog
*/
//--------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHLIST_H
#include "util/nhashlist.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#ifndef N_CMD_H
#include "kernel/ncmd.h"
#endif

#undef N_DEFINES
#define N_DEFINES nRoot
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nCmd;
class nClass;
class nKernelServer;
class nPersistServer;
template<class TYPE> class nRef;

class N_PUBLIC nRoot : public nHashNode 
{
public:
    nRoot(void);
    virtual ~nRoot(void);

    /**
        @name Safe Object Referencing

        See nRef and the other @ref SmartPointers for further
        information.
    */
    //@{
    void AddObjectRef(nRef<nRoot> *);
    void RemObjectRef(nRef<nRoot> *);
    nList *GetRefs(void);
    void InvalidateAllRefs(void);
    //@}

    /**
         @name Initialize/Cleanup
    */
    //@{
    virtual void Initialize(void);
    virtual bool Release(void);
    long AddRef(void);
    long GetRefCount(void);
    void SetFlags(int);
    void UnsetFlags(int);
    int  GetFlags(void);
    //@}
      
    /**
        @name RTTI

        The @ref NebulaObjectSystem provides run-time type information
        through these methods.
    */
    //@{
    virtual void SetClass(nClass *);
    virtual nClass *GetClass(void);
    virtual bool IsA(nClass *);
    virtual bool IsInstanceOf(nClass *); 
    //@}
            
    /**
        @name Serialization
    */
    //@{
    virtual bool Save(void);
    virtual bool SaveAs(const char *name);
    virtual nRoot *Clone(const char *name);
    virtual bool Parse(const char *name);
    virtual bool SaveCmds(nPersistServer *fs);
    virtual bool Import(const char *name);
    //@}

    /**
        @name Scripting
    */
    //@{
    virtual bool Dispatch(nCmd *);
    void GetCmdProtos(nHashList *);
    //@}

    /**
        @name Named Object Hierarchy

        All instances of classes derived from nRoot are stored in a
        hierarchy.  This hierarchy is where nearly all of the data
        used by the Nebula Device as well as references to objects
        can be found.

        See http://nebuladevice.sourceforge.net/cgi-bin/twiki/view/Nebula/NamedObjectHierarchy for more details.
    */
    //@{
    void SetName(const char *str);
    char *GetFullName(char *buf, long sizeof_buf);
    char *GetRelPath(nRoot *other, char *buf, long sizeof_buf);
    nRoot *Find(const char *str);
    void AddHead(nRoot *n);
    void AddTail(nRoot *n);
    nRoot *RemHead(void);
    nRoot *RemTail(void);
    void Remove(void);
    void Sort(void);

    // inline for speed
    nRoot *GetParent(void);
    nRoot *GetHead(void);
    nRoot *GetTail(void);
    nRoot *GetSucc(void);
    nRoot *GetPred(void);
    const char *GetName(void) const;
    //@}

	/// pointer to instance class 
    static nClass *local_cl;
    /// pointer to kernel server
    static nKernelServer *ks;   

protected:
	/// list of references pointing to me
    nList ref_list;
	/// pointer to instance class object
    nClass *instance_cl;
	/// parent object in hierarchical name space
    nRoot  *parent;
	/// list of child objects in hierarchical name space
    nHashList *child_list;      
    ushort ref_count;
    ushort root_flags;
	/// nRoot::Import() stores filename here...
    char *import_file;          
    enum {
        N_FLAG_SAVEUPSIDEDOWN = (1<<0),     // save children first, then own state
        N_FLAG_SAVESHALLOW    = (1<<1),     // do not save child objects!
    };
};

/**
  @brief Returns the parent object in hierarchical namespace

  - 22-Jan-01   floh    created
*/
//--------------------------------------------------------------------
inline
nRoot*
nRoot::GetParent(void)
{
    return this->parent;
}

/**
  @brief Returns the first child object in hierarchical namespace

  - 22-Jan-01   floh    created
*/
//--------------------------------------------------------------------
inline
nRoot*
nRoot::GetHead()
{
    if (this->child_list) 
    {
        return (nRoot *) this->child_list->GetHead();
    }
    else
    {
        return NULL;
    }
}

/**
  @brief Returns the last child object in hierarchical namespace

  - 22-Jan-01   floh    created
*/
//--------------------------------------------------------------------
inline
nRoot*
nRoot::GetTail()
{
    if (this->child_list) {
        return (nRoot *) this->child_list->GetTail();
    } 
    else 
    {
        return NULL;
    }
}

/**
  @brief Returns the succeeding object in hierarchical namespace
  
  - 22-Jan-01   floh    created
*/
//--------------------------------------------------------------------
inline
nRoot*
nRoot::GetSucc()
{
    return (nRoot *) nHashNode::GetSucc();
}

/**
  @brief Returns the preceding object in hierarchical namespace
  
  - 22-Jan-01   floh    created
*/
//--------------------------------------------------------------------
inline
nRoot*
nRoot::GetPred()
{
    return (nRoot *) nHashNode::GetPred();
}

/**
  @brief Returns the name of the object in hierarchical namespace
  
  - 22-Jan-01   floh    created
*/
//--------------------------------------------------------------------
inline
const char*
nRoot::GetName(void) const
{
    return nHashNode::GetName();
}
//--------------------------------------------------------------------
#endif
