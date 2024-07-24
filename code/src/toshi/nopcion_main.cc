#define N_IMPLEMENTS nOpcion
//------------------------------------------------------------------------------
//  (C) 2005	Pei
//------------------------------------------------------------------------------
#include "toshi/nopcion.h"
#include "kernel/nscriptserver.h"
#include "node/ntexarraynode.h"
#include "node/n3dnode.h"

nNebulaScriptClass( nOpcion, "nroot" );

/*------------------------------------------------------------------------------

Clase nOpcion
Opción de un menú con sus eventos

------------------------------------------------------------------------------*/

// Constructor por defecto
nOpcion::nOpcion() :
    scriptServer(kernelServer, this)
{
	scriptServer		=	"/sys/servers/script";
	nodoSprite			=	"";
	imagenOpcion		=	"";
	imagenOver			=	"";
	imagenSeleccionado	=	"";
	accionAEjecutar		=	"";
	posx				=	0.0f;
	posy				=	0.0f;
	ancho				=	0.0f;
	alto				=	0.0f;
	mouseOver			=	false;
	selected			=	false;
	creadoSprite		=	false;
}

// Destructor
nOpcion::~nOpcion()
{
}

void nOpcion::onClick() 
{
	const char* result;
	scriptServer->Run(accionAEjecutar.Get(), result);
}

void nOpcion::onSelect() 
{
	if (!selected) 
	{
		this->selected=true;

		char buf[N_MAXPATH];
		sprintf(buf,"%s/tex",this->nodoSprite.Get());
		nTexArrayNode *imagen=(nTexArrayNode *) kernelServer->Lookup(buf);
		if (mouseOver)
			imagen->SetTexture(0,imagenOver.Get(),NULL);
		else
			imagen->SetTexture(0,imagenSeleccionado.Get(),NULL);
	}
}

void nOpcion::onMouseOver() 
{
	if (!mouseOver) 
	{
		this->mouseOver=true;

		char buf[N_MAXPATH];
		sprintf(buf,"%s/tex",this->nodoSprite.Get());
		nTexArrayNode *imagen=(nTexArrayNode *) kernelServer->Lookup(buf);
		imagen->SetTexture(0,imagenOver.Get(),NULL);
	}
}

void nOpcion::onMouseOut() 
{
	if (mouseOver) 
	{
		this->mouseOver=false;

		char buf[N_MAXPATH];
		sprintf(buf,"%s/tex",this->nodoSprite.Get());
		nTexArrayNode *imagen=(nTexArrayNode *) kernelServer->Lookup(buf);
		if (selected)
			imagen->SetTexture(0,imagenSeleccionado.Get(),NULL);
		else
			imagen->SetTexture(0,imagenOpcion.Get(),NULL);
	}
}

void nOpcion::SetNodo(const char *nodo) 
{
	char buf[N_MAXPATH];
	this->nodoSprite=nodo;
	n3DNode *imagen=(n3DNode *) kernelServer->New("n3dnode",this->nodoSprite.Get());
	imagen->SetActive(false);
	imagen->Preload();
	sprintf(buf,"%s/tex",this->nodoSprite.Get());
	nTexArrayNode *tex = (nTexArrayNode *) kernelServer->New("ntexarraynode",buf);
}

void nOpcion::GenerarSprite() 
{
	char buf[N_MAXPATH];
	sprintf(buf,"%s/sprite",this->nodoSprite.Get());
	nCSprite *sprite=(nCSprite *) kernelServer->New("nCSprite",buf);
	sprite->setAutoScale(false);
	sprite->setOrder(51);
	sprite->setScreenPositionf(this->posx,this->posy);
	sprite->setScreenSizef(this->ancho,this->alto);
	sprite->Preload();
	this->creadoSprite=true;
}

void nOpcion::SetAccion(const char *accion) 
{
	this->accionAEjecutar=accion;
}

void nOpcion::SetPosicion(float posx, float posy)
{
	this->posx=posx;
	this->posy=posy;

	if (!this->creadoSprite)
		return;

	char buf[N_MAXPATH];
	sprintf(buf,"%s/sprite",this->nodoSprite.Get());
	nCSprite *imagen=(nCSprite *) kernelServer->Lookup(buf);
	imagen->setScreenPositionf(posx,posy);
	imagen->Preload();
}

void nOpcion::SetDimensiones(float ancho, float alto)
{
	this->ancho=ancho;
	this->alto=alto;

	if (!this->creadoSprite)
		return;

	char buf[N_MAXPATH];
	sprintf(buf,"%s/sprite",this->nodoSprite.Get());
	nCSprite *imagen=(nCSprite *) kernelServer->Lookup(buf);
	imagen->setScreenSizef(ancho,alto);
	imagen->Preload();
}

void nOpcion::SetImagenPrincipal(const char *imagen)
{
	this->imagenOpcion=imagen;
	char buf[N_MAXPATH];
	sprintf(buf,"%s/tex",this->nodoSprite.Get());
	nTexArrayNode *tex=(nTexArrayNode *) kernelServer->Lookup(buf);
	tex->SetTexture(0,imagenOpcion.Get(),NULL);
	tex->Preload();
}

void nOpcion::SetImagenOver(const char *imagen)
{
	this->imagenOver=imagen;
}

void nOpcion::SetImagenSeleccion(const char *imagen)
{
	this->imagenSeleccionado=imagen;
}

bool nOpcion::CheckMousePosition(float posx, float posy) 
{
	if (posx>this->posx && posx<this->posx+this->ancho &&
		posy>this->posy && posy<this->posy+this->alto)
		return true;
	else
		return false;
}

bool nOpcion::CheckSelected()
{
	return this->selected;
}

void nOpcion::Mostrar() 
{
	n3DNode *imagen=(n3DNode *) kernelServer->Lookup(this->nodoSprite.Get());
	imagen->SetActive(true);
	char buf[N_MAXPATH];
	sprintf(buf,"%s/tex",this->nodoSprite.Get());

	nTexArrayNode *textura=(nTexArrayNode *) kernelServer->Lookup(buf);
	if (this->selected)
		textura->SetTexture(0,imagenSeleccionado.Get(),NULL);
	else
		textura->SetTexture(0,imagenOpcion.Get(),NULL);

	SetPosicion(this->posx,this->posy);
	SetDimensiones(this->ancho,this->alto);
}

void nOpcion::Ocultar() 
{
	n3DNode *imagen=(n3DNode *) kernelServer->Lookup(this->nodoSprite.Get());
	imagen->SetActive(false);
}