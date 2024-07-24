#ifndef N_COLLIDESERVER_H
#define N_COLLIDESERVER_H
//------------------------------------------------------------------------------
/**
	@class nCollideServer
    @ingroup NebulaCollideModule
	@brief central collision system server

	The nCollideServer object serves as factory object of the
	different classes of the collision system, namely
	nCollideContext and nCollideShape. An nCollideContext
	serves as factory for nCollideObjects.
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_HASHLIST_H
#include "util/nhashlist.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_COLLIDECONTEXT_H
#include "collide/ncollidecontext.h"
#endif

#ifndef N_COLLIDESHAPE_H
#include "collide/ncollideshape.h"
#endif

#ifndef N_STRNODE_H
#include "util/nstrnode.h"
#endif

#ifndef N_STRLIST_H
#include "util/nstrlist.h"
#endif

#ifndef N_COLLTYPE_H
#include "collide/ncolltype.h"
#endif

//--------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nCollideServer
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
typedef int nCollClass;
class nCollClassNode : public nStrNode 
{
    nCollClass coll_class;

public:
    nCollClassNode(const char *n, nCollClass cl)
        : nStrNode(n),
          coll_class(cl)
    { };
    nCollClass GetCollClass(void) {
        return this->coll_class;
    };
};

//--------------------------------------------------------------------
class nGfxServer;
class N_PUBLIC nCollideServer : public nRoot {
protected:
    int unique_id;
    nList context_list;
    nHashList shape_list;
    nCollideContext *default_context;

    bool in_begin_collclasses;
    bool in_begin_colltypes;
    int num_coll_classes;
    nStrList collclass_list;
    nCollType *colltype_table;  

public:
    static nKernelServer *kernelServer;

    nCollideServer();
    virtual ~nCollideServer();

    virtual nCollideContext *NewContext(void);
    virtual nCollideShape   *NewShape(const char *id);
    virtual void ReleaseContext(nCollideContext *);
    virtual void ReleaseShape(nCollideShape *);

    virtual nCollideContext *GetDefaultContext(void);

    // define collision classes and collision check relationships
    void BeginCollClasses(void);
    void AddCollClass(const char *);
    void EndCollClasses(void);

    void BeginCollTypes(void);
    void AddCollType(const char *, const char *, nCollType);
    void EndCollTypes(void);

    virtual nCollClass QueryCollClass(const char *);
    virtual nCollType QueryCollType(const char *, const char *);

    nCollType QueryCollType(nCollClass cc1, nCollClass cc2)
    {
        
        // check for CollClass override cases
        if ((cc1 == COLLCLASS_ALWAYS_IGNORE) || (cc2 == COLLCLASS_ALWAYS_IGNORE))
        {
            return COLLTYPE_IGNORE;
        }
        else if ((cc1 == COLLCLASS_ALWAYS_QUICK) || (cc2 == COLLCLASS_ALWAYS_QUICK))
        {
            return COLLTYPE_QUICK;
        }
        else if ((cc1 == COLLCLASS_ALWAYS_CONTACT) || (cc2 == COLLCLASS_ALWAYS_CONTACT))
        {
            return COLLTYPE_CONTACT;
        }
        else if ((cc1 == COLLCLASS_ALWAYS_EXACT) || (cc2 == COLLCLASS_ALWAYS_EXACT))
        {
            return COLLTYPE_EXACT;
        }
        n_assert(this->colltype_table);
        n_assert((cc1 >= 0) && (cc2 >= 0));
        n_assert(int(cc1) < this->num_coll_classes);
        n_assert(int(cc2) < this->num_coll_classes);

        int index = (int(cc1)*this->num_coll_classes) + int(cc2);
        return this->colltype_table[index];
    };

protected:
    char *getResourceID(const char *, char *, int);
};
//--------------------------------------------------------------------
#endif
