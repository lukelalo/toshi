#define N_IMPLEMENTS nMLoader
//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "MLoader/nMLoader.h"
#include "HalfLifeMDL/MDLReader.h"
#include <string>
#include <cstdlib>
#include <cstdio>
#include "mathlib/vector.h"
#define MAX_VERT 10000

using namespace HalfLifeMDL;

#define for	if(true) for

nNebulaScriptClass(nMLoader, "nvisnode");

//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------
/**
 * Constructor de la clase. Inicializa todas las variables de la clase: 
 * establece las rutas de los archivos de los que se leerán los modelos,
 * carga los buffers de vértices e índices y establece la secuencia a pintar.
 */
nMLoader::nMLoader() : nVisNode(),
	ref_vb(NULL), ref_ibuf(NULL), tex_array(NULL), shader(NULL),
	_core(NULL), _instance(NULL), refFileServer(kernelServer, this),
	refMDLRepository(kernelServer, this), ref_attach(kernelServer, this),
	rutaAttach(""), numeroAttach (-1), scale(0.02) {
	refFileServer	 = "/sys/servers/file2";
	refMDLRepository = "/sys/servers/mdlRepository";
}
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
nMLoader::~nMLoader() {
	ReleaseTextures();
	Release3D();
	ReleaseHLModel();
}
//------------------------------------------------------------------------------
void nMLoader::SetFileName(const char *nombreFichero) {
	n_assert(nombreFichero);

	// TODO: ¿Comprobar si el nombre es el mismo que el que ya teníamos?
//	ReleaseAll();

	// Establecemos el nuevo nombre.
	rsrc_path.Set(refFileServer.get(), nombreFichero, refFileServer->GetCwd());
	fileName = nombreFichero;
	loadMDL();
}

const char *nMLoader::GetFileName() const {
	return rsrc_path.GetAbsPath(); // GetPath();
}

//------------------------------------------------------------------------------
void nMLoader::ReleaseHLModel() {

	// TODO!!!

}

//------------------------------------------------------------------------------
void nMLoader::Release3D() {

	if (ref_vb) {
		for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {
			if (ref_vb[i].isvalid()) {
				ref_vb[i]->Release(); ref_vb[i].invalidate();
			}
		}
		delete []ref_vb;
		ref_vb = NULL;
	}
	if (ref_ibuf) {
		for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {
			if (ref_ibuf[i].isvalid()) {
				ref_ibuf[i]->Release(); ref_ibuf[i].invalidate();
			}
		}
		delete []ref_ibuf;
		ref_ibuf = NULL;
	}

}

//------------------------------------------------------------------------------
void nMLoader::ReleaseTextures() {

	if (tex_array) {
		for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {
			if (tex_array[i]) {
				delete tex_array[i]; // Supongo que borrará la textura
									// de memoria... :-?
				tex_array[i] = NULL;
			}
		}
		delete [] tex_array;
	}

	if (shader) {
		shader->Release();
		shader = NULL;
	}
}

//------------------------------------------------------------------------------
/**
 * Método que añade este nodo a la escena.
 */
bool nMLoader::Attach(nSceneGraph2 *sceneGraph){

	if (nVisNode::Attach(sceneGraph)) {
		sceneGraph->AttachVisualNode(this);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
/**
 * Método al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
 * que se le han ido añadiendo para actualizar sus valores.
 */
void nMLoader::Compute(nSceneGraph2 *sceneGraph) {
	nVisNode::Compute(sceneGraph);

	// ¿¿Hay nombre de fichero??
	if (rsrc_path.GetPath()) {
		// (re)-load mesh on demand
		if (!ref_vb) {
			if (!loadMDL())
				return;
		}
	} else
		return;
	//if (!vb)
	//	vb=ref_vb[0].get();
/*
	matrix44 m = sceneGraph->GetTransform();
	refGfx.get()->SetMatrix(N_MXM_MODELVIEW, m);
*/
	//_instance->update(0);
	// TODO: Ponerlo con canales!!
	long  time = GetTickCount();
	_instance->update((float)time/1000);
/*
            // get current anim channel value
            nChannelContext* chnContext = sceneGraph->GetChannelContext();
            n_assert(chnContext);
            float t = chnContext->GetChannel1f(this->localChannelIndex);
*/
	if (_instance->getNumMeshes() > 0) {
		// Dibujamos.
		for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {
			_instance->selectMesh(i);
			// Rellenamos los vértices y normales.
			unsigned int numVertices = _instance->getNumVertices();
			const float *arrayVert = _instance->getVertices();
			const float *arrayNormales = _instance->getNormals();
			nRef<nVertexBuffer> *r_vb = &(ref_vb[i]);
			if (!vb) continue;
			if (!r_vb->isvalid()) continue;
			vb = r_vb->get();
			vb->LockVertices();
			if (scale == 1.0) {
				for (int j = 0; j < numVertices; j++) {
					vb->Coord(j, vector3(*arrayVert, 
										 *(arrayVert+1), 
										 *(arrayVert+2)));
					vb->Norm(j, vector3(*arrayNormales, 
										*(arrayNormales+1), 
										*(arrayNormales+2)));
					arrayNormales += 3;
					arrayVert += 3;
				}
			} else {
				for (int j = 0; j < numVertices; j++) {
					vb->Coord(j, vector3(*arrayVert * scale, 
										 *(arrayVert+1) * scale, 
										 *(arrayVert+2) * scale));
					vb->Norm(j, vector3(*arrayNormales, 
										*(arrayNormales+1), 
										*(arrayNormales+2)));
					arrayNormales += 3;
					arrayVert += 3;
				}
			}
			vb->UnlockVertices();
			// Dibujamos la malla.
			/*sceneGraph->SetVertexBuffer(vb->GetVertexBubble());
			sceneGraph->SetIndexBuffer(vb->G);*/
			if (tex_array[i])
				vb->Render(ref_ibuf[i].get(), sceneGraph->GetPixelShader() , tex_array[i]);  //sceneGraph->GetPixelShader()
			else
				vb->Render(ref_ibuf[i].get(), sceneGraph->GetPixelShader(), sceneGraph->GetTextureArray()); //sceneGraph->GetPixelShader()
			/*sceneGraph->SetVertexBuffer(vb);
			sceneGraph->SetIndexBuffer(ref_ibuf[i].get());
			*/
			

			
		}
	}


	// Y ahora dibujamos el attach, si lo hay...
	/*if (numeroAttach != -1) {

		refGfx.get()->PushMatrix(N_MXM_MODELVIEW);
		MDLPoint pos, puntoX, puntoY, puntoZ;
		MDLPoint huesoRaiz;
		ref_attach->_instance->getBonePos(0, huesoRaiz);
		_instance->getAttachment(numeroAttach, puntoX, puntoY, puntoZ, pos);
		matrix44 nmatriz(
			vector4(puntoX[0],puntoX[1],puntoX[2],0),
			vector4(puntoY[0],puntoY[1],puntoY[2],0),
			vector4(puntoZ[0],puntoZ[1],puntoZ[2],0),
			vector4(pos[0],pos[1],pos[2],1)
		);
		matrix44 mat;
		refGfx.get()->GetMatrix(N_MXM_MODELVIEW,mat);
		nmatriz.translate(-huesoRaiz[0],-huesoRaiz[1], -huesoRaiz[2]);
		mat=nmatriz * mat;
		refGfx.get()->SetMatrix(N_MXM_MODELVIEW,mat);

		ref_attach->Compute(sceneGraph);
		
		refGfx.get()->PopMatrix(N_MXM_MODELVIEW);

	}*/
}
//----------------------------------------------------------------------------------------

HalfLifeMDL::MDLInstance *nMLoader::getInstance() {
	return _instance;
}

// Carga 'dura': ha cambiado el nombre del fichero, o el recubrimiento.
bool nMLoader::loadMDL() {

	const char *fileName = rsrc_path.GetAbsPath();
	_core = refMDLRepository->getModelo(fileName);
	if (!_core)
		return false;

	if (!_instance) {
		_instance = new HalfLifeMDL::MDLInstance();
	}
	_instance->attachMDLCore(_core);

	// El core está cargado. Sacamos el instance.
	return cacheInstance();
}

// Ha cambiado la piel del modelo; únicamente hay que cambiar las texturas.
bool nMLoader::cacheInstance() {

	// Creamos el vertex buffer y el index buffer para cada malla.
	ref_vb = new nRef<nVertexBuffer>[_instance->getNumMeshes()];
	ref_ibuf = new nRef<nIndexBuffer>[_instance->getNumMeshes()];

	for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {
		ref_vb[i] = NULL; ref_ibuf[i] = NULL;
	}

	// Rellenamos los datos de cada uno. Hay que tener en cuenta que
	// los buffer de índices no varían con cada animación, y que
	// de los buffers de vértices, únicamente cambia la posición del vértice.
	// Los buffers serán únicos para el sistema, por lo tanto, utilizamos
	// como nombre el this del objeto.
	for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {

		char	rsrc_name[N_MAXPATH];	// Nombre de los recursos para esta malla.
		sprintf(rsrc_name, "mdl%X@%d", (int)this, i);

		// Cargamos los datos de esa malla.
		if (!cacheMesh(i, rsrc_name))
			return false;

	}

	if (!cacheTextures())
		return false;

	return true;
}

// Ha cambiado la piel del modelo; únicamente hay que cambiar las texturas.
bool nMLoader::cacheTextures() {
	// Cargamos las texturas
	tex_array = new nTextureArray*[_instance->getNumMeshes()];

	for (unsigned int i = 0; i < _instance->getNumMeshes(); i++) {
		nTextureArray *nTA;
		nTA = new nTextureArray(this);

		// Probamos a cargar la textura del fichero origen.
		bool cargada;
		int numTextura = _instance->getTextureIndex();
		char buf[N_MAXNAMELEN];
		sprintf(buf, "%d@%s", i, fileName.c_str());
		n_assert(strlen(buf) < N_MAXNAMELEN);
		cargada = nTA->SetTexture(refGfx.get(), 0, buf, NULL, false, false);
		if (!cargada) {
			buf[strlen(buf)-4] = '\0';
			sprintf(buf, "%sT.mdl", buf);		
			cargada = nTA->SetTexture(refGfx.get(), 0, buf, NULL, false, false);
			if (!cargada) {
				n_printf("No se encuentra la textura %s.\n", buf);
				delete nTA;
				nTA = NULL;
			}
		}
		tex_array[i] = nTA;
	}

	// Conseguimos el shader de vértices.
	if (!shader) {
		shader = refGfx.get()->FindPixelShader("mdlPixelShader");
		if (shader)
			shader->AddRef();
		else {
			// Construimos el shader.
			shader = refGfx->NewPixelShader("mdlPixelShader");
			nPixelShaderDesc *desc = new nPixelShaderDesc();
			desc->SetNumStages(1);
			/*
			desc->SetColorOp(0, nPSI::nOp::REPLACE,
							 nPSI::nArg::TEX, nPSI::nArg::NOARG,
							 nPSI::nArg::NOARG,
							 nPSI::nScale::ONE);*/
			desc->SetColorOp(0, nPSI::nOp::MUL,
							 nPSI::nArg::TEX, nPSI::nArg::PREV,
							 nPSI::nArg::NOARG,
							 nPSI::nScale::ONE);
			desc->SetAlphaOp(0, nPSI::nOp::NOP, nPSI::nArg::NOARG,
							 nPSI::nArg::NOARG, nPSI::nArg::NOARG,
							 nPSI::nScale::ONE);
			desc->SetConst(0, vector4(0, 0, 0, 0));
			desc->SetAddressU(0, N_TADDR_WRAP);
			desc->SetAddressV(0, N_TADDR_WRAP);
			desc->SetMinFilter(0, N_TFILTER_LINEAR_MIPMAP_NEAREST);
			desc->SetMagFilter(0, N_TFILTER_LINEAR);
			desc->SetTexCoordSrc(0, N_TCOORDSRC_UV0);
			desc->SetEnableTransform(0, false);
			desc->Txyz(0, vector3(0, 0, 0));
			desc->Rxyz(0, vector3(0, 0, 0));
			desc->Sxyz(0, vector3(1, 1, 1));
				desc->SetLightEnable(true);
				desc->SetDiffuse(vector4(1.0, 1.0, 1.0, 1.0));
				desc->SetAmbient(vector4(1.0, 1.0, 1.0, 1.0));
			shader->SetShaderDesc(desc);
		}
	}


	return false;
}

bool nMLoader::cacheMesh(int numMesh, const char *nombreRecurso) 
{
	nVertexBuffer *vertexBuffer = NULL;
	nIndexBuffer *indexBuffer = NULL;
	int numFaces;
	int numVertex;
	int numIndices;
	int i;
	HalfLifeMDL::MDLMesh::MeshType tipoMalla;
	nPrimType ibPrimType;

	tipoMalla = _instance->selectMesh(numMesh);
	switch (tipoMalla) {
		case HalfLifeMDL::MDLMesh::MeshType::MT_Triangles:
			ibPrimType = N_PTYPE_TRIANGLE_LIST;
			break;
		case HalfLifeMDL::MDLMesh::MeshType::MT_Strip:
			ibPrimType = N_PTYPE_TRIANGLE_STRIP;
			break;
		case HalfLifeMDL::MDLMesh::MeshType::MT_Fan:
			ibPrimType = N_PTYPE_TRIANGLE_FAN;
			break;
	};

	// Creamos el buffer de vértices.
	numVertex = _instance->getNumVertices();
	nVBufType vbufType = N_VBTYPE_STATIC; // readOnly ? N_VBTYPE_READONLY : N_VBTYPE_STATIC;
	int vertexType = N_VT_COORD | N_VT_NORM | N_VT_UV0;
	vertexBuffer = refGfx.get()->NewVertexBuffer(nombreRecurso,
							vbufType, vertexType, numVertex);
	n_assert(vertexBuffer);
	// Rellenamos lo que es constante: coordenada de textura.
	const float *coordTextura = _instance->getTextureCoords();
	vertexBuffer->LockVertices();
	for (unsigned int i = 0; i < numVertex; i++) {
		vertexBuffer->Uv(i, 0, vector2(*coordTextura, *(coordTextura + 1)));
		coordTextura += 2;
	}
	vertexBuffer->UnlockVertices();
	
	// Creamos el búffer de índices
	numIndices = _instance->getNumVertexIndices();
	indexBuffer = refGfx.get()->NewIndexBuffer(nombreRecurso);
	n_assert(indexBuffer);
	// Lo rellenamos
	const unsigned int *indices = _instance->getVertexIndices();
	indexBuffer->Begin(N_IBTYPE_STATIC, ibPrimType, numIndices);
	for (unsigned int i = 0; i < numIndices; i++) {
		indexBuffer->Index(i, *indices);
		indices++;
	}

	indexBuffer->End();

	ref_vb[numMesh] = vertexBuffer;
//	vertexBuffer->AddRef();	// Cuando se crea, se incrementa la referencia ;)
	ref_ibuf[numMesh] = indexBuffer;
//	indexBuffer->AddRef();

	return true;
}

void nMLoader::SetAnimation(int numAnim) {
	assert(_instance);
	_instance->setSequenceByIndex(numAnim);
}

const char *nMLoader::GetAnimation() const {
	assert(_instance);

	const HalfLifeMDL::MDLSequence *seq;
	seq = _instance->getSequence();

	return seq->getName().c_str();
}

/**
Pega un modelo al 'attach' del modelo dado.
@param numHueso Número del attach donde se enganchará.
@param ruta Ruta donde está el modelo que se dibujará.
@return Devuelve cierto si se ha podido realizar con éxito la operación.
@note Únicamente puede haber un fichero pegado al modelo, independientemente
de cuántas posiciones para attach posibles tenga. Por otro lado, la ruta
se supone que apuntará a un objeto (nMLoader también) que NO se está dibujando
por otro lado (es decir, no cuelga de /usr/scene por ejemplo).
*/
bool nMLoader::AttachModel(int numAttach, const std::string &ruta) {

	static nClass *estaClase = kernelServer->FindClass("nMLoader");

	if ((numAttach < 0) || (numAttach >= _instance->getNumAttachments()))
		return false;

	nRoot *entidad = kernelServer->Lookup(ruta.c_str());
/*
	if (!entidad->IsInstanceOf(estaClase))
		return false;
*/
	rutaAttach = ruta;

	numeroAttach = numAttach;

	ref_attach = rutaAttach.c_str();

	return true;
}

/**
Despega el objeto adjunto.
*/
void nMLoader::DetachModel() {

	if (numeroAttach == -1)
		return;

	numeroAttach = -1;
}


	
/*

  NOTAS: Con recubrimiento y piel constante, en cada momento, lo único que cambia
  de lo que hay que dibujar es la posición de los vértices de cada maya.
  Cuando cambia la piel, cambian únicamente las texturas que se utilizan.
  Cuando cambia el recubrimiento, cambia todo totalmente.

	- Texturas: son globales al modelo, por lo que puede ser nombre_modelo+numero_textura
	- Buffer de vértices: el contenido varía en cada frame. El tamaño no cambia, si no
	cambia el recubrimiento.
	- Buffer de índices: el tamaño no cambia si no cambia el recubrimiento. El contenido tampoco.

*/
