#ifndef N_QUICKWATCH_H
#define N_QUICKWATCH_H
//-------------------------------------------------------------------
/**
    @class nQuickWatch

    @brief Easy way to create an nEnv variable for debugging purposes.

    Produces an nEnv variable under @c /sys/var/ and supplies it
    with an initial type.  These can be seen by using the
    @c /sys/servers/console.watch command, or by accessing the
    nQuickWatch object directly as if it were an nEnv object.

    @b PERFORMANCE @b WARNING <br>
    These should only be used for debugging because the
    nQuickWatch does a @c Lookup() with each new instance.
*/
//-------------------------------------------------------------------
#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_ENV_H
#include "kernel/nenv.h"
#endif

//-------------------------------------------------------------------
class nQuickWatch {
    nEnv *env;
public:
    /// Create a new nQuickWatch with a given name.
    nQuickWatch(nKernelServer *ks, const char *n) {
        char buf[N_MAXPATH];
        sprintf(buf,"/sys/var/%s",n);
        env = (nEnv *) ks->Lookup(buf);
        if (!env) {
            env = (nEnv *) ks->New("nenv",buf);
        }
    };
    /// Create a new nQuickWatch with a given name and an initial type.
    nQuickWatch(nKernelServer *ks, const char *n, nArg::ArgType initial_type)
    {
        char buf[N_MAXPATH];
        sprintf(buf,"/sys/var/%s",n);
        env = (nEnv *) ks->Lookup(buf);
        if (!env) {
            env = (nEnv *) ks->New("nenv",buf);
            switch (initial_type) {
                case nArg::ARGTYPE_INT:       env->SetI(0);       break;
                case nArg::ARGTYPE_FLOAT:     env->SetF(0.0f);    break;
                case nArg::ARGTYPE_STRING:    env->SetS("empty"); break;
                case nArg::ARGTYPE_BOOL:      env->SetB(false);   break;
                case nArg::ARGTYPE_OBJECT:    env->SetO(NULL);    break;
                case nArg::ARGTYPE_VOID:      break;
                case nArg::ARGTYPE_CODE:	  break;
            }
        }
    };
    /// Access the nQuickWatch as if it were an nEnv.
    nEnv *operator->() {
        return env;
    };
};
//-------------------------------------------------------------------
#endif
