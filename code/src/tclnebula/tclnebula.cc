//-------------------------------------------------------------------
//  tclnebula.cc
//
//  Nebula as proper Tcl extension.
//  Code taken from:
//
//  A sample dynamically loadable extension for Tcl 8.0
//
//  by Scott Stanton w/ help from Ray Johnson
//  scott.stanton@eng.sun.com
//  ray.johnson@eng.sun.com
//
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "tclnebula/tclnebula.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "script/ntclserver.h"

static nKernelServer *ks = NULL;

/*
 *----------------------------------------------------------------------
 *
 * DllEntryPoint --
 *
 *	This wrapper function is used by Windows to invoke the
 *	initialization code for the DLL.  If we are compiling
 *	with Visual C++, this routine will be renamed to DllMain.
 *	routine.
 *
 * Results:
 *	Returns TRUE;
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
#ifdef __WIN32__
extern "C" __declspec(dllexport) BOOL APIENTRY
DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            // Create a new Nebula runtime and store the pointer
            // to the kernel server in the global
            // ks variable. Since each process has its own address
            // space, we don't need to care about overwriting
            // the pointer (I guess)
            printf("tclnebula: DLL_PROCESS_ATTACH\n");
            ks = new nKernelServer();
            n_assert(ks);
            break;

        case DLL_PROCESS_DETACH:
            // time for clean up, kill the kernel server, this
            // will shutdown Nebula completely
            printf("tclnebula: DLL_PROCESS_DETACH\n");
            if (ks) {
                delete ks;
                ks = NULL;
            }
            break;
    }
    return TRUE;
}
#endif

//-------------------------------------------------------------------
//  tclnebula_Init()
//  Called by Tcl interpreter to initialize Nebula extension.
//  23-Nov-00   floh    created
//-------------------------------------------------------------------
extern "C" __declspec(dllexport) int Tclnebula_Init(Tcl_Interp *interp)
{   
    // If there is no tcl server object yet, create one and
    // initialize it as Tcl extension. This only happens once
    // for the process, don't know yet whether this will
    // be a problem with several Tcl interpreters running
    // in the same process
    n_assert(interp);
    n_assert(ks);
    nTclServer *ts = (nTclServer *) ks->Lookup("/sys/servers/script");
    if (!ts) {
        ts = (nTclServer *) ks->New("ntclserver","/sys/servers/script");
        n_assert(ts);
    }
    
    // add the Nebula specific commands to the Tcl interpreter
    ts->InitAsExtension(interp);

    // that's it
    int res = Tcl_PkgProvide(interp, "tclnebula", "1.0");
    return res;
}

//-------------------------------------------------------------------
//  tclnebula_SafeInit()
//  Same as tclnebula_Init() but for a safe interpreter (would
//  leave out any commands that are considered unsafe
//  23-Nov-00   floh    created
//-------------------------------------------------------------------
extern "C" __declspec(dllexport) int Tclnebula_SafeInit(Tcl_Interp *interp)
{
    // create a Nebula kernel server object
    nKernelServer *ks = new nKernelServer();
    n_assert(ks);

    // create the Nebula Tcl server object and initialize
    // as a Tcl extension
    nTclServer *ts = (nTclServer *) ks->New("ntclserver","/sys/servers/script");
    n_assert(ts);
    ts->InitAsExtension(interp);

    // that's it
    int res = Tcl_PkgProvide(interp, "tclnebula", "1.0");
    return res;
}
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
