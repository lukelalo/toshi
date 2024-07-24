#define N_IMPLEMENTS nConstruccion
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nconstruccion.h"
#include "toshi/nworld.h"

nNebulaScriptClass(nConstruccion, "nedificio");

// Constructor por defecto
nConstruccion::nConstruccion(): ruta( "" ),
								nombre ( "" ),
								estado( INICIAL ),
								ValorEdificio( 0 )
{
	posicion_x=0.0f;
	posicion_y=0.0f;
	SetTipo(T_CONSTRUCCION);
	char buf[N_MAXPATH];
	sprintf(buf,"sel_cn");
	CanalSeleccion=refChannelServer->GenChannel(buf);
	valorSeleccion=TIEMPO_SELECCION;
	refChannelServer->SetChannel1f(CanalSeleccion,valorSeleccion);

	sprintf(buf,"construc");
	CanalConstruccion=refChannelServer->GenChannel(buf);
	valorConstruccion=0.0f;
	refChannelServer->SetChannel1f(CanalConstruccion,valorConstruccion);
	manager = NULL;
}

// Destructor
nConstruccion::~nConstruccion()
{
}

void nConstruccion::SetPos(float x,float y)
{
	posicion_x=x;
	posicion_y=y;
	this->SetPosition(x,y);
}

void nConstruccion::Pintar() 
{
}

void nConstruccion::Seleccionar() 
{
	vector3 v_posicion=this->GetPosition();
	n3DNode *seleccion;
	vector3 posicion;
	seleccion=(n3DNode *) kernelServer->Lookup("/game/scene/seleccion2");
	//this->valorSeleccion=this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2));
	this->valorSeleccion=0.0f;
}

bool nConstruccion::EstaEnRango(vector3 p_posicion)
{
    vector3 v_distancia,miPosicion;
	p_posicion.y	= 0.0f;
	miPosicion		= this->GetPosition();
	//miPosicion.y=this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2));
	miPosicion.y	= 0.0f;
	v_distancia		= p_posicion-miPosicion;
	return v_distancia.len() < DISTANCIA_A_CONSTRUCCION;
}

void nConstruccion::Construir( nJugador* jugador )
{
	vector3 v_posicion=this->GetPosition();
	nMapa* mapa = (nMapa*) kernelServer->Lookup( "/data/mapa" );
	mapa->ConstruirEdificio( v_posicion.x - 0.5f, v_posicion.z - 0.5f );
	n3DNode* polvo = (n3DNode*) kernelServer->Lookup( "/game/scene/escenario/polvo" );
	//polvo->Txyz( this->GetPosition().x, this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2)), this->GetPosition().z );
	polvo->Txyz( v_posicion.x, this->refWorld->GetHeight(vector3(v_posicion.x,0.0f,v_posicion.z)), v_posicion.z );
	polvo->SetActive( true );
	this->estado = EN_CONSTRUCCION;
	//this->valorConstruccion = this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2));
	this->valorConstruccion = 0.0f;
	this->Tick( 0 );
	this->CambiarShader( "bicolor.n" );
	SetManager( jugador );
	this->SetVisible(true);
}

void nConstruccion::Destruir()
{
	vector3 v_posicion=this->GetPosition();
	n3DNode *humo=(n3DNode *)kernelServer->Lookup("/game/scene/escenario/humo");
	//humo->Txyz(this->GetPosition().x,this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2)),this->GetPosition().z);
	humo->Txyz(v_posicion.x,this->refWorld->GetHeight(vector3(v_posicion.x,0.0f,v_posicion.z)),v_posicion.z);
	humo->SetActive(true);
	this->estado=EN_DESTRUCCION;
	this->valorConstruccion=TIEMPO_CONSTRUCCION;
	this->Tick(0);
	nMapa *mapa = (nMapa*) kernelServer->Lookup("/data/mapa");
	mapa->DestruirEdificio(v_posicion.x,v_posicion.z);
}

bool nConstruccion::EstaConstruido()
{
	return this->estado==CONSTRUIDO;
}

void nConstruccion::Tick(float dt) 
{
	vector3 v_posicion=this->GetPosition();
	switch (this->estado) {
		case EN_CONSTRUCCION:
			if (this->valorConstruccion<TIEMPO_CONSTRUCCION) {
				this->valorConstruccion+=dt;
			} else if (this->valorConstruccion>TIEMPO_CONSTRUCCION) {
				this->valorConstruccion=TIEMPO_CONSTRUCCION;
				this->estado=CONSTRUIDO;
				//this->SetEntityHeight(this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2)));
				n3DNode *polvo=(n3DNode *)kernelServer->Lookup("/game/scene/escenario/polvo");
				polvo->SetActive(false);
			}
			this->refChannelServer->SetChannel1f(this->CanalConstruccion,this->valorConstruccion);
			this->SetCollideClass("edificio");
			break;
		case EN_DESTRUCCION:
			//if (this->valorConstruccion>this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2))) {
			if (this->valorConstruccion>0.0f) 
			{
				this->valorConstruccion-=dt;
			} 
			//else if (this->valorConstruccion<this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2))) 
			else if (this->valorConstruccion<0.0f) 
			{
				//this->valorConstruccion=this->refWorld->GetHeight(vector3(this->GetPosition().x-1.0f+ANCHO_MAPA/2,0.0f,this->GetPosition().z-1.0f+ALTO_MAPA/2));
				this->valorConstruccion=0.0f;
				this->estado=DESTRUIDO;
				n3DNode *humo=(n3DNode *)kernelServer->Lookup("/game/scene/escenario/humo");
				humo->SetActive(false);
			}
			this->refChannelServer->SetChannel1f(this->CanalConstruccion,this->valorConstruccion);
			break;
		case CONSTRUIDO:
			if (this->valorSeleccion<TIEMPO_SELECCION) {
				this->valorSeleccion+=dt;
				this->refChannelServer->SetChannel1f(this->CanalSeleccion,this->valorSeleccion);
			}	
			else if (this->valorSeleccion>TIEMPO_SELECCION) {
				this->valorSeleccion=TIEMPO_SELECCION;
				this->refChannelServer->SetChannel1f(this->CanalSeleccion,this->valorSeleccion);
			}
			break;
		case DESTRUIDO:
			this->SetVisible(false);
			break;
		case INICIAL:
			break;
	}
	this->SetEntityHeight(-5.0+this->valorConstruccion+this->refWorld->GetHeight(v_posicion));
	Update();
}

void nConstruccion::SetValorConstruccion(float valor) 
{
	this->valorConstruccion=valor;
}

void nConstruccion::SetValor(int valor) 
{
	this->ValorEdificio=valor;
}

int nConstruccion::GetValor() 
{
	return ValorEdificio;
}

void nConstruccion::CambiarShader(const char* shader) 
{
	char *paramsScript=NULL;
	char buf[N_MAXPATH];
	sprintf(buf,"%s/shader",this->GetVisNode());
	kernelServer->SetCwd(kernelServer->Lookup(buf));
	sprintf(buf,"runshader %s",shader);
    this->refScriptServer->Run(buf,paramsScript);
}

void nConstruccion::SetId( int id )
{
	this->id_construccion = id;
}

int nConstruccion::GetId()
{
	return id_construccion;
}

void nConstruccion::SetManager( nJugador* jugador )
{
	this->manager = jugador;
}

nJugador* nConstruccion::GetManager()
{
	return manager;
}

void nConstruccion::SetNombre( const char *p_nombre )
{
	this->nombre = p_nombre;
}

const char *nConstruccion::GetNombre()
{
	return nombre.Get();
}

nConstruccion::TEstadoConstruccion nConstruccion::GetEstado()
{
	return estado;
}