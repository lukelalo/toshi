#ifndef N_ANIMSERVER_H
#define N_ANIMSERVER_H
//------------------------------------------------------------------------------
/**
    @class nAnimServer
    
    @brief The anim server manages shared animation resource objects.

    (C) 2002 RadonLabs GmbH
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nAnimServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nAnimCurveArray;
class nAnimServer : public nRoot
{
public:
    /// constructor
    nAnimServer();
    /// destructor
    virtual ~nAnimServer();
    /// find an anim curve array by its resource name
    nAnimCurveArray* FindAnimCurveArray(const char* rsrcId);
    /// create a shared anim curve array object
    nAnimCurveArray* NewAnimCurveArray(const char* rsrcId);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
    /// create a valid resource id from a path string
    const char* GetResourceId(const char* rsrcId, char* buf, int bufSize);

    nRef<nRoot> refCurveArrays;
};
//------------------------------------------------------------------------------
#endif
