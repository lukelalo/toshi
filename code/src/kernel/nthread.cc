#define N_IMPLEMENTS nThread
#define N_KERNEL
//-------------------------------------------------------------------
//  nthread.cc
//  Wrapper class for a thread with attached msg port. 
//  (C) 1998-2000 Andre Weissflog
//-------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nthread.h"

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
     - 27-Apr-99   floh    + support for __NEBULA_NO_THREADS__
     - 20-Feb-00   floh    + Win32: rewritten to _beginthreadx() instead
                             of _beginthread()
*/
//-------------------------------------------------------------------
nThread::nThread(int (N_THREADPROC *_thread_func)(nThread *),
                 ulong stack_size,
                 void (*_wakeup_func)(nThread *),
                 nThreadSafeList *_ext_msglist,
                 void *_user_data)
{
#ifndef __NEBULA_NO_THREADS__    
    n_assert(_thread_func);
    if (!stack_size) stack_size = N_DEFAULT_STACKSIZE;
    if (_ext_msglist) {
        this->msg_list = _ext_msglist;
        this->is_extmsglist = TRUE;
    } else {
        this->msg_list = n_new nThreadSafeList;
        this->is_extmsglist = FALSE;
    }
    this->thread_func = _thread_func;
    this->wakeup_func = _wakeup_func; 
    this->user_data   = _user_data;
    this->stop_thread = FALSE;
    this->shutdown_signal_received = FALSE;
    
    // launch thread
#   ifdef __WIN32__
    // we are using _beginthreadx() instead of CreateThread(),
    // because we want to use c runtime functions from within the thread
    unsigned int thrdaddr;
    this->thread = (HANDLE) _beginthreadex(
                   NULL,    // security
                   stack_size,
                   (unsigned (__stdcall *)(void *))_thread_func,
                   this,    // arglist
                   0,       // init_flags
                   &thrdaddr);
    if ((long)this->thread == 0) {
        n_error("nThread::nThread(): _beginthreadx() failed!\n");
    }     
#   else
    // FIXME: ignore stack size under Linux
    int pok = pthread_create(&(this->thread),
                             NULL,
                             (void *(*)(void *))_thread_func,
                             this);
    if ((pok == EAGAIN) || (pok == EINVAL)) n_error("nThread::nThread(): pthread_create() failed!");
#    endif
    // wait until the thread has started
    this->startup_event.Wait();
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
     - 30-Oct-98   floh    Wenn sich der Thread lange vor
                           dem Aufruf des Destruktors selbst terminiert
                           hat, konnte es passieren, das WaitForSingleObject()
                           endlos haengen blieb, weil das Signal
                           offensichtlich schon lange verraucht war.
                           ThreadHarakiri() blockiert den Thread jetzt
                           solange, bis der Destruktor das shutdown_event
                           signalisiert
     - 31-Oct-98   floh    das shutdown_event konnte signalisiert werden,
                           bevor der Thread in ThreadHarakiri() darauf
                           warten konnte... deshalb setzt der Destruktor
                           jetzt das Signal in einer ausgebremsten Schleife
                           sooft, bis ThreadHarakiri() das Signal wirklich
                           empfangen konnte.   
     - 26-Dec-98   floh    auf das shutdown-Signal vom Thread wird jetzt
                           nicht mehr in einer Schleife gewartet, weil
                           nEvent unter Linux jetzt auf Posix-Semaphoren
                           umgeschrieben wurde (welche hoffentlich 
                           funktionieren).
     - 27-Apr-99   floh    + Support fuer __NEBULA_NO_THREADS__
     - 03-Feb-00   floh    + changed WaitForSingleObject() from
                             INFINITE to 1000 milliseconds
     - 20-Feb-00   floh    + Win32: rewritten to _beginthreadx(), _endthreadx()
     - 08-Nov-00   floh    + WaitForSingleObject() waits 500 milliseconds.
                             under WinNT/2000 with several socket threads
                             open, it can still happen that the socket
                             does not return
*/
//-------------------------------------------------------------------
nThread::~nThread(void)
{
#ifndef __NEBULA_NO_THREADS__
    // wake up thread, if a wakeup func is defined
    this->stop_thread = TRUE;
    if (this->wakeup_func) this->wakeup_func(this);
    
    // signal the thread that it may terminate now
    this->shutdown_event.Signal();
    
    // wait until the thread has indeed terminated
    // (do nothing under Win32, because _endthreadex()
    // will be called at the end of the thread, which
    // does the CloseHandle() stuff itself
#   ifdef __WIN32__
    WaitForSingleObject(this->thread,500);
    CloseHandle(this->thread);
    this->thread = 0;
#   else
    pthread_join(this->thread,NULL);
    this->thread = 0;
#    endif
    
    // flush msg list (all remaining messages will be lost)
    if (!(this->is_extmsglist)) {
        nMsgNode *nd;
        this->msg_list->Lock();
        while ((nd = (nMsgNode *) this->msg_list->RemHead())) n_delete nd;
        this->msg_list->Unlock();
        n_delete this->msg_list;
    }
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
     - 30-Oct-98   floh    wartet jetzt auf den Destruktor
     - 27-Apr-99   floh    + Support fuer __NEBULA_NO_THREADS__
     - 20-Feb-00   floh    + rewritten to _endthreadex()
*/
//-------------------------------------------------------------------
void nThread::ThreadHarakiri(void)
{
#ifndef __NEBULA_NO_THREADS__
    // synchronize with destructor
    this->shutdown_event.Wait();
    this->shutdown_signal_received = TRUE;
#   ifdef __WIN32__
    _endthreadex(0);
#   else
      pthread_exit(0);
#   endif
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
     - 27-Apr-99   floh    + Support fuer __NEBULA_NO_THREADS__
*/
//-------------------------------------------------------------------
void nThread::ThreadStarted(void)
{
#ifndef __NEBULA_NO_THREADS__
    this->startup_event.Signal();
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
     - 27-Apr-99   floh    + Support fuer __NEBULA_NO_THREADS__
*/
//-------------------------------------------------------------------
bool nThread::ThreadStopRequested(void)
{
#ifndef __NEBULA_NO_THREADS__
    return this->stop_thread;
#else
    return true;
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
void nThread::ThreadSleep(float sec)
{
#ifndef __NEBULA_NO_THREADS__
    n_sleep(sec);
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
nMsgNode *nThread::GetMsg(void)
{
#ifndef __NEBULA_NO_THREADS__
    nMsgNode *nd;
    this->msg_list->Lock();
    nd = (nMsgNode *) this->msg_list->RemHead();
    this->msg_list->Unlock();
    return nd;
#else
    return NULL;
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
void nThread::ReplyMsg(nMsgNode *nd)
{
#ifndef __NEBULA_NO_THREADS__
    n_delete nd;
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
void nThread::WaitMsg(void)
{
#ifndef __NEBULA_NO_THREADS__
    this->msg_list->WaitEvent();
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
void nThread::PutMsg(void *buf, ulong size)
{
#ifndef __NEBULA_NO_THREADS__
    n_assert(buf);
    n_assert(size > 0);
    nMsgNode *nd = n_new nMsgNode(buf,size);
    this->msg_list->Lock();
    this->msg_list->AddTail(nd);
    this->msg_list->Unlock();
    this->msg_list->SignalEvent();
#endif
}
 
//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
void *nThread::LockUserData(void)
{
#ifndef __NEBULA_NO_THREADS__
    if (this->user_data) this->user_data_mutex.Lock();
    return this->user_data;
#else
    return NULL;
#endif
}

//-------------------------------------------------------------------
/**
     - 20-Oct-98   floh    created
*/
//-------------------------------------------------------------------
void nThread::UnlockUserData(void)
{
#ifndef __NEBULA_NO_THREADS__
    this->user_data_mutex.Unlock();
#endif
}

