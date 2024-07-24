#define N_IMPLEMENTS nFileLogHandler
#define N_KERNEL
//------------------------------------------------------------------------------
//  nfileloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nfileloghandler.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "util/npathstring.h"

//------------------------------------------------------------------------------
/**
    The constructor takes an application name which is used to name the
    log file.  It is expected that the kernel has been initialized at
    this point.

    @param  app     an application name
*/
nFileLogHandler::nFileLogHandler(const char* logname) :
    logFile(0)
{
    this->logName = logname;
	char buf[N_MAXPATH];
    // obtain app directory
    nPathString logFileName;
    //We assume we have a kernel server up at this point
    if (nKernelServer::ks)
    {
        nKernelServer::ks->GetFileServer2()->ManglePath("home:", buf, N_MAXPATH);
        logFileName.Append(buf);
        logFileName.Append("/");
    }
    logFileName.Append(logName.Get());
    logFileName.Append(".log");

    // low level file access functions since we cannot guarantee that
    // a file server exists!
    this->logFile = fopen(logFileName.Get(), "w");
    
    // to prevent a infinite loop on error, we cannot simply call n_assert(),
    // since this would in turn ourselves, so handle the error message
    // with our own weapons
    if (!this->logFile)
    {
        printf("nFileLogHandler: could not open log file!\n");
        exit(10);
    }
}

//------------------------------------------------------------------------------
/**
*/
nFileLogHandler::~nFileLogHandler()
{
    fclose(this->logFile);
    this->logFile = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nFileLogHandler::Print(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nFileLogHandler::Message(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nFileLogHandler::Error(const char* msg, va_list argList)
{
    vfprintf(this->logFile, msg, argList);
    fflush(this->logFile);
}

