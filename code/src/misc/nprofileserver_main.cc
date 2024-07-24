#define N_IMPLEMENTS nProfileServer
//------------------------------------------------------------------------------
//  nprofileserver_main.cc
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "misc/nprofileserver.h"

//------------------------------------------------------------------------------
/**
*/
nProfileServer::nProfileServer() :
    isProfiling(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nProfileServer::~nProfileServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nProfileServer::Start()
{
    n_assert(!this->isProfiling);
    n_printf("*** START PROFILING\n");
    this->isProfiling = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nProfileServer::Stop()
{
    n_assert(this->isProfiling);
    n_printf("*** STOP PROFILING\n");
    this->isProfiling = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nProfileServer::IsProfiling()
{
    return this->isProfiling;
}

//------------------------------------------------------------------------------
/**
*/
void
nProfileServer::Toggle()
{
    if (this->isProfiling)
    {
        this->Stop();
    }
    else
    {
        this->Start();
    }
}

//------------------------------------------------------------------------------
