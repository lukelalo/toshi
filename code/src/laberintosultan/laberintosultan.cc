#ifdef __WIN32__
	// System includes
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

// Nebula includes
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/nfileloghandler.h"

// Constants
const char* scriptServerName = "ntclserver";
const char* startupScriptName = "laberintosultan:scripts/startup.tcl";
// Application entry method
#ifdef __WIN32__
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
    // Create just kernel and script servers then boot startup script
    nKernelServer* ks = new nKernelServer();
    
#ifdef __WIN32__
    nWin32LogHandler* lh = new nWin32LogHandler("laberintosultan");
    ks->AddLogHandler(lh);
#endif
    nFileLogHandler* fh = new nFileLogHandler("laberintosultan");
    ks->AddLogHandler(fh);
    
    nFileServer2* fs = ks->GetFileServer2();
    fs->SetAssign("laberintosultan", "home:data/laberintosultan/");
    nScriptServer* ss = (nScriptServer*)ks->New("ntclserver", 
                                                "/sys/servers/script");
    const char* result = NULL;

    char buf[N_MAXPATH];
    fs->ManglePath("laberintosultan:scripts/startup.tcl", buf, N_MAXPATH);
    ss->RunScript(buf, result);
    return 0;
}