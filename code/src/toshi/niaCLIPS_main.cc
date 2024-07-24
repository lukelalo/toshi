#define N_IMPLEMENTS nIACLIPS

#include "toshi/niaCLIPS.h"

nNebulaScriptClass(nIACLIPS, "nia");

//------------------------------------------------------------------------------
/**
*/
nIACLIPS::nIACLIPS() :
	refScriptServer(kernelServer, this),
	refFileServer(kernelServer, this)
	{
		InitializeEnvironment();
		entorno = CreateEnvironment();
		refScriptServer = "/sys/servers/script";
		refFileServer = "/sys/servers/file2";
		sprintf(CLIPSScript, "");
	}

nIACLIPS::~nIACLIPS() {
	DestroyEnvironment(entorno);
}

void nIACLIPS::SetCLIPSScript(const char *script) {
	n_assert(strlen(script) < N_MAXPATH);
	char buf[N_MAXPATH];
	refFileServer->ManglePath(script,buf,N_MAXPATH);
	sprintf(CLIPSScript, "%s", buf);
	EnvLoad(entorno,CLIPSScript);
	EnvReset(entorno);
	EnvRun(entorno,-1);
}


void nIACLIPS::Trigger() {

	if (CLIPSScript[0] == '\0') // strlen(iaScript) == 0)
		return;
/*	
	n_assert(refScriptServer.isvalid());

	const char *result;
	refScriptServer->Run(iaScript, result);
*/
}