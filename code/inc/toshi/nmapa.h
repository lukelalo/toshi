#ifndef N_MAPA_H
#define N_MAPA_H
//------------------------------------------------------------------------------
/**
    @class nMapa

    @brief clase que permite cargar un gráfico de mapa y almacena la escena	

    Contiene métodos para cargar y visualizar el mapa, así como para acceder a 
	cada nodo del mapa y devolver la información que contiene.

    (C)	2005	Pei
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#ifndef N_ASTAR_H
#include "toshi/AStar.h"
#include "toshi/ASIncludes.h"
#endif

#ifndef N_SCRIPTSERVER_H
#include "kernel/nscriptserver.h"
#endif

#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#include "file/nnpkfileserver.h"

#undef N_DEFINES
#define N_DEFINES nMapa
#include "kernel/ndefdllclass.h"

#define ANCHO_MAPA 80
#define ALTO_MAPA 80
#define NUM_DIVISIONES 1
//------------------------------------------------------------------------------
class nVisNode;
class n3DNode;
class nMeshNode;
class nShaderNode;
class nTexArrayNode;
class nCSprite;
class nSceneGraph2;
class nChannelServer;
class nEdificio;
class nArray;
class CAStar;

class nMapa : public nVisNode
{
public:
    enum Command
    {
        CMD_FORWARD,
        CMD_BACKWARD,
        CMD_LEFT,
        CMD_RIGHT
    };
	
public:
    /// constructor
    nMapa();
    /// destructor
    virtual ~nMapa();
    //virtual void Initialize();

    /// persistency
    virtual bool SaveCmds(nPersistServer* ps);

	inline int GetNumConstrucciones();
	inline void AgregarConstruccion();
	virtual bool PuedeConstruirEdificio(int posx, int posy);
	virtual void ConstruirEdificio(int posx, int posy);
	virtual void DestruirEdificio(float posx, float posy);

	virtual void PintarDebug(bool pintar);
	// Carga del fichero del mapa
    //void SetMap(const char* map_path);
	
	const char* GetMap();

	// Generación del mapa automática en función del número de jugadores
	void SetMap(int num_jugadores);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

	bool Attach(nSceneGraph2 *sceneGraph);
	void Compute(nSceneGraph2 *sceneGraph);

	CAStar *path;


	void BuscarRuta(int posXini,int posYini,int posXfin,int posYfin);
	static int Funcion_Coste( _asNode *param1, _asNode *param2, int data, void *cb);
	static int Funcion_Validez( _asNode *param1, _asNode *param2, int data, void *cb);
	bool Movimiento_Valido(int oX, int oY, int dX, int dY);
	int Coste_Movimiento(int oX, int oY, int dX, int dY);

protected:

private:

    nAutoRef<nChannelServer> refChannelServer;
	nAutoRef<nScriptServer> refScriptServer;
	nAutoRef<nNpkFileServer> refFileServer;
	nAutoRef<nSceneGraph2> sceneGraph;

	nString mapPath;

	// Almacenes de los mapas físico y lógico
	char *mapa_fisico[ALTO_MAPA];
	char *mapa_logico[ALTO_MAPA];
	nArray<nEdificio*> lista_edificios;

	int numConstrucciones;

	// Pinta el mapa en la escena
	void GenerarMapa();

    // Properties

    // Constants

};
//------------------------------------------------------------------------------
inline int nMapa::GetNumConstrucciones() {
	return this->numConstrucciones;
}

//------------------------------------------------------------------------------
inline void nMapa::AgregarConstruccion() {
	this->numConstrucciones++;
}

//------------------------------------------------------------------------------
#endif
