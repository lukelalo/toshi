#ifndef N_HARDREFSERVER_H
#define N_HARDREFSERVER_H

#ifndef N_STRLIST_H
#include "util/nstrlist.h"
#endif

#undef N_DEFINES
#define N_DEFINES nHardRefServer
#include "kernel/ndefdllclass.h"

template<class TYPE> class nHardRef;
class nRoot;
class nKernelServer;

/**
	@class nHardRefServer
    @ingroup SmartPointers
	@brief Immediatly validates nHardRefs.

    Validate nHardRef objects as soon as target object is created.
*/
class N_PUBLIC nHardRefServer
{
public:
    /// default constructor.
    nHardRefServer();
    /// default destructor.
    ~nHardRefServer();
    
    /// Adds a nHardRef to internal list
    void RegisterHardRef(nKernelServer* kernelServer, nHardRef<nRoot>* hardRef);
    /// registers a potential target object
    void RegisterTargetObject(nRoot& targetObject);
    
private:
    nStrList   strList;
};

#endif
