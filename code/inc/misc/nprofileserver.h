#ifndef N_PROFILESERVER_H
#define N_PROFILESERVER_H
//------------------------------------------------------------------------------
/**
    @class nProfileServer

    @brief Aids in collecting profiling data. Can be subclassed for specific
    profilers.

    (C) 2001 RadonLabs GmbH
*/

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nProfileServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nProfileServer : public nRoot
{
public:
    /// default constructor.
    nProfileServer();
    /// default destructor.
    virtual ~nProfileServer();

    /// start profiling
    virtual void Start();
    /// stop profiling
    virtual void Stop();
    /// toggle profiling
    void Toggle();
    /// get current profiling state
    bool IsProfiling();

    static nClass* clazz;
    static nKernelServer* kernelServer;

private:
    bool isProfiling;
};
//------------------------------------------------------------------------------
#endif
