#define N_IMPLEMENTS nENetServer
//------------------------------------------------------------------------------
/* Copyright (c) 2003 Bruce Mitchener, Jr.
 *
 * See the file "nenet_license.txt" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//------------------------------------------------------------------------------
#include "nenet/nenetserver.h"

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

nNebulaScriptClass(nENetServer, "nroot")

//------------------------------------------------------------------------------
/**
*/
nENetServer::nENetServer() :
    initialized(false)
{
    int result;

    result = enet_initialize();

    n_assert(0 == result);

    if (0 == result)
    {
        this->initialized = true;
    }

    // Prepare host database
    nRoot * network = kernelServer->Lookup("/sys/share/network");
    if (NULL != network)
    {
        network->Release();
    }
    network = kernelServer->New("nroot", "/sys/share/network");
    nRoot * hosts = kernelServer->Lookup("/sys/share/network/hosts");
    if (NULL != hosts)
    {
        hosts->Release();
    }
    hosts = kernelServer->New("nroot", "/sys/share/network/hosts");
}

//------------------------------------------------------------------------------
/**
*/
nENetServer::~nENetServer()
{
    if (true == this->initialized)
    {
        enet_deinitialize();
    }
}

//------------------------------------------------------------------------------
/**
*/
nENetHost *
nENetServer::CreateHost(const char * name)
{
    char buf[N_MAXPATH];
    int res = _snprintf(buf, N_MAXPATH, "/sys/share/network/hosts/%s", name);

    if (N_MAXPATH == res)
    {
        n_warn("Name exceeded max name length");
        return NULL;
    }

    nENetHost * host = (nENetHost *)kernelServer->New("nenethost", buf);

    return host;
}

