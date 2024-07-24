#ifndef N_WIN32LOGHANDLER_H
#define N_WIN32LOGHANDLER_H

#ifndef N_SYSTEM_H
#include "kernel/nsystem.h"
#endif

#if defined(__WIN32__) && !defined(__XBxX__)
//------------------------------------------------------------------------------
/**
    A log handler class for Win32 apps:
    
    - maintains a log file in the application directory where ALL
      output is recorded
    - creates a message box for nKernelServer::Message() 
      and nKernelServer::Error()

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_LOGHANDLER_H
#include "kernel/nloghandler.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nWin32LogHandler
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nWin32LogHandler : public nLogHandler
{
public:
    /// constructor
    nWin32LogHandler(const char* appName);
    /// destructor
    virtual ~nWin32LogHandler();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);

private:
    enum MsgType
    {
        MsgTypeMessage,
        MsgTypeError,
    };

    /// put a message box on screen
    void PutMessageBox(MsgType msgType, const char* msg, va_list argList);

    nString appName;
};

//------------------------------------------------------------------------------
#endif /* defined(__WIN32__) && !defined(__XBxX__) */
#endif

