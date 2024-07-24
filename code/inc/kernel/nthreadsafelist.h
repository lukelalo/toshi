#ifndef N_THREADSAFELIST_H
#define N_THREADSAFELIST_H
/*!
  \file
*/
//-------------------------------------------------------------------
/**
        @class nThreadSafeList 
        @ingroup NebulaDataTypes
        @ingroup ThreadServices
        @brief A thread safe doubly linked list

        Offers method to manipulate lists in a thread safe way,
        so that the list can be used as a communication point between
        threads.

        (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_MUTEX_H
#include "kernel/nmutex.h"
#endif

#ifndef N_EVENT_H
#include "kernel/nevent.h"
#endif

class nThreadSafeList : public nList {
    nMutex mutex;
    nEvent event;

public:
	/// Signals the event object
    void SignalEvent(void);
	/// Send the event object a wait
    void WaitEvent(void); 
    void TimedWaitEvent(float sec);
	/// Locks the the mutex for this list
    void Lock(void);
	/// Unlocks the mutex for this list
    void Unlock(void);
};
//--------------------------------------------------------------------
inline void nThreadSafeList::SignalEvent(void)
{
    this->event.Signal();
}
//--------------------------------------------------------------------
inline void nThreadSafeList::WaitEvent(void)
{
    this->event.Wait();
}
//--------------------------------------------------------------------
inline void nThreadSafeList::Lock(void)
{
    this->mutex.Lock();
}
//--------------------------------------------------------------------
inline void nThreadSafeList::Unlock(void)
{
    this->mutex.Unlock();
}
//--------------------------------------------------------------------
#endif
