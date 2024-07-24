//--------------------------------------------------------------------
//  nlaunch.cc
//  Minimales Launcherprogramm fuer Nebula-Scripts, enthaelt
//  speziellen Win32-Startup-Code um ein Aufpoppen der Konsole
//  zu unterbinden.
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nroot.h"
#include "script/ntclserver.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/nfileloghandler.h"

#ifdef __WIN32__
#include <windows.h>
#endif

//-------------------------------------------------------------------
//  launch()
//  29-May-99   floh    created
//  02-Feb-00   floh    route Nebula log messages into file
//-------------------------------------------------------------------
void launch(const char *script)
{
    n_assert(script);

    nKernelServer *ks;
    nScriptServer *ss;

    ks = new nKernelServer;
#ifdef __WIN32__
    nWin32LogHandler* lh = new nWin32LogHandler("nlaunch");
    ks->AddLogHandler(lh);
#endif
    nFileLogHandler* fh = new nFileLogHandler("nlaunch");
    ks->AddLogHandler(fh);
    ss = (nScriptServer *) ks->New("ntclserver","/sys/servers/script");
    n_assert(ss);
    const char* result;
    ss->RunScript(script, result);
    ss->Release();
#ifdef __WIN32__
    ks->RemoveLogHandler(lh);
    delete lh;
#endif
    ks->RemoveLogHandler(fh);
    delete fh;
    delete ks;
}

#ifdef __WIN32__
//-------------------------------------------------------------------
//  WinMain()
//  29-May-99   floh    created
//-------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int cmdshow)
{
    if (strlen(cmdline)>0) launch(cmdline);
    return 0;
}        
#else
//-------------------------------------------------------------------
//  main()
//  29-May-99   floh    created
//-------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if (argc==2) launch(argv[1]);
    else {
        printf("useage: nlaunch 'script'\n");
        return 5;
    }
    return 0;
}
#endif
//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------

    
    
        
    
    
            
        
        
