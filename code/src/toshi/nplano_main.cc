#define N_IMPLEMENTS nPlano
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nplano.h"
#include "toshi/nmapa.h"
#include "toshi/nconstruccion.h"
#include "node/ntexarraynode.h"

nNebulaScriptClass( nPlano, "n3dnode" );


// Constructor por defecto
nPlano::nPlano() : medidas( 0.09f, 0.11f ),
				   posicion( 0.0f, 0.86f ),
				   refScriptServer( kernelServer, this )
{
	refScriptServer = "/sys/servers/script";
	IdHUD           = 0;
	IdEdificio	    = 0;
	Nombre          = "";
	Distintivo      = DESCONOCIDO;
	Descripcion     = "";
	Descripcion2    = "";
	Coste           = 0;
	Valor           = 0;
	Construido      = false;
}


// Constructor general
nPlano::nPlano( int id_plano, int id_edificio, string nombre_plano,
				nPlano::TPlano distintivo_plano, string descripcion_plano,
				string descripcion_plano2, int coste_plano, int valor_plano ) : medidas( 0.09f, 0.11f ),
																			    posicion( 0.0f, 0.86f ),
																			    refScriptServer( kernelServer, this )
{
	refScriptServer = "/sys/servers/script";
	SetIdHUD( id_plano );
	SetIdEdificio( id_edificio );
	SetNombre( nombre_plano );
	SetDistintivo( distintivo_plano );
	SetDescripcion( descripcion_plano );
	SetDescripcion2( descripcion_plano2 );
	SetCoste( coste_plano );
	SetValor( valor_plano );
	GenerarModeloEdificio();
	Construido = false;
}


// Destructor
nPlano::~nPlano()
{
}


/********************************************************************************/
/* M�todo      : SetIdHUD													    */
/* Descripci�n : Asigna al atributo IdHUD el valor pasado por par�metro.		*/
/* Par�metros  : int id_plano (ENT). Valor para el IdHUD.					 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetIdHUD( int id_plano )
{
	IdHUD = id_plano;
}


/********************************************************************************/
/* M�todo      : SetIdEdificio												    */
/* Descripci�n : Asigna al atributo IdEdificio el valor pasado por par�metro.	*/
/* Par�metros  : int id_edificio (ENT). Valor para el IdEdificio.			 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetIdEdificio( int id_edificio )
{
	IdEdificio = id_edificio;
}


/********************************************************************************/
/* M�todo      : SetNombre													    */
/* Descripci�n : Asigna al atributo Nombre el valor pasado por par�metro.		*/
/* Par�metros  : string nombre_plano (ENT). Valor para el Nombre.			 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetNombre( string nombre_plano )
{
	Nombre = nombre_plano;
}


/********************************************************************************/
/* M�todo      : SetDistintivo												    */
/* Descripci�n : Asigna al atributo Distintivo el valor pasado por par�metro.	*/
/* Par�metros  : TPlano distintivo_plano (ENT). Valor para el Distintivo.	 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetDistintivo( nPlano::TPlano distintivo_plano )
{
	Distintivo = distintivo_plano;
}


/********************************************************************************/
/* M�todo      : SetDescripcion												    */
/* Descripci�n : Asigna al atributo Descripcion el valor pasado por par�metro.	*/
/* Par�metros  : string descripcion_plano (ENT). Valor para la Descripcion.	 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetDescripcion( string descripcion_plano )
{
	Descripcion = descripcion_plano;
}


/********************************************************************************/
/* M�todo      : SetDescripcion2											    */
/* Descripci�n : Asigna al atributo Descripcion2 el valor pasado por par�metro.	*/
/* Par�metros  : string descripcion_plano (ENT). Valor para la Descripcion2. 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetDescripcion2( string descripcion_plano )
{
	Descripcion2 = descripcion_plano;
}


/********************************************************************************/
/* M�todo      : SetCoste													    */
/* Descripci�n : Asigna al atributo Coste el valor pasado por par�metro.		*/
/* Par�metros  : int coste_plano (ENT). Valor para el Coste.				 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetCoste( int coste_plano )
{
	Coste = coste_plano;
}


/********************************************************************************/
/* M�todo      : SetValor													    */
/* Descripci�n : Asigna al atributo Valor el valor pasado por par�metro.		*/
/* Par�metros  : int valor_plano (ENT). Valor para el Valor.				 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetValor( int valor_plano )
{
	Valor = valor_plano;
}


/********************************************************************************/
/* M�todo      : EstaConstruido												    */
/* Descripci�n : Comprueba si el plano est� construido.							*/
/* Par�metros  : 																*/
/* Devuelve    : TRUE si est� construido.										*/
/*				 FALSE si no lo est�.											*/
/********************************************************************************/
bool nPlano::EstaConstruido()
{
	return Construido;
}


/********************************************************************************/
/* M�todo      : Construir													    */
/* Descripci�n : Se modifica el atributo correspondiente del plano a construido	*/
/* Par�metros  : 																*/
/* Devuelve    : La cadena con el nombre de la construcci�n.					*/
/********************************************************************************/
const char *nPlano::Construir()
{
	Construido = true;
	nString buf;
	char buf2[N_MAXPATH];
	sprintf( buf2, "/game/world/cons%d", GetIdEdificio() );
	buf.Set( buf2 );
	return buf.Get();
}


/********************************************************************************/
/* M�todo      : Click														    */
/* Descripci�n : Se modifica el atributo correspondiente del plano a construido	*/
/* Par�metros  : float x (ENT). Valor de la coordenada x de la pantalla.		*/
/*				 float y (ENT). Valor de la coordenada y de la pantalla.		*/
/* Devuelve    : TRUE si la posici�n est� dentro del recuadro asociado al plano	*/
/*				 FALSE si la posici�n queda fuera.								*/
/********************************************************************************/
bool nPlano::Click( float x, float y )
{
	if ( x > posicion.x && x < ( posicion.x + medidas.x ) &&
		 y > posicion.y && y < ( posicion.y + medidas.y ) )
		return true;
	else
		return false;
}


/********************************************************************************/
/* M�todo      : SetPosition												    */
/* Descripci�n : Asigna al atributo posicion el valor pasado por par�metro.		*/
/* Par�metros  : vector2 pos_ent (ENT). Valor para la posicion.				 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetPosition( vector2 pos_ent )
{
	posicion = pos_ent;
}


/********************************************************************************/
/* M�todo      : SetMedidas													    */
/* Descripci�n : Asigna al atributo medidas el valor pasado por par�metro.		*/
/* Par�metros  : vector2 med_ent (ENT). Valor para las medidas.				 	*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetMedidas( vector2 med_ent )
{
	medidas = med_ent;
}


/********************************************************************************/
/* M�todo      : Pintar														    */
/* Descripci�n : Asigna un sprite y una textura al plano creado.    			*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::Pintar()
{
	char           path[N_MAXPATH];
	nCSprite       *ref_sprite;
	nTexArrayNode  *ref_tex;

	sprintf( path, "/game/scene/hud/plano%d/sprite", IdHUD );
	ref_sprite = (nCSprite *) kernelServer->New( "nCSprite", path );
	ref_sprite->setAutoScale( false );
	ref_sprite->setScreenSizef( medidas.x, medidas.y );
	ref_sprite->setScreenPositionf( posicion.x, posicion.y );
	ref_sprite->setOrder( 11 + IdHUD );
	sprintf( path, "/game/scene/hud/plano%d/tex", IdHUD );
	ref_tex = (nTexArrayNode *) kernelServer->New( "ntexarraynode", path );
	sprintf( path, "textures:pergamino%d%d.png", Distintivo, (int) (Coste/10) );
	ref_tex->SetTexture( 0, path, NULL );
}


/********************************************************************************/
/* M�todo      : GenerarModeloEdificio										    */
/* Descripci�n : Asigna un modelo al plano creado.    							*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::GenerarModeloEdificio()
{
	char path[N_MAXPATH];
	char *params = NULL;

	sprintf( path, "GenerarEdificio %d %d %d", Distintivo, (int) (Valor/10), IdEdificio );
	refScriptServer->Run( path, params );
}


/********************************************************************************/
/* M�todo      : SetPlanoHUD												    */
/* Descripci�n : Asigna una entidad para colgarlo del HUD.						*/
/* Par�metros  : 																*/
/* Devuelve    : 																*/
/********************************************************************************/
void nPlano::SetPlanoHUD()
{
	char  path[512];

	sprintf( path, "/game/scene/hud/plano%d", IdHUD );
	PlanoHUD = (n3DNode *) kernelServer->New( "n3dnode", path );
}


/********************************************************************************/
/* M�todo      : GetPlanoHUD												    */
/* Descripci�n : Obtiene la entidad que cuelga del HUD.							*/
/* Par�metros  : 																*/
/* Devuelve    : La entidad.													*/
/********************************************************************************/
n3DNode* nPlano::GetPlanoHUD()
{
	return PlanoHUD;
}