#ifndef N_SYSTEM_H
#include "kernel/nsystem.h"
#endif

#if defined(__WIN32__) && !defined(__XBxX__)

#define N_IMPLEMENTS nWin32LogHandler
#define N_KERNEL
//------------------------------------------------------------------------------
//  nwin32loghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nwin32loghandler.h"
#include "util/nstring.h"

#include <windows.h>
#include <assert.h>

//------------------------------------------------------------------------------
/**
    The constructor takes an application name which is used to name the
    log file and which is shown in the error message box titles.

    @param  app     an application name
*/
nWin32LogHandler::nWin32LogHandler(const char* app)
{
    this->appName = app;
}

//------------------------------------------------------------------------------
/**
*/
nWin32LogHandler::~nWin32LogHandler()
{
  //empty
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Print(const char* msg, va_list argList)
{
  //empty
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Message(const char* msg, va_list argList)
{
    this->PutMessageBox(MsgTypeMessage, msg, argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nWin32LogHandler::Error(const char* msg, va_list argList)
{
    this->PutMessageBox(MsgTypeError, msg, argList);
}

//------------------------------------------------------------------------------
/**
    Generate a Win32 message box.
*/
void
nWin32LogHandler::PutMessageBox(MsgType type, const char* msg, va_list argList)
{
    UINT boxType = (MB_OK | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST);
    switch (type)
    {
        case MsgTypeMessage:
            boxType |= MB_ICONINFORMATION;
            break;
        case MsgTypeError:
            boxType |= MB_ICONERROR;
            break;
    }
    // FIXME: unsafe!
    char msgBuf[2048];
    int len = vsprintf(msgBuf, msg, argList);
    assert(len < (sizeof(msgBuf) - 1));
    MessageBox(0, msgBuf, this->appName.Get(), boxType);
}
#endif /* defined __WIN32__ && !defined(__XBxX__) */
