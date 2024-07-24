#define N_IMPLEMENTS nIA

#include "toshi/nia.h"

nNebulaScriptClass(nIA, "nroot");

//------------------------------------------------------------------------------
/**
*/
nIA::nIA() :
	refScriptServer(kernelServer, this)
	{
		refScriptServer = "/sys/servers/script";
		sprintf(iaScript, "");
	}

nIA::~nIA() {
}

void nIA::SetIAScript(const char *script) {
	n_assert(strlen(script) < N_MAXPATH);
	sprintf(iaScript, "%s", script);
}


void nIA::Trigger() {

	if (iaScript[0] == '\0') // strlen(iaScript) == 0)
		return;

	n_assert(refScriptServer.isvalid());

	const char *result;

	refScriptServer->Run(iaScript, result);
}