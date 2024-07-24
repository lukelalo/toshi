#ifndef N_MLOADER_H
#define N_MLOADER_H
//--------------------------------------------------------------------------------------------------
/**
 *    @file nMLoader.h
 *
 *    @brief Esta clase se encarga de transformar los modelos le�dos del fichero mdl y empaquetados
 *	  en en una parte est�tica(MDLCore) y otra din�mica(MDLInstance), en las estructuras
 *	  que usar� Nebula para pintarlos. 
 *  
 *    @author Felicidad Ramos Manjavacas, Marco Antonio G�mez Mart�n
 *	  @date Febrero 2004.
 */
//--------------------------------------------------------------------------------------------------
#ifndef N_N3DNODE_H
#include "node/n3dnode.h"
#endif

#include "kernel/ndefdllclass.h"

#include "HalfLifeMDL/MDLCore.h"
#include "HalfLifeMDL/MDLReader.h"
#include "HalfLifeMDL/MDLInstance.h"
#include "MLoader/nMDLRepository.h"


#include "gfx/npixelshader.h"
#include "kernel/nautoref.h"
#include "gfx/nscenegraph2.h"
#include "gfx/ntexturearray.h"
#include "node/nshadernode.h"
#include "node/nmeshnode.h"
#include "node/ntexarraynode.h"
#include "gfx/ndynvertexbuffer.h"
#include <vector>
#include <map>
#include <deque>
#include "time.h"
#include <functional>

#undef N_DEFINES
#define N_DEFINES nMLoader
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
/**
 *    Esta clase se encarga de transformar los modelos le�dos del fichero mdl y empaquetados
 *	  en en una parte est�tica(MDLCore) y otra din�mica(MDLInstance), en las estructuras
 *	  que usar� Nebula para pintarlos. 
 *  
 *    @author Marco Antonio G�mez Mart�n, Felicidad Ramos Manjavacas
 *	  @date Febrero 2004.
 */
class N_PUBLIC nMLoader : public nVisNode
{
private:
	 nVertexBuffer *vb;
public:
    nMLoader();			//Constructor
    
    virtual ~nMLoader();	//Destructor

	void SetFileName(const char *nombreFichero);
	const char *GetFileName() const;

	void SetAnimation(int numAnim);
	const char *GetAnimation() const;

    virtual bool SaveCmds(nPersistServer* persistServer);	//Persistencia

    static nKernelServer* kernelServer;	//Puntero al kernelServer

	bool Attach(nSceneGraph2 *sceneGraph);
	void Compute(nSceneGraph2 *sceneGraph);

	HalfLifeMDL::MDLInstance *getInstance();

	// ----- FUNCIONES RELATIVAS A LOS ATTACH -----

	/**
	Devuelve el n�mero de posiciones posibles que tiene el modelo
	donde enganchar elementos.
	*/
	int getNumAttachTotales() { return _instance->getNumAttachments();}

	/**
	Pega un modelo al 'attach' del modelo dado.
	@param numHueso N�mero del attach donde se enganchar�.
	@param ruta Ruta donde est� el modelo que se dibujar�.
	@return Devuelve cierto si se ha podido realizar con �xito la operaci�n.
	@note �nicamente puede haber un fichero pegado al modelo, independientemente
	de cu�ntas posiciones para attach posibles tenga. Por otro lado, la ruta
	se supone que apuntar� a un objeto (nMLoader tambi�n) que NO se est� dibujando
	por otro lado (es decir, no cuelga de /usr/scene por ejemplo).
	*/
	bool AttachModel(int numAttach, const std::string &ruta);

	/**
	Despega el objeto adjunto.
	*/
	void DetachModel();

	/**
	Devuelve el n�mero del attach en el que hay enganchado un modelo.
	@return En caso de no haber nada pegado, devuelve -1.
	*/
	int getNumeroAttach() {return numeroAttach;}

	/**
	Devuelve la ruta donde se encuentra el modelo adjunto a este.
	@return Devolver� la cadena vac�a si no hay modelo adjunto.
	*/
	const std::string &getNombreAttach() { return rutaAttach; }

	float scale;

protected:
	std::string	rutaAttach;

	int numeroAttach;

	// Puntero al objeto que est� 'adjunto' (attached)
	nAutoRef<nMLoader> ref_attach;

	// ------- FIN FUNCIONES RELATIVAS A ATTACH -----

protected:
    nAutoRef<nFileServer2>  refFileServer;
	nAutoRef<nMDLRepository> refMDLRepository;

	// Buffers de v�rtices; uno por cada malla. Son locales al modelo.
	nRef<nVertexBuffer> *ref_vb;
	
	// B�ffers de �ndices; uno por cada malla. Son locales al modelo.
	nRef<nIndexBuffer> *ref_ibuf;

	// Array de punteros a los objetos que guardan el array de textura de
	// cada malla. La textura asignada es global.
	nTextureArray	**tex_array;

	// Pixel shader. Es global a todos los modelos ("mdlPixelShader").
	nPixelShader	*shader;

	/**
	   Path to the .MDL file.
	*/
    nRsrcPath               rsrc_path;

	/**
		Nombre del fichero
	*/
	std::string fileName;

	/**
	Carga 'dura': ha cambiado el nombre del fichero, o el recubrimiento.
	*/
	bool loadMDL();

	/**
	Se ha cambiado alguna propiedad de la instancia del modelo, se recargan
	los buffers de v�rtices, etc. as� como las texturas (es decir,
	llama a cacheTextures()).
	*/
	bool cacheInstance();

	/**
	Ha cambiado la piel del modelo; �nicamente hay que cambiar las texturas,
	y tambi�n se crea el Pixel Shader, si no estaba ya creado.
	*/
	bool cacheTextures();

	/**
	Instancia del modelo que se est� dibujando.
	*/
	HalfLifeMDL::MDLInstance *_instance;

	/**
	Core del modelo que se dibuja.
	*/
	HalfLifeMDL::MDLCore *_core;

private:
	/**
	Elimina toda la informaci�n del modelo (_core e _instance).
	*/
	void ReleaseHLModel();

	/**
	Elimina toda la informaci�n de mallas 3D (vertex buffers e
	index buffers).
	*/
	void Release3D();

	/**
	Elimina toda la informaci�n de texturas.
	*/
	void ReleaseTextures();

	bool cacheMesh(int numMesh, const char *nameRecurso);
};
//------------------------------------------------------------------------------

/*

  NOTAS: Con recubrimiento y piel constante, en cada momento, lo �nico que cambia
  de lo que hay que dibujar es la posici�n de los v�rtices de cada maya.
  Cuando cambia la piel, cambian �nicamente las texturas que se utilizan.
  Cuando cambia el recubrimiento, cambia todo totalmente.

	- Texturas: son globales al modelo, por lo que puede ser nombre_modelo+numero_textura
	- Buffer de v�rtices: el contenido var�a en cada frame. El tama�o no cambia, si no
	cambia el recubrimiento.
	- Buffer de �ndices: el tama�o no cambia si no cambia el recubrimiento. El contenido tampoco.

*/

#endif
