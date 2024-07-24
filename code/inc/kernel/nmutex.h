#ifndef N_MUTEX_H
#define N_MUTEX_H
/*!
  \file
*/
//-------------------------------------------------------------------
/**
  @class nMutex
  @ingroup ThreadServices
  @brief Mutex wrapper class

  Implements a simple mutex object for thread synchronization.
  Win32: win32 mutex handles (CreateMutex())
  Linux: posix thread mutexes
    
  (C) 1999 A.Weissflog
*/  
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
#       ifndef _INC_WINDOWS
#       include <windows.h>
#       endif
#       ifndef _INC_PROCESS
#       include <process.h>
#       endif
#   else
#   include <pthread.h>
#   endif
#endif

class nMutex {
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    HANDLE wmutex;
#   else
    pthread_mutex_t pmutex;
#   endif
#endif
public:
	/// Standard constuctor
    nMutex();
	/// Standard destructor
    ~nMutex();
	/// Lock the mutex
    void Lock(void);
	/// Unlock mutex
    void Unlock(void);
};
//-------------------------------------------------------------------
inline nMutex::nMutex()
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    this->wmutex = CreateMutex(NULL,FALSE,NULL);
    if (!(this->wmutex)) n_error("nMutex::nMutex(): CreateMutex() failed!");
#   else
    if (pthread_mutex_init(&(this->pmutex),NULL) != 0) {
        n_error("nMutex::nMutex(): pthread_mutex_init() failed!");
    }
#   endif
#endif
}
//-------------------------------------------------------------------
inline nMutex::~nMutex()
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    CloseHandle(this->wmutex);
#   else
    pthread_mutex_destroy(&(this->pmutex));
#   endif
#endif
}
//-------------------------------------------------------------------
inline void nMutex::Lock(void)
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    WaitForSingleObject(this->wmutex,INFINITE);
#   else
    pthread_mutex_lock(&(this->pmutex));
#   endif
#endif
}
//-------------------------------------------------------------------
inline void nMutex::Unlock(void)
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    ReleaseMutex(this->wmutex);
#   else
    pthread_mutex_unlock(&(this->pmutex));
#   endif
#endif
}
//-------------------------------------------------------------------
#endif
