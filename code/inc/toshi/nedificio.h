#ifndef N_EDIFICIO_H
#define N_EDIFICIO_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_ENTITY_H
#include "toshi/nentity.h"
#endif

#undef N_DEFINES
#define N_DEFINES nEdificio
#include "kernel/ndefdllclass.h"

#define TIEMPO_SELECCION 0.3f

//------------------------------------------------------------------------------
class nEdificio : public nEntity
{
public:
	enum TEdificio {
		T_CONSTRUCCION,
		T_AYUNTAMIENTO,
		T_CASAMONEDA,
		T_CASAPLANOS
	};
    /// constructor
    nEdificio();
    /// destructor
    virtual ~nEdificio();
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);
	// Identificador del edificio
	virtual void SetId(int id);
	// Tipo de edificio: 0 Construccion 1 Ayuntamiento 2 Casa Moneda 3 Casa Planos
	virtual void SetTipo(TEdificio tipo);
	// Posición del edificio
	virtual void SetPos(float x, float y);
	// Crear el edificio y pintarlo
	virtual void Pintar();
	// Actualiza el estado del edificio
	virtual void Tick(float dt);
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
private:
	float		edificio_x;
	float		edificio_y;
	TEdificio	tipo_edificio;
	int			id_edificio;
};
//------------------------------------------------------------------------------
#endif