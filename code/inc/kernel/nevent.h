#ifndef N_EVENT_H
#define N_EVENT_H
/*!
  \file
*/
//--------------------------------------------------------------------
/**
  @class nEvent
  @ingroup ThreadServices
  @brief Event for thread synchronization
    
  One or more threads can wait for a nEvent object to be
  signaled by another thread. 

  The Win32 implementation uses an Event Handle, the Linux
  implementation uses semaphores.
  
  THERE MAY BE DIFFERENCES IN BEHAVIOUR IF AN EVENT IS SIGNALLED
  WITHOUT ANY THREADS WAITING FOR IT. THE EVENT MAY OR MAY NOT
  REMAIN SIGNALLED BASED ON THE PLATFORM. 

  (C) 1999 A.Weissflog
*/  
//--------------------------------------------------------------------
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
#       include <semaphore.h>
#       include <sys/time.h>
#       include <unistd.h>
#   endif
#endif

class nEvent {
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    HANDLE wevent;
#   else
    sem_t sem;
#   endif
#endif
public:
	/// Simple class costructor
    nEvent();
	/// Simple class destrutor
    ~nEvent();
	/// Signal this event
    void Signal(void);
	/// Sets this event to wait state
    void Wait(void);
	/// Sets this event to wait for ms milliseconds
    bool TimedWait(long ms);
};

//--------------------------------------------------------------------
inline nEvent::nEvent()
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    this->wevent = CreateEvent(NULL,FALSE,FALSE,NULL);
    if (!this->wevent) n_error("nEvent::nEvent(): CreateEvent() failed!");
#   else
    if (sem_init(&(this->sem), 0, 0) != 0) {
        n_error("nEvent::nEvent() failed!");
    }
#   endif
#endif
}
//--------------------------------------------------------------------
inline nEvent::~nEvent()
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    CloseHandle(this->wevent);
#   else
    if (sem_destroy(&(this->sem)) != 0) {
        n_error("nEvent::~nEvent() failed!");
    }    
#   endif
#endif
}
//--------------------------------------------------------------------
inline void nEvent::Signal(void)
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    SetEvent(this->wevent);
#   else
    if (sem_post(&(this->sem)) != 0) {
        n_error("nEvent::Signal() failed!");
    }
#   endif
#endif
}
//--------------------------------------------------------------------
inline void nEvent::Wait(void)
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    WaitForSingleObject(this->wevent,INFINITE);
#   else
    sem_wait(&(this->sem));
#   endif
#endif
}
//--------------------------------------------------------------------
inline bool nEvent::TimedWait(long ms)
{
#ifndef __NEBULA_NO_THREADS__
#   ifdef __WIN32__
    int r = WaitForSingleObject(this->wevent,ms);
    return (WAIT_TIMEOUT == r) ? false : true;
#   else
    // HACK
    while(ms > 0) {
        if(0==sem_trywait(&(this->sem))) return true;
        usleep(1000);
        ms -= 1;
    }
    return false;
#   endif
#endif
}
//--------------------------------------------------------------------
#endif
