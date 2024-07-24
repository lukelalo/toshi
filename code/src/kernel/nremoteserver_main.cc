#define N_IMPLEMENTS nRemoteServer
#define N_KERNEL
//------------------------------------------------------------------------------
//  nremoteserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nremoteserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nloghandler.h"

/*#undef n_init
#undef n_fini
#undef n_create
#define n_init   nremoteserver_init
#define n_fini   nremoteserver_fini
#define n_create nremoteserver_create

nNebulaClass(nRemoteServer, "nroot");*/

//------------------------------------------------------------------------------
/**
*/
nRemoteServer::nRemoteServer() :
    refScriptServer(kernelServer, this),
    isOpen(false),
    ipcServer(0)
{
    this->refScriptServer = "/sys/servers/script";
}

//------------------------------------------------------------------------------
/**
*/
nRemoteServer::~nRemoteServer()
{
    if (this->isOpen)
    {
        this->Close();
    }
    n_assert(0 == this->ipcServer);
}

//------------------------------------------------------------------------------
/**
*/
bool
nRemoteServer::Open(const char* portName)
{
    n_assert(!this->isOpen);
    n_assert(portName);
    n_assert(0 == this->ipcServer);

#ifdef __WIN32__
    #ifdef __XBxX__
        xbNetStartup();
    #endif    
    struct WSAData wsa_data;
    WSAStartup(0x101, &wsa_data);
#endif

    // create an ipc server object
    this->ipcServer = n_new nIpcServer(portName);
    n_assert(this->ipcServer);

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nRemoteServer::Close()
{
    n_assert(this->isOpen);
    n_assert(this->ipcServer);

    // release all client context objects
    nClientContext* curContext;
    while ((curContext = (nClientContext*) this->clientContexts.RemHead()))
    {
        n_delete curContext;
    }

    // kill ipc server object
    n_delete this->ipcServer;

    this->ipcServer = 0;
    this->isOpen = false;

#ifdef __WIN32__
    #ifdef __XBxX__
        xbNetShutdown();
    #endif
    WSACleanup();
#endif

}

//------------------------------------------------------------------------------
/**
*/
void
nRemoteServer::Broadcast(const char* str)
{
    n_assert(str);
    if (this->isOpen)
    {
        n_assert(this->ipcServer);
        this->ipcServer->BroadcastMsg((void*) str, strlen(str) + 1);
    }
}

//------------------------------------------------------------------------------
/**
    Find client context matching the given client id. If no matching client
    context exists, create and initialize a new one.
*/
nRemoteServer::nClientContext*
nRemoteServer::GetClientContext(int clientId)
{
    n_assert(-1 != clientId);

    // find existing client context
    nClientContext* curContext;
    for (curContext = (nClientContext*) this->clientContexts.GetHead();
         curContext;
         curContext = (nClientContext*) curContext->GetSucc())
    {
        if (clientId == curContext->GetClientId())
        {
            return curContext;
        }
    }

    // fallthrough: create a new client context
    n_printf("nRemoteServer: creating new client context!\n");
    nClientContext* newContext = n_new nClientContext(clientId, this);
    n_assert(newContext);
    newContext->SetCwd(kernelServer->GetCwd());
    
    this->clientContexts.AddTail(newContext);
    return newContext;
}

//------------------------------------------------------------------------------
/**
    Return the cwd object for the client id (creates a new client context
    if no matching context exists yet). If the client object's cwd no longer
    exists, return the kernel's current cwd.

    @param  clientId    a client id from nIpcServer::GetMsg()
    @return             a cwd object (always valid)
*/
nRoot*
nRemoteServer::GetClientCwd(int clientId)
{
    nClientContext* context = this->GetClientContext(clientId);
    nRoot* cwd = context->GetCwd();
    if (!cwd)
    {
        // old cwd has vanished, replace with kernelserver cwd
        cwd = kernelServer->GetCwd();
        context->SetCwd(cwd);
    }
    return cwd;
}

//------------------------------------------------------------------------------
/**
    Update the cwd in a client context defined by its cliend id.
*/
void
nRemoteServer::SetClientCwd(int clientId, nRoot* cwd)
{
    nClientContext* context = this->GetClientContext(clientId);
    context->SetCwd(cwd);
}

//------------------------------------------------------------------------------
/**
    Process pending messages. Should either be called frequently (ie once
    per frame), or after a call to WaitMsg().

    TODO: implement quitRequested functionality

    @return     true if a client has requested the remote server to quit
*/
bool
nRemoteServer::Trigger()
{
    char prompt_buf[1024];
    nLineBuffer *line_array;
    int head_line_before;
    nString output;
    char *prompt = "prompt:";
    int len_prompt = strlen(prompt);


    if (this->isOpen)
    {
        n_assert(this->ipcServer);
        nScriptServer* scriptServer = this->refScriptServer.get();

        // turn off FailOnError mode in script server
        bool origFailOnError = scriptServer->GetFailOnError();
        scriptServer->SetFailOnError(false);

        // poll ipc server and check if any messages are pending
        if (this->ipcServer->Poll())
        {
            // for each pending message...
            nMsgNode* msg;
            int msgClientId;
            while ((msg = this->ipcServer->GetMsg(msgClientId)))
            {
                // make sure the message is a valid string
                const char* msgPtr = (const char*) msg->GetMsgPtr();
                int msgSize  = msg->GetMsgSize();
                n_assert(msgPtr);
                if ((msgSize > 0) && (msgPtr[msgSize - 1] == 0))
                {
                    // set the client's cwd
                    kernelServer->PushCwd(this->GetClientCwd(msgClientId));
                    const char* result = 0;
                    output.Set(0);

                    // Handle the prompt
                    if (strncmp(msgPtr,prompt,len_prompt)==0) 
                    {
                        //1024 is the contract size with the script servers
                        //for the size of the prompt buffer
                        result = scriptServer->Prompt(prompt_buf, 1024);
                        output.Append(result);
                    }
                    else
                    {
                        //Get the index of the head line
                        line_array = this->kernelServer->GetDefaultLogHandler()->GetLineBuffer();

                        //Make sure we are starting on a fresh line
                        //Some script servers are ill behaved
                        if (line_array->GetLine(line_array->GetHeadLine())[0] )
                        {
                            line_array->Put("\n");
                        }
                        head_line_before = line_array->GetHeadLine();

                        // this seems to be a valid message, send the message
                        // to the scriptserver for validation
                        scriptServer->Run(msgPtr, result);
                        //export the result to the line buffer
                        if (result[0])
                        {
                            //Make sure we put this on a new line
                            if (line_array->GetLine(line_array->GetHeadLine())[0] )
                            {
                                line_array->Put("\n");
                            }
                            line_array->Put(result);
                        }
                        int i = head_line_before;
                        const char* line_buf;
                        while (i != -1)
                        {
                            line_buf = line_array->GetLine(i);
                            if( line_buf[0])
                            {
                                output.Append(line_buf);
                                output.Append("\n"); //line buffer eats \r and \n
                            }
                            i = line_array->GetNextLine(i);
                        }
                    }
                    result = output.Get();
                    // release the msg node
                    this->ipcServer->ReplyMsg(msg);

                    // send the result string back to the client
                    const char* answerMsg = "";
                    if (result && (result[0] != 0))
                    {
                        answerMsg = result;
                    }
                    this->ipcServer->AnswerMsg((void*)answerMsg, strlen(answerMsg) + 1, msgClientId);

                    // store new cwd in client context
                    this->SetClientCwd(msgClientId, kernelServer->GetCwd());

                    // restore the original cwd
                    kernelServer->PopCwd();
                }
                else
                {
                    n_printf("nRemoteServer: BROKEN MESSAGE RECEIVED!\n");
                    const char* msg = "### comm error!";
                    this->ipcServer->AnswerMsg((void*) msg, strlen(msg) + 1, msgClientId);
                }
            }
        }

        // restore fail on error mode
        scriptServer->SetFailOnError(origFailOnError);
    }
    return false;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
