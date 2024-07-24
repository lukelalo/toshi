#ifndef N_IACLIPS_H
#define N_IACLIPS_H
//------------------------------------------------------------------------------
/**
    @class nIACLIPS

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
#include "file/nnpkfileserver.h"

#include "toshi/AStar.h"

extern "C" {
#include "CLIPS/clips.h"
}

#undef N_DEFINES
#define N_DEFINES nIACLIPS
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class N_PUBLIC nIACLIPS : public nIA
{
public:
    /// constructor
	nIACLIPS();
    /// destructor
    virtual ~nIACLIPS();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

	// Trigger. Realiza la IA. Esta clase básica, lo que hace es
	// llamar al script que tiene asociado (si tiene alguno...)
	virtual void Trigger();

	// Funciones para acceso al script.
	inline const char *GetCLIPSScript() const;
	void SetCLIPSScript(const char *script);
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
	CAStar *path;
private:
	nAutoRef<nScriptServer> refScriptServer;
	nAutoRef<nFileServer2> refFileServer;

	char CLIPSScript[N_MAXPATH];
	void *entorno;
};

//---------------------------------------------------------------------------
inline const char *nIACLIPS::GetCLIPSScript() const {
	return CLIPSScript;
}
#endif
