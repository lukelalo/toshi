#ifndef N_NET_H
#define N_NET_H
//------------------------------------------------------------------------------
/**
    @class nNet

    @brief Clase que representa un controlador de red

	Es llamado por el bucle principal (nGame), a intervalos constantes.

*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif


#undef N_DEFINES
#define N_DEFINES nNet
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------

class N_PUBLIC nNet: public nRoot
{
public:


	virtual void Trigger(){};

};

#endif
