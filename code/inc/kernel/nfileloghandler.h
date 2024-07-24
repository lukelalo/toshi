#ifndef N_FILELOGHANDLER_H
#define N_FILELOGHANDLER_H
//------------------------------------------------------------------------------
/**
    A file based log handler class:
    
    - maintains a log file in the home: directory where ALL
      output is recorded

    (C) 2003 RadonLabs GmbH
*/
#ifndef N_LOGHANDLER_H
#include "kernel/nloghandler.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#undef N_DEFINES
#define N_DEFINES nFileLogHandler
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nFileLogHandler : public nLogHandler
{
public:
    /// constructor
    nFileLogHandler(const char* logName);
    /// destructor
    virtual ~nFileLogHandler();
    /// print a message to the log dump
    virtual void Print(const char* msg, va_list argList);
    /// show an important message (may block the program until the user acks)
    virtual void Message(const char* msg, va_list argList);
    /// show an error message (may block the program until the user acks)
    virtual void Error(const char* msg, va_list argList);

private:
    nString logName;
    FILE* logFile;
};

//------------------------------------------------------------------------------
#endif

