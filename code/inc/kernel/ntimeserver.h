#ifndef N_TIMESERVER_H
#define N_TIMESERVER_H
/*!
  \file
*/
/**
    @defgroup TimeServices Time Services
    @ingroup NebulaKernelModule

    @brief Nebula provides a single realtime time source to assist
    in decoupling simulation speed from the frame rate.
    
    Nebula has two main time management services.  The first is
    nTimeServer, which provides time management function and
    must be updated each frame.  The second is nProfiler, which
    is a simple class for measuring time intervals.
*/

#include <stdlib.h>
#include <stdio.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifdef __WIN32__
#   ifndef _INC_WINDOWS
#   include <windows.h> 
#   endif
#elif defined(__LINUX__) || defined(__MACOSX__)
#include <sys/time.h>
#include <unistd.h>
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_REF_H
#include "kernel/nref.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

//-------------------------------------------------------------------
#undef N_DEFINES
#define N_DEFINES nTimeServer
#include "kernel/ndefdllclass.h"

/**
    @class nProfiler
    @ingroup TimeServices
    @brief nProfiler provides an easy way to measure time intervals.

    nProfiler objects can be queried by a corresponding environment
    variable, @c /sys/var/prof_[name], where [name] is the name given
    to the constructor.  The value returned by the environment
    variable is updated when the nProfiler is rewound (by calling
    Rewind()).

    Create a new profiler.

    @code
        nProfiler *p_update;
        // This creates a timer with an nEnv at /sys/var/prof_my_update
        p_update = timeServer->NewProfiler("my_update");
    @endcode

    Start() and Stop() can be called multiple times, and the total time
    accumulated will be put into the nEnv once the profiler is rewound,
    not just the interval between the last Start()/Stop() sequence.

    @code
        p_update->Start();

        ...

        p_update->Stop();
    @endcode

    Update the nEnv and reset the profiler.

    @code
        p_update->Rewind();
    @endcode
*/
//-------------------------------------------------------------------
class nProfiler : public nStrNode {
public:
    nRef<nEnv> ref_env;
    bool is_started;
#   ifdef __WIN32__
    LONGLONG start;
    LONGLONG accum;
#   else
    long long int start;
    long long int accum;
#   endif

    inline nProfiler(nKernelServer *ks, nRoot *owner, const char *name);
    inline ~nProfiler();

    /// Start the timer.
    inline void Start(void);
    /// Stop the timer.
    inline void Stop(void);
    /**
        Reset the timer, and store the total time accumulated since the
        last call to Rewind() within the nEnv variable.
    */
    inline void Rewind(void);
};

inline nProfiler::nProfiler(nKernelServer *ks, nRoot *owner, const char *name)
      : nStrNode(name),
        ref_env(owner)
{
    char buf[N_MAXPATH];
    sprintf(buf,"/sys/var/prof_%s",name);
    nRoot *o = ks->Lookup(buf);
    n_assert(NULL == o);
    ref_env = (nEnv *) ks->New("nenv",buf);
    this->start = 0;
    this->accum = 0;
    this->is_started = false;
}

inline nProfiler::~nProfiler()
{
    if (ref_env.isvalid()) ref_env->Release();
}

#ifdef __WIN32__
inline void nProfiler::Start(void) {
    n_assert(!this->is_started);
    QueryPerformanceCounter((LARGE_INTEGER *)&(this->start));
    this->is_started = true;
}
inline void nProfiler::Stop(void) {
    n_assert(this->is_started);
    LONGLONG stop;
    QueryPerformanceCounter((LARGE_INTEGER *)&stop);
    this->accum += (stop - this->start);
    this->is_started = false;
}
inline void nProfiler::Rewind(void) {
    n_assert(!this->is_started);
    LONGLONG freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    double d_accum = (double) this->accum;
    double d_freq  = (double) freq;
    double d_time  = d_accum / d_freq;
    ref_env->SetF((float)d_time);
    this->accum = 0;
}

#elif defined(__LINUX__) || defined(__MACOSX__)
#define N_MICROSEC_INT    (1000000)
#define N_MICROSEC_FLOAT  (1000000.0)
inline void nProfiler::Start(void) {
    n_assert(!this->is_started);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->start = tv.tv_sec * N_MICROSEC_INT + tv.tv_usec;
    this->is_started = true;
}
inline void nProfiler::Stop(void) {
    n_assert(this->is_started);
    struct timeval tv;
    long long int stop;
    gettimeofday(&tv,NULL);
    stop = tv.tv_sec * N_MICROSEC_INT + tv.tv_usec;
    this->accum += stop - this->start;
    this->is_started = false;
}
inline void nProfiler::Rewind(void) {
    n_assert(!this->is_started);
    double d_accum = (double) this->accum;
    double d_time = (d_accum / N_MICROSEC_FLOAT);
    ref_env->SetF((float)d_time);
    this->accum = 0;
}

#else
#error "nProfiler class not supported"
#endif

/**
	@class nTimeServer 
    @ingroup TimeServices
	@brief A realtime high resolution time source

    nTimeServer provides a single realtime source to decouple simulation
    speed from the frame rate. It also offers a 'mini' high-resolution timer
    class, nProfiler, that can be used by client code for profiling and
    local time measurement reasons.

    Time is started when the time server is created. There are various
    functions for controlling time.

        - nTimeServer::StartTime()
        - nTimeServer::StopTime()
        - nTimeServer::ResetTime()
        - nTimeServer::SetTime()

    The current time value can be retrieved with nTimeServer::GetTime()
    and nTimeServer::GetFrameTime(). The difference being that the value
    returned by nTimeServer::GetFrameTime() is guaranteed to be the same
    between two nTimeServer::Trigger() calls, which is usually one frame.

    The time server usually updates time based on system timers, but it can
    be set to a fixed time step with nTimeServer::LockDeltaT(). Setting
    this causes time to be increased by the value passed to
    nTimeServer::LockDeltaT() each time nTimeServer::Trigger() is
    called, rather than the actual time taken between calls. For example:

    @code
        timeServer->LockDeltaT(0.01);
    @endcode

    This would mean that time increases by 0.01 seconds each frame, whatever
    the frame rate.

    nTimeServer::WaitDeltaT() can be used to make Nebula sleep for a set
    amount of time for each call to nTimeServer::Trigger().

	(C) 1999 A.Weissflog
*/
//-------------------------------------------------------------------
class N_DLLCLASS nTimeServer : public nRoot {
protected:
    bool stopped;
    bool frame_enabled;
    double frame_time;
    double lock_delta_t;
    double wait_delta_t;
    double lock_time;
	/// list of living nProfiler objects
    nStrList prof_list;             

#   ifdef __WIN32__
    LONGLONG time_diff;
    LONGLONG time_stop;
#   else
    long long int time_diff;
    long long int time_stop; 
#   endif

public:
    static nClass *local_cl;
    static nKernelServer *ks;

    nTimeServer();
    virtual ~nTimeServer();

    /**
        @brief Trigger updates the time server's internal data and must be
        called every frame.
    */
    virtual void Trigger(void);

    /**
        @name General Time Data

        The general methods for accessing and manipulating time.
        nTimeServer::GetTime() always returns the exact current
        time.  For frame-based time, see nTimeServer::GetFrameTime().
    */
    //@{
    virtual void ResetTime(void);
    virtual void StopTime(void); 
    virtual void StartTime(void);
    virtual double GetTime(void);
    virtual void SetTime(double);
    //@}

    /**
        @name Profiler Timing

        Manage nProfile objects.  See the documentation for nProfiler
        for further details.
    */
    //@{
    virtual nProfiler *NewProfiler(const char *);
    virtual void ReleaseProfiler(nProfiler *);
    virtual nStrList *GetProfilers(void);
    //@}

    /**
        @name Fixed Time Stepping

        Locking the delta in time specifies that a fixed quantity of
        time be used to increment the time for each call to
        nTimeServer::Trigger().
    */
    //@{
    virtual void LockDeltaT(double);
    virtual double GetLockDeltaT(void);
    //@}

    /**
        @name Waiting

        The wait delta time is used to make Nebula sleep for the
        specified amount of time for each call to nTimeServer::Trigger().

        This allows Nebula to be CPU friendly and yield processor time
        to other applications when a maximum framerate is not required.
    */
    //@{
    virtual void WaitDeltaT(double);
    virtual double GetWaitDeltaT(void);
    //@}

    /**
        @name Frame Time

        When frame time is enabled, nTimeServer::getFrameTime() will
        return an exactly identical value in between two calls to
        nTimeServer::Trigger().
    */
    //@{
    virtual void EnableFrameTime(void);
    virtual double GetFrameTime(void);
    virtual void DisableFrameTime(void);
    //@}
};
//--------------------------------------------------------------------
#endif
