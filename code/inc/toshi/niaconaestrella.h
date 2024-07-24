#ifndef N_IACONAESTRELLA_H
#define N_IACONAESTRELLA_H
//------------------------------------------------------------------------------
/**
    @class nIACONAESTRELLA

    @brief Clase que representa un controlador de inteligencia artificial.

	Es llamado por el bucle principal (nGame), a intervalos constantes.
	Permite enganchar un script, que se ejecuta cada vez. Para comportamientos
	específicos programados en C++, habrá que heredar de esta clase.

*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_IA_H
#include "toshi/nIA.h"
#endif

#include "kernel/nscriptserver.h"

#include "toshi/AStar.h"

extern "C" {
#include "CLIPS/clips.h"
}

#undef N_DEFINES
#define N_DEFINES nIACONAESTRELLA
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class N_PUBLIC nIACONAESTRELLA : public nIA
{
public:
    /// constructor
	nIACONAESTRELLA();
    /// destructor
    virtual ~nIACONAESTRELLA();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

	// Trigger. Realiza la IA. Esta clase básica, lo que hace es
	// llamar al script que tiene asociado (si tiene alguno...)
	virtual void Trigger();

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
	CAStar *path;
private:
	nAutoRef<nScriptServer> refScriptServer;

	char iaScript[N_MAXPATH];
};

//---------------------------------------------------------------------------
#endif
