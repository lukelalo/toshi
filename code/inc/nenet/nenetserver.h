#ifndef N_ENETSERVER_H
#define N_ENETSERVER_H
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
/**
    @class nENetServer
    @ingroup NENetContribModule
    @brief Manage ENet-related objects.

    (C) 2003 Bruce Mitchener, Jr.
*/
#include <enet/enet.h>

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

class nENetHost;

#undef N_DEFINES
#define N_DEFINES nENetServer
#include "kernel/ndefdllclass.h"


//------------------------------------------------------------------------------
class N_PUBLIC nENetServer : public nRoot
{
public:
    /// constructor
    nENetServer();
    /// destructor
    ~nENetServer();

    /// Create a new host with the name in the NOH
    nENetHost * CreateHost(const char * name);

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;
private:

    bool initialized;
};
#endif /* N_ENETSERVER_H */
