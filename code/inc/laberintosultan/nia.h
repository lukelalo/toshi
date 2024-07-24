#ifndef N_IA_H
#define N_IA_H
//------------------------------------------------------------------------------
/**
    @class nIA

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

#include "kernel/nscriptserver.h"

#undef N_DEFINES
#define N_DEFINES nIA
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class N_PUBLIC nIA : public nRoot
{
public:
    /// constructor
	nIA();
    /// destructor
    virtual ~nIA();
    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

	// Trigger. Realiza la IA. Esta clase básica, lo que hace es
	// llamar al script que tiene asociado (si tiene alguno...)
	virtual void Trigger();

	// Funciones para acceso al script.
	inline const char *GetIAScript() const;
	void SetIAScript(const char *script);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

private:
	nAutoRef<nScriptServer> refScriptServer;

	char iaScript[N_MAXPATH];
};

//---------------------------------------------------------------------------
inline const char *nIA::GetIAScript() const {
	return iaScript;
}
#endif
