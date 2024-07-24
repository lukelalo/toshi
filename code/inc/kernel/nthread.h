#ifndef N_THREAD_H
#define N_THREAD_H
/*!
  \file
*/
//-------------------------------------------------------------------
/**
        @defgroup ThreadServices Threading Services
        @ingroup NebulaKernelModule
*/
/**
        @class nThread 
        @ingroup ThreadServices
        @brief A wrapper class for threads with an attached message
        port.

        Wraps a user defined thread function into a C++ object, 
        additionally offers a message list for safe communication
        with the thread function.   

        (C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_MUTEX_H
#include "kernel/nmutex.h"
#endif

#ifndef N_EVENT_H
#include "kernel/nevent.h"
#endif

#ifndef N_THREADAFELIST_H
#include "kernel/nthreadsafelist.h"
#endif

#ifndef N_MSGNODE_H
#include "util/nmsgnode.h"
#endif

#undef N_DEFINES
#define N_DEFINES nThread
#include "kernel/ndefdllclass.h"
//-------------------------------------------------------------------
//  Prefix fuer Thread-Funktion
//-------------------------------------------------------------------
#ifdef __WIN32__
#define N_THREADPROC __stdcall
#else
#define N_THREADPROC
#endif  

class N_PUBLIC nThread {
#ifndef __NEBULA_NO_THREADS__
    nMutex user_data_mutex;
    nEvent sleep_event;
    nEvent shutdown_sleep_event;
    nEvent shutdown_event;
    nEvent startup_event;
    bool stop_thread;
    bool shutdown_signal_received;
    int (N_THREADPROC *thread_func)(nThread *);
    void (*wakeup_func)(nThread *);
        
    nThreadSafeList *msg_list;
    bool is_extmsglist;
    void *user_data; 

#   ifdef __WIN32__
    HANDLE thread;
#   else
    pthread_t thread;
#   endif
#endif
    
    enum {
        N_DEFAULT_STACKSIZE = 4096,
    };
        
public:
    nThread(int (N_THREADPROC *_thread_func)(nThread *),
            ulong stack_size,
            void (*_wakeup_func)(nThread *),
            nThreadSafeList *_ext_msglist,
            void *_user_data);

    virtual ~nThread(void);
    
    /**
        @name Thread Management
    */
    //@{
    /// Signals the start of the thread
    virtual void ThreadStarted(void);
    /// Signals a stop request for this thread
    virtual bool ThreadStopRequested(void);
    /// Sets the thread into sleep state
    virtual void ThreadSleep(float sec);
    /// Kill off the thread
    virtual void ThreadHarakiri(void);
    //@}

    /**
        @name Message Handling
    */
    //@{
    virtual void WaitMsg(void);
    /// Removes first message in message list
    virtual nMsgNode *GetMsg(void);
    virtual void ReplyMsg(nMsgNode *);
    /// Puts a message to the message list
    virtual void PutMsg(void *buf, ulong buf_size);
    //@}
 
    /**
        @name User Data Field Access
    */
    //@{
    /// Locks mutex
    virtual void *LockUserData(void);
    /// Unlocks mutex
    virtual void UnlockUserData(void);
    //@}
};

//-------------------------------------------------------------------
#endif    
