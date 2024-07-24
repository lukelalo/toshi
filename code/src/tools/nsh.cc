//--------------------------------------------------------------------
//  nsh.cc - A simple shell to get Nebula started
//
//  (C) 1998 Andre Weissflog
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "kernel/nenv.h"
#include "script/ntclserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nwin32loghandler.h"
#include "kernel/nfileloghandler.h"

int main(int argc, char *argv[])
{
    int retval = 0;
    char arg_cmd[1024];
    char *startup_script = 0;
    char *run_script     = 0;
    char *script_server  = "ntclserver";
    char *file_server = "nfileserver2";
    bool show_help = false;
    bool quiet = false;
    int i;

    // Args auswerten
    arg_cmd[0] = 0;
    for (i=1; i<argc; i++)
    {
        if (strcmp(argv[i],"-help")==0)
        {
            show_help=true;
        }
        else if (strcmp(argv[i], "-startup")==0)
        {
            if (++i < argc)
            {
                startup_script = argv[i];
            }
            else
            {
                printf("args error: startup script filename expected\n");
                return 10;
            }
        }
        else if (strcmp(argv[i], "-run")==0)
        {
            if (++i < argc)
            {
                run_script = argv[i];
            }
            else
            {
                printf("args error: run script filename expected\n");
                return 10;
            }
        }
        else if (strcmp(argv[i], "-quiet")==0)
        {
            quiet = true;
        }
        else if (strcmp(argv[i], "-script")==0)
        {
            if (++i < argc)
            {
                script_server = argv[i];
            }
            else
            {
                printf("args error: script server class name expected\n");
                return 10;
            }
        }
        else if (strcmp(argv[i], "-file")==0)
        {
            if (++i < argc)
            {
                file_server = argv[i];
            }
            else
            {
                printf("args error: fileserver2 class name expected\n");
                return 10;
            }
        }
        else if (strcmp(argv[i], "-args")==0)
        {
            // alle restlichen Args gehen an Tcl
            for (++i; i<argc; i++)
            {
                strcat(arg_cmd,argv[i]);
                strcat(arg_cmd," ");
            }
        }
        else
        {
            printf("args error: unknown arg: %s\n",argv[i]);
            return 10;
        }
    }

    // Banner
    if ((!quiet) && show_help)
    {
        printf("nsh -- the Tcl|Nebula shell\n"
               "Command line args:\n"
               "  -help                 - show this help\n"
               "  -startup [script]     - run script and go interactive\n"
               "  -run [script]         - run script and terminate\n"
               "  -script class         - use 'class' as script server\n"
               "  -file class           - use 'class' as fileserver2\n"
               "  -quiet                - be somewhat quiet (for shell scripts)\n"
               "  -args [rest of line]  - args to give to the tcl interpreter (in $argv)\n"
               "Builtin commands (when using Tcl script server):\n"
               "  name = new class name - create new object\n"
               "  delete name           - delete object\n"
               "  name = sel name       - select object\n"
               "  name = psel           - return selected object\n"
               "  name = get name       - get object from filesystem\n"
               "  server portname       - go into servermode\n"
               "  connect host:portname - connect to a server\n"
               "  disconnect            - disconnect from a server\n"
               "  exit                  - exit nsh or kill server\n"
               "  ...plus the Tcl command set\n"
               "  Hint: '.getcmds' gives list of commands for selected object\n");
    }

    nKernelServer *ks = new nKernelServer();
    if (ks)
    {
        // Set up logging
#ifdef __WIN32__
        nWin32LogHandler* lh = new nWin32LogHandler("nsh");
        ks->AddLogHandler(lh);
#endif
        nFileLogHandler* fh = new nFileLogHandler("nsh");
        ks->AddLogHandler(fh);

        // create a fileserver2
        if (ks->New(file_server, "/sys/servers/file2"))
        {
            // create script server
            nScriptServer *ss = (nTclServer *)  ks->New(script_server, "/sys/servers/script");
            if (ss)
            {
                if (arg_cmd[0])
                {
                    nEnv *args = (nEnv *) ks->New("nenv","/sys/share/args");
                    args->SetS(arg_cmd);
                }
                if (startup_script)
                {
                    if (!quiet)
                    {
                        printf("running startup script: %s\n",startup_script);
                    }
                    const char *res;
                    ss->RunScript(startup_script, res);
                    if (res) printf("%s\n",res);
                }
                if (run_script)
                {
                    if (!quiet)
                    {
                        printf("running script: %s\n",run_script);
                    }
                    const char *res;
                    ss->RunScript(run_script, res);
                    if (res)
                    {
                        printf("%s\n",res);
                    }
                }
                else
                {
                    // interactively execute commands
                    bool lineOk = true;
                    ss->SetFailOnError(false);
                    while (!ss->GetQuitRequested() && lineOk)
                    {
                        char line[1024];

                        // generate prompt string
                        char promptBuf[N_MAXPATH];
                        printf("%s", ss->Prompt(promptBuf, sizeof(promptBuf)));
                        fflush(stdout);

                        // get user input
                        lineOk = (gets(line) > 0);
                        if (strlen(line) > 0)
                        {
                            const char* result = 0;
                            ss->Run(line, result);
                            if (result)
                            {
                                printf("%s\n", result);
                            }
                        }
                    }
                }
                ss->Release();
            }
            else
            {
                retval = 10;
                if (!quiet) printf("Could not create script server object!\n");
            }
        }
        else 
        {
            retval = 10;
            if (!quiet) printf("Could not create file server object!\n");
        }
        
#ifdef __WIN32__
        ks->RemoveLogHandler(lh);
        delete lh;
#endif
        ks->RemoveLogHandler(fh);
        delete fh;        
        
        delete ks;
    }
    else
    {
        retval = 10;
        if (!quiet) printf("Could not create nKernelServer object!\n");
    }
    return retval;
}
