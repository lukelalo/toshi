#define N_IMPLEMENTS nIACONAESTRELLA

#include "toshi/niaconaestrella.h"

nNebulaScriptClass( nIACONAESTRELLA, "nia" );

// Constructor general
nIACONAESTRELLA::nIACONAESTRELLA() : refScriptServer( kernelServer, this )
{
		InitializeCLIPS();
		refScriptServer = "/sys/servers/script";
		sprintf(iaScript, "");
}

// Destructor
nIACONAESTRELLA::~nIACONAESTRELLA()
{
}


void nIACONAESTRELLA::Trigger()
{
	if ( iaScript[0] == '\0' ) // strlen(iaScript) == 0)
		return;
}