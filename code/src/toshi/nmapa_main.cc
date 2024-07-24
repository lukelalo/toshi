#define N_IMPLEMENTS nMapa
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "gfx/nchannelserver.h"
#include "gfx/nscenegraph2.h"
#include "node/n3dnode.h"
#include "toshi/nmapa.h"
#include "toshi/nworld.h"
#include "toshi/nedificio.h"

nNebulaScriptClass(nMapa, "nroot");

//------------------------------------------------------------------------------
/**
*/

nMapa::nMapa() :
    refChannelServer(kernelServer, this),
    refFileServer(kernelServer, this),
	sceneGraph(kernelServer, this),
	refScriptServer(kernelServer, this),
	mapPath(""),
	lista_edificios()
{
    refChannelServer	= "/sys/servers/channel";
	refScriptServer		= "/sys/servers/script";
	refFileServer		= "/sys/servers/file2";
	sceneGraph			= "/sys/servers/sgraph2";
	numConstrucciones	= 0;
	path				= new CAStar();
	path->udCost		= Funcion_Coste;
	path->udValid		= Funcion_Validez;
	path->SetRows(ANCHO_MAPA*NUM_DIVISIONES);
}

//------------------------------------------------------------------------------
/**
*/
nMapa::~nMapa()
{
	n_free(path);
/*	for (int i=0;i<lista_edificios.Size();i++)
	{
		n_free( lista_edificios.At(i) );
	}
*/
	lista_edificios.Clear();
}
/*
void nMapa::SetMap(const char* map_path)
{
    n_assert(map_path);
	mapPath = map_path;
	nFile* fichero=NULL;
	char basura[ANCHO_MAPA+2];
	int num=0;
	fichero=refFileServer->NewFileObject();
    if (fichero->Open(map_path, "rb"))
    {
		for (int i=0;i<ALTO_MAPA;i++) {
			this->mapa_logico[i]=new char[ANCHO_MAPA];
			num = fichero->GetS(mapa_logico[i],ANCHO_MAPA);
			num = fichero->GetS(basura,2);
		}
		// Quitamos las dos líneas de separación del mapa lógico al mapa físico
		fichero->GetS(basura,ANCHO_MAPA+2);
		fichero->GetS(basura,ANCHO_MAPA+2);

		for (int i=0;i<ALTO_MAPA;i++) {
			this->mapa_fisico[i]=new char[ANCHO_MAPA];
			num = fichero->GetS(mapa_fisico[i],ANCHO_MAPA);
			num = fichero->GetS(basura,2);
		}
    }
    fichero->Close();
	GenerarMapa();
}
*/
void nMapa::SetMap(int num_jugadores)
{
	double x,y;
	// Generamos un mapa vacío
	for (int i=0;i<ALTO_MAPA;i++) 
	{
		this->mapa_logico[i]=new char[ANCHO_MAPA];
		this->mapa_fisico[i]=new char[ANCHO_MAPA];
		for (int j=0;j<ANCHO_MAPA;j++) 
		{
			this->mapa_logico[i][j]='#';
			this->mapa_fisico[i][j]='#';
		}
	}
	// Colocamos las casas de moneda y planos
	this->ConstruirEdificio(-1+(ANCHO_MAPA/2),1.5+(ALTO_MAPA/2));
	this->ConstruirEdificio(2+(ANCHO_MAPA/2),1.5+(ALTO_MAPA/2));
	this->mapa_fisico[(int)(ALTO_MAPA/2)][(int)(ANCHO_MAPA/2)-2]='M';
	this->mapa_fisico[(int)(ALTO_MAPA/2)][(int)(ANCHO_MAPA/2)+1]='P';
	// Colocamos los ayuntamientos
	double pedazo=360.0/(double)num_jugadores;
	for (int i=0;i<num_jugadores;i++) 
	{
		x=18.0*n_cos(n_deg2rad((double)i*pedazo));
		y=18.0*n_sin(n_deg2rad((double)i*pedazo));
		this->ConstruirEdificio((int)x+(ANCHO_MAPA/2),(int)y+(ALTO_MAPA/2));
		this->mapa_fisico[(int)(ALTO_MAPA/2)+(int)y-1][(int)(ANCHO_MAPA/2)+(int)x-1]='A';
	}
	GenerarMapa();
}

const char* nMapa::GetMap()
{
	return mapPath.Get();
}

void nMapa::GenerarMapa()
{	
	char path[200];
	n3DNode *ref_nodo=NULL;
	nEdificio *ref_edificio=NULL;
	nMeshNode *ref_mesh=NULL;
	nShaderNode *ref_shader=NULL;
	nTexArrayNode *ref_tex=NULL;
	nWorld *ref_world=NULL;
	vector3 v_posicion;
	ref_world=(nWorld *) kernelServer->Lookup("/game/world");

	int cont=0;
	int num_edificios=0;
	int num_ayuntamientos=0;
	int suelo=0;
	// CREAMOS LOS OBJETOS DEL MAPA (Esta búsqueda igual se haría mejor si la hiciéramos directamente objeto
	// a objeto en lugar de posición a posición.
	for (int i=0;i<ALTO_MAPA;i++) {
		for (int j=0;j<ANCHO_MAPA;j++) {
		ref_nodo=NULL;
		ref_mesh=NULL;
		ref_shader=NULL;
		ref_tex=NULL;
		ref_edificio=NULL;
		// Bloqueamos todo acceso a una altura menor de -1.5f
		// Así podemos hacer monticulos de -1.5f a 0.0f
		if (ref_world->GetHeight(vector3((float)j,0.0f,(float)i))<-1.5f) 
		{
			this->mapa_logico[i][j]='*';	
		}
		switch (this->mapa_fisico[i][j]) 
			{
				case '#':
					break;
				case 'A':
						sprintf(path,"/game/world/ayto%d",num_ayuntamientos);
						ref_edificio=(nEdificio*) kernelServer->New("nayuntamiento",path);
						this->lista_edificios.PushBack(ref_edificio);
						ref_edificio->SetId(num_ayuntamientos);
						ref_edificio->SetPos((float)j+1.5f,(float)i+1.5f);
						//v_posicion.set((float)j+1.5f,0.0f,(float)i+1.5f);
						//ref_edificio->SetEntityHeight(ref_world->GetHeight(v_posicion));
						ref_edificio->Pintar();
						cont++;
						num_ayuntamientos++;
					break;
				case 'M':
						sprintf(path,"/game/world/casamoneda");
						ref_edificio=(nEdificio*) kernelServer->New("ncasamoneda",path);
						this->lista_edificios.PushBack(ref_edificio);
						ref_edificio->SetPos((float)j+1.5f,(float)i+1.5f);
						//v_posicion.set((float)j+1.5f,0.0f,(float)i+1.5f);
						//ref_edificio->SetEntityHeight(ref_world->GetHeight(v_posicion));
						ref_edificio->Pintar();
						cont++;
					break;
				case 'P':
						sprintf(path,"/game/world/casaplanos");
						ref_edificio=(nEdificio*) kernelServer->New("ncasaplanos",path);
						this->lista_edificios.PushBack(ref_edificio);
						ref_edificio->SetPos((float)j+1.5f,(float)i+1.5f);
						//v_posicion.set((float)j+1.5f,0.0f,(float)i+1.5f);
						//ref_edificio->SetEntityHeight(ref_world->GetHeight(v_posicion));
						ref_edificio->Pintar();
						cont++;
					break;
				default:
					break;
			}
		}
	}
	// Esto es una pequeña chapucilla para 
	// colocar bien la altura de los edificios...
	for (int i=0; i<this->lista_edificios.Size();i++)
	{
		ref_edificio=this->lista_edificios.At(i);
		v_posicion=ref_edificio->GetPosition();
		ref_edificio->SetEntityHeight(ref_world->GetHeight(v_posicion));
	}
}

void nMapa::PintarDebug(bool pintar) {
	char path[N_MAXPATH];
	n3DNode *ref_nodo=NULL;
	nMeshNode *ref_mesh=NULL;
	nShaderNode *ref_shader=NULL;
	nTexArrayNode *ref_tex=NULL;
	nWorld *ref_world=NULL;
	ref_world=(nWorld *) kernelServer->Lookup("/game/world");
	int suelo=0;
	if (pintar) {
		sprintf(path,"/game/scene/debug");
		this->kernelServer->New("n3dnode",path);
		for (int i=0;i<ALTO_MAPA;i++) {
			for (int j=0;j<ANCHO_MAPA;j++) {
			ref_nodo=NULL;
			ref_mesh=NULL;
			ref_shader=NULL;
			ref_tex=NULL;
			switch (this->mapa_logico[i][j]) 
				{
					case '*':
						// DEPURACION
						int i1,j1;
						j1=j;
						i1=i;
						sprintf(path,"/game/scene/debug/suelo%d",suelo);
						ref_nodo = (n3DNode*) this->kernelServer->New("n3dnode",path);
						//ref_nodo->Txyz(floor((float)(j1-(ALTO_MAPA/2)+1)),0.1f,floor((float)(i1-(ANCHO_MAPA/2)+1)));
						ref_nodo->Txyz(floor((float)(j1+1)),ref_world->GetHeight(vector3(floor((float)(j1+1)),0.0f,floor((float)(i1+1)))),floor((float)(i1+1)));
						ref_nodo->Sxyz((float)(1.0f/16.0f-0.001f),(float)1.0f,(float)(1.0f/16.0f-0.001f));
						vector3 w_normal;
						ref_world->GetNormal(vector3(floor((float)(j1+1)),0.0f,floor((float)(i1+1))),w_normal);
						ref_nodo->Qxyzw(w_normal.x,w_normal.y,w_normal.z,0.0f);

						sprintf(path,"/game/scene/debug/suelo%d/mesh",suelo);
						ref_mesh = (nMeshNode*) this->kernelServer->New("nmeshnode",path);
						ref_mesh->SetFilename("meshes:plano.n3d");
						sprintf(path,"/game/scene/debug/suelo%d/shader",suelo);
						ref_shader = (nShaderNode*) this->kernelServer->New("nshadernode",path);
						if (ref_shader) {
							ref_shader->SetLightEnable(true);
							ref_shader->SetRenderPri(0);
							ref_shader->SetNumStages(1);
							ref_shader->BeginTUnit(0);
							ref_shader->SetColorOp(0,"replace const");
							ref_shader->SetConst(0,vector4(1.0f,0.0f,0.0f,1.0f));
							ref_shader->SetAddress(N_TADDR_CLAMP,N_TADDR_CLAMP);
							ref_shader->SetMinMagFilter(N_TFILTER_LINEAR,N_TFILTER_LINEAR);
							ref_shader->SetTexCoordSrc(N_TCOORDSRC_UV0);
							ref_shader->SetEnableTransform(false);
							ref_shader->EndTUnit();
							ref_shader->SetDiffuse(vector4(0.0f,0.0f,0.0f,0.0f));
							ref_shader->SetEmissive(vector4(0.0f,0.0f,0.0f,0.0f));
							ref_shader->SetAmbient(vector4(1.0f,0.0f,0.0f,0.5f));
							ref_shader->SetAlphaEnable(false);
							ref_shader->SetZWriteEnable(true);
							ref_shader->SetWireframe(true);
						}
						suelo++;

						ref_nodo=NULL;
						ref_mesh=NULL;
						ref_shader=NULL;
						ref_tex=NULL;
						break;	
				}
			}
		}
	}
	else {
		char *params=NULL;
		refScriptServer->Run("delete /game/scene/debug",params);
	}
}

void nMapa::BuscarRuta(int xini,int yini, int xfin, int yfin) {
	path->Reset();
	int x1,y1,x2,y2;

/*
	x1=(xini+(int)(ANCHO_MAPA/2))*NUM_DIVISIONES;
	x2=(xfin+(int)(ANCHO_MAPA/2))*NUM_DIVISIONES;
	y1=(yini+(int)(ALTO_MAPA/2))*NUM_DIVISIONES;
	y2=(yfin+(int)(ALTO_MAPA/2))*NUM_DIVISIONES;
*/

	x1=(xini)*NUM_DIVISIONES;
	x2=(xfin)*NUM_DIVISIONES;
	y1=(yini)*NUM_DIVISIONES;
	y2=(yfin)*NUM_DIVISIONES;

	path->GeneratePath(x1,y1,x2,y2);
}

bool nMapa::Movimiento_Valido(int xini,int yini,int xfin,int yfin)
{ 
	yfin-=NUM_DIVISIONES;
	xfin-=NUM_DIVISIONES;
	int x2=0, y2=0;
	x2=(int)(xfin/NUM_DIVISIONES);
	y2=(int)(yfin/NUM_DIVISIONES);
	if (x2>=ANCHO_MAPA || x2<0 || y2>=ALTO_MAPA || y2<0)
			return false;
	return (this->mapa_logico[y2][x2]=='#');
}

int nMapa::Coste_Movimiento(int xini,int yini, int xfin, int yfin)
{ 
	yfin-=NUM_DIVISIONES;
	xfin-=NUM_DIVISIONES;
	int x2=0, y2=0;
	x2=(int)(xfin/NUM_DIVISIONES);
	y2=(int)(yfin/NUM_DIVISIONES);
	int coste=0;
	if (x2>=ANCHO_MAPA || x2<0 || y2>=ALTO_MAPA || y2<0)
			return 10000;
	if (this->mapa_logico[y2][x2]=='#')
		coste=10;
	else
		coste=10000;
	return coste;
}

int nMapa::Funcion_Coste(_asNode *parent, _asNode *node, int data, void *pointer) {
	nMapa *self = (nMapa*)pointer;
	int x = (int)(node->x), y = (int)(node->y);
	int px = (int)(parent->x); int py = (int)(parent->y);
	nAutoRef<nMapa> mapa(kernelServer, self);
	mapa = "/data/mapa";
	if (px - x != 0 && py - y != 0) return 15;
	return (int)mapa->Coste_Movimiento((int)(x/NUM_DIVISIONES),(int)(y/NUM_DIVISIONES),(int)(px/NUM_DIVISIONES),(int)(py/NUM_DIVISIONES));
}

int nMapa::Funcion_Validez(_asNode *parent, _asNode *node, int data, void *pointer)
{ 
	nMapa *self = (nMapa*)pointer;
	// parent es el nodo origen y node es el destino
	int x = (int)(node->x/NUM_DIVISIONES), y = (int)(node->y/NUM_DIVISIONES);
	int px = (int)(parent->x/NUM_DIVISIONES); int py = (int)(parent->y/NUM_DIVISIONES);
	// No se permiten diagonales
	if (x>=ANCHO_MAPA || x<0 || y>=ALTO_MAPA || y<0)
			return false;
	if (px>=ANCHO_MAPA || px<0 || py>=ALTO_MAPA || py<0)
			return false;
	//if (px - x != 0 && py - y != 0) return false;
	nAutoRef<nMapa> mapa(kernelServer, self);
	mapa = "/data/mapa";
	return (int)mapa->Movimiento_Valido(x,y,px,py);
}

bool nMapa::Attach(nSceneGraph2 *sceneGraph)
{
	if (nVisNode::Attach(sceneGraph)) {
		sceneGraph->AttachVisualNode(this);
		return true;
	}
	return false;
}

void nMapa::Compute(nSceneGraph2 *sceneGraph) 
{
	
}

bool nMapa::PuedeConstruirEdificio(int posx, int posy) {
	bool valido=true;
//	int x=(posx-1+(int)(ANCHO_MAPA/2));
//	int y=(posy-1+(int)(ALTO_MAPA/2));
	int x=(posx-1);
	int y=(posy-1);

	if (x>=ANCHO_MAPA-1 || x<0 || y>=ALTO_MAPA-1 || y<0)
		return false;
	for (int i=y;i<y+2;i++) {
		for (int j=x;j<x+2;j++) {
			valido&=this->mapa_logico[i][j]=='#';
		}
	}
	return valido;
}
void nMapa::ConstruirEdificio(int posx, int posy) {
	//int x=(posx-0.5+(int)(ANCHO_MAPA/2));
	//int y=(posy-0.5+(int)(ALTO_MAPA/2));
	int x=(posx-0.5);
	int y=(posy-0.5);
	for (int i=y;i<y+2;i++) {
		for (int j=x;j<x+2;j++) {
			this->mapa_logico[i][j]='*';
		}
	}
}

void nMapa::DestruirEdificio(float posx, float posy) {
	//int x=(posx-1.5+(int)((ANCHO_MAPA)/2));
	//int y=(posy-1.5+(int)((ALTO_MAPA)/2));
	int x=(posx-1.5);
	int y=(posy-1.5);
	//n_printf("Edificio está en (%d,%d), se destruye el array (%d,%d)",posx,posy,x,y);
	for (int i=y;i<y+2;i++) {
		for (int j=x;j<x+2;j++) {
			this->mapa_logico[i][j]='#';
		}
	}
}