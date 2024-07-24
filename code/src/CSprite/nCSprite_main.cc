#define N_IMPLEMENTS nCSprite

#include "gfx/ntexturearray.h"
#include "gfx/npixelshader.h"

#include "CSprite/nCSprite.h"
nNebulaScriptClass(nCSprite, "nvisnode");

//------------------------------------------------------------------------------
/**
 * Constructora sin par�metros. Como no existe textura a�n, el par�metro "visible"
 * ser� puesto a false para que el nodo no se a�ada a la escena y por tanto no 
 * se visualice de momento. 
 */
nCSprite::nCSprite() : nVisNode(), pixelShader(NULL), taArchivoTextura(NULL), 
					   imageAncho(0), imageAlto(0), lastFrameUsingExternalTexture(false),
					   sizeXExternalTexture(0), sizeYExternalTexture(0), _order(0) {

	// Inicializamos el buffer de �ndices.
	initStaticIndexBuffer();
	_visible=true;
	// Inicializamos las posiciones: se dibuja entero, desde la esquina
	// superior izquierda.
	setScreenPositionf(0.0, 0.0);
	setScreenSizef(1.0, 1.0);

	setSourcePositionf(0.0, 0.0);
	setSourceSizef(1.0, 1.0);
	setAutoScale(true);
}
//------------------------------------------------------------------------------
/**
 * Destructora.
 */
nCSprite::~nCSprite(){
	if (pixelShader)
		pixelShader->Release();
	if (indexBuffer)
		indexBuffer->Release();
	if (taArchivoTextura)
		delete taArchivoTextura;
}

//------------------------------------------------------------------------------
/**
 * M�todo para crear un sprite a partir de un fichero en el que se encuentra
 * la textura. Este m�todo ser� el invocado para cargar texturas desde un script
 * en tcl.Recibe como par�metro el archivo del que va a leer la textura y 
 * por tanto, el par�metro "visible" se pondr� a cierto para que el nodo se a�ada 
 * a la escena y se visualice posteriormente. Para esto,se llama aqu� a las funciones 
 * que rellenan las estructuras correspondientes para la correcta renderizaci�n de este nodo.
 * @param fichero Fichero del que se cargar� la textura.
 */
void nCSprite::createSprite(const char* fich){

	if (!refGfx.isvalid())
		return;

	if (fich && (strcmp(fich, archivoTextura) == 0))
		return;

	if (taArchivoTextura) {
		delete taArchivoTextura;
		taArchivoTextura = NULL;
	}

	if ((!fich) || (strlen(fich) == 0)) {
		imageAlto = imageAncho = 0;
		notVisible = true;
		return;
	}

	setVisible(true);
	n_assert(strlen(fich) < N_MAXNAMELEN);
	strcpy(archivoTextura, fich);
	
	taArchivoTextura = new nTextureArray(this);
	if (!taArchivoTextura->SetTexture(refGfx.get(),0,archivoTextura,0,true,true)) {
		createSprite(NULL);
/*		delete taArchivoTextura;
		taArchivoTextura = NULL;
		strcpy(archivoTextura, "");
		notVisible = true;
		imageAncho = 0;
		imageAlto = 0;*/
		return;
	}
	
	// Actualizamos el tama�o de la textura
	nTexture* tex = taArchivoTextura->GetTexture(0);
	imageAncho = tex->GetWidth();
    imageAlto = tex->GetHeight();

	vbDirty = true;
}
//------------------------------------------------------------------------------
/**	
 * Hace visible el sprite.
 * @param visible Ser� cierto si el sprite ser� visible y falso en caso contrario.
 */
void nCSprite::setVisible(bool visible){

	_visible=visible;
}
//------------------------------------------------------------------------------
/**
 * Establece las coordenadas de pantalla donde se dibujar� el sprite.
 *@param posX
 *@param posY
 */
void nCSprite::setScreenPosition(int posX, int posY){

	screenX = posX;
	screenY = posY;
	screenIntUsed = true;
	vbDirty = true;
}

/**
 Establece las coordenadas relativas donde se dibujar� el sprite en la
 pantalla (entre 0 y 1).
*/
void nCSprite::setScreenPositionf(float posX, float posY) {

//	n_assert((0 <= posX) && (posX <= 1));
//	n_assert((0 <= posY) && (posY <= 1));
	fScreenX = posX;
	fScreenY = posY;
	screenIntUsed = false;
	vbDirty = true;
}

//------------------------------------------------------------------------------

void nCSprite::setScreenSize(int width, int height) {
	rectangleScreenAlto = height;
	rectangleScreenAncho = width;
	rectangleScreenIntUsed = true;
	vbDirty = true;
}

/**
Establece el tama�o relativo en la pantalla de lo que se va a dibujar,
entre 0 y 1 (relativo a la pantalla).
*/
void nCSprite::setScreenSizef(float width, float height) {
	n_assert((0 <= height) && (height <= 1));
	n_assert((0 <= width) && (width <= 1));

	fRectangleScreenAlto = height;
	fRectangleScreenAncho = width;
	rectangleScreenIntUsed = false;
	vbDirty = true;
}

void nCSprite::setSourcePosition(int posX, int posY) {
	sourceX = posX;
	sourceY = posY;
	sourceIntUsed = true;
	vbDirty = true;
}

/**
Establece la posici�n de la esquina superior izquierda en la
textura origen, relativo a la textura, es decir, entre 0 y 1,
independientemente del tama�o de la textura.
*/
void nCSprite::setSourcePositionf(float posX, float posY) {
	n_assert((0 <= posX) && (posX <= 1));
	n_assert((0 <= posY) && (posY <= 1));

	fSourceX = posX;
	fSourceY = posY;
	sourceIntUsed = false;
	vbDirty = true;
}

void nCSprite::setSourceSize(int width, int height) {
	rectangleSourceAlto = height;
	rectangleSourceAncho = width;
	rectangleSourceIntUsed = true;
	vbDirty = true;
}

/**
Establece el tama�o del rect�ngulo en la imagen origen, relativo
al tama�o de la textura, es decir, entre 0 y 1.
*/
void nCSprite::setSourceSizef(float width, float height) {
	n_assert((0 <= height) && (height <= 1));
	n_assert((0 <= width) && (width <= 1));

	fRectangleSourceAlto = height;
	fRectangleSourceAncho = width;
	rectangleSourceIntUsed = false;
	vbDirty = true;
}

void nCSprite::setAutoScale(bool autoScale) {
	if (this->autoScale == autoScale)
		return;

	this->autoScale = autoScale;
	vbDirty = true;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

/**
 * Devuelve el ancho y alto del archivo leido.
 *@param longX
 *@param longY
 */ 
void nCSprite::getImageSize(int& longX,int& longY){

	longX = imageAncho;
	longY = imageAlto;
}

//------------------------------------------------------------------------------

/**
 * M�todo que a�ade este nodo a la escena.
 *@param sceneGraph 
 */
bool nCSprite::Attach(nSceneGraph2 *sceneGraph){
//	return n3DNode::Attach(sceneGraph);

	if (!refGfx.isvalid())
		return false;

	bool generado=false;
	if (nVisNode::Attach(sceneGraph) && _visible) 
	{
		//n_printf("Mostrando Sprite\n");
		sceneGraph->AttachMeshNode(this);
		sceneGraph->AttachRenderPri(_order);
		generado=true;
	}
	return generado;
}

//------------------------------------------------------------------------------

/**
 * M�todo al que llama cada nodo de la estructura de Nebula con todos los objetos visuales 
 * que se le han ido a�adiendo para actualizar sus valores.
 *@param sceneGraph
 */
void nCSprite::Compute(nSceneGraph2 *sceneGraph){
	
	if (sceneGraph == NULL)
		return;

	if (!refGfx.isvalid())
		return;

	nVisNode::Compute(sceneGraph);

	if (!_visible)
		return;

	nTextureArray *ta;
	nPixelShader *pshader;

	ta = (nTextureArray*)sceneGraph->GetTextureArray();
	pshader = (nPixelShader*)sceneGraph->GetPixelShader();
	
	/* Miramos si hay que actualizar el buffer de v�rtices. Eso ocurre cuando ocurre
	una de las siguientes cosas:
	- El tama�o de la ventana a cambiado
	- La referencia al buffer de v�rtices no es v�lida (a�n no se ha inicializado).
	- El contenido est� 'sucio', debido al cambio de alg�n rect�ngulo.
	- Existe una textura 'hermana', y o bien en el �ltimo frame no la utilizamos,
	o bien ha cambiado su tama�o.
	- No existe una textura hermana, y en el �ltimo frame utilizamos una.
	*/
	if (UpdateScreenSize() || vbDirty || (!ref_vb.isvalid()) ||
		((ta) && (ta->GetTexture(0)) && 
			( (!lastFrameUsingExternalTexture) || (UpdateSiblingTextureSize(ta) ))) ||
		(lastFrameUsingExternalTexture && (!ta || !ta->GetTexture(0))))
//		 (lastFrameUsingExternalTexture) && (UpdateSiblingTextureSize(ta))) ||
//		((ta) && (lastFrameUsingExternalTexture != (bool)ta->GetTexture(0))))
		calculateVertexBuffer(ta);

	if (notVisible)
		return;

	if (!ta || !ta->GetTexture(0)) {
		ta = taArchivoTextura;
		lastFrameUsingExternalTexture = false;
	} else
		lastFrameUsingExternalTexture = true;

	if (!pshader)
		pshader = getStaticPixelShader();

	matrix44 identity;
        
    refGfx.get()->PushMatrix(N_MXM_PROJECTION);
    refGfx.get()->PushMatrix(N_MXM_MODELVIEW);
	refGfx.get()->SetMatrix(N_MXM_MODELVIEW, identity);
    refGfx.get()->SetMatrix(N_MXM_PROJECTION,identity);

	ref_vb.get()->Render(indexBuffer, pshader, ta);

    refGfx.get()->PopMatrix(N_MXM_PROJECTION);
    refGfx.get()->PopMatrix(N_MXM_MODELVIEW);
		
}

//------------------------------------------------------------------------------
      
nPixelShader *nCSprite::getStaticPixelShader() {

	// Si ya est�, lo devolvemos sin m�s.
	if (pixelShader)
		return pixelShader;

	if ( !refGfx.isvalid() )
		return NULL;

	pixelShader = refGfx.get()->FindPixelShader("nCSpriteShader");

	if (pixelShader) {
		pixelShader->AddRef();
		return pixelShader;
	}

	// El shader no est� a�n registrado; lo creamos.
	pixelShader = refGfx->NewPixelShader("nCSpriteShader");

	/*
	Explicaci�n del shader: queremos que se utilice �nicamente
	el color y transparencia de la textura, por lo tanto,
	la operaci�n no tiene en cuenta el color del v�rtice,
	ni las luces.
	*/

	nPixelShaderDesc *psDesc=new nPixelShaderDesc();
	psDesc->SetNumStages(1);
	psDesc->SetColorOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	/*psDesc->SetConst(0,vector4(0,0,0,0));*/
	psDesc->SetAddressU(0, N_TADDR_CLAMP);
	psDesc->SetAddressV(0, N_TADDR_CLAMP);
	psDesc->SetMinFilter(0, N_TFILTER_LINEAR_MIPMAP_NEAREST);//N_TFILTER_LINEAR_MIPMAP_NEAREST);
	psDesc->SetMagFilter(0, N_TFILTER_LINEAR_MIPMAP_NEAREST); // LINEAR? Yo creo que es mejor nearest...
	psDesc->SetTexCoordSrc(0,N_TCOORDSRC_UV0);
	psDesc->SetEnableTransform(0,false);
	/*psDesc->Txyz(0,vector3(0,0,0));
	psDesc->Rxyz(0,vector3(0,0,0));
	psDesc->Sxyz(0,vector3(1,1,1));*/
	// Para el alpha...
	psDesc->SetAlphaEnable(true);
	psDesc->SetAlphaTestEnable(true);
	// nRStateParam
	psDesc->SetAlphaSrcBlend(N_ABLEND_SRCALPHA);
	psDesc->SetAlphaDestBlend(N_ABLEND_INVSRCALPHA);
	psDesc->SetAlphaFunc(N_CMP_GREATEREQUAL);
	//Debe ser para OPENGL 0.0625 y para DIRECTX 0.9375
	if (strcmp(refGfx->GetClass()->GetName(),"nglserver")==0) 
		psDesc->SetAlphaRef(0.0625);
	else
		psDesc->SetAlphaRef(0.9375);
	psDesc->SetAlphaOp(0, nPSI::nOp::REPLACE,
					nPSI::nArg::TEX,
					nPSI::nArg::PREV,
					nPSI::nArg::NOARG,
					nPSI::nScale::ONE);
	//psDesc->SetColorMaterial(N_CMAT_DIFFUSE);
	psDesc->SetZWriteEnable(false);
	psDesc->SetZFunc(N_CULL_NONE);
	// Para el color...
	pixelShader->SetShaderDesc(psDesc);

	return pixelShader;
}

void nCSprite::initStaticIndexBuffer() {

	if (!refGfx.isvalid())
		return;

	// Lo buscamos, a partir del nombre del recurso.
	indexBuffer = refGfx.get()->FindIndexBuffer("nCSpriteIndexBuffer");
	if (indexBuffer) {
		indexBuffer->AddRef();
		return;
	}

	// Si no est�, lo creamos.
	indexBuffer = refGfx->NewIndexBuffer("nCSpriteIndexBuffer");
	if (!indexBuffer) {
		n_message("ERROR: Can't create shared index buffer in nCSprite.");
		return;
	}

	// Establecemos las propiedades, y lo rellenamos.
/* 
	El rect�ngulo a dibujar vendr� determinado por cuatro v�rtices dispuestos del siguiente
	modo:

					  _____________
			vertice0 |			   | vertice1
					 |			   |
			vertice3 |			   | vertice2
                      ------------- 

	(a partir de ellos, los tri�ngulos ser�n: v0-v2-v1 y v0-v3-v2)
*/

	nIBufType ibType =  N_IBTYPE_WRITEONLY;  //N_IBTYPE_STATIC; 
	nPrimType ibPrimType = N_PTYPE_TRIANGLE_LIST; 
	indexBuffer->Begin(ibType, ibPrimType, 6); 

	//El tri�ngulo 1 est� formado por los v�rtices 0, 1 y 2
	indexBuffer->Index(0,0); //v�rtice0
	indexBuffer->Index(1,2); //v�rtice2
	indexBuffer->Index(2,1); //v�rtice1
	//El tri�ngulo 1 est� formado por los v�rtices 0, 2 y 3
	indexBuffer->Index(3,0); //v�rtice0
	indexBuffer->Index(4,3); //v�rtice3
	indexBuffer->Index(5,2); //v�rtice2

	indexBuffer->End();
}

/**
Rellena el buffer de v�rtices, teniendo en cuenta las posiciones origen
y destino.
@param ta Array de texturas que se va a utilizar para dibujar. De �l, mirar�
el tama�o de la textura 0 que se dibuja, y considerar� que es de ah� de donde
se coge el sprite. Si no se indica par�metro (valor por defecto NULL),
se considera que la textura es la establecida con createSprite.
*/
void nCSprite::calculateVertexBuffer(nTextureArray *ta) {

	if (! refGfx.isvalid() )
		return;
/* 
	El rect�ngulo a dibujar vendr� determinado por cuatro v�rtices dispuestos del siguiente modo
		(a partir de los tri�ngulos que formar�an los v�rtices: v0-v1-v2 y v0-v3-v2):

					  _____________
			vertice0 |			   | vertice1
					 |			   |
			vertice3 |			   | vertice2
                      ------------- 

	Y viene dado por dos cosas: la posici�n-tama�o del rect�ngulo en la textura origen, que
afectar� a las coordenadas de textura, y su posici�n-tama�o en la pantalla, que afectar�
a los v�rtices.
*/
	//n_printf("updating vertex buffer...\n");
	vbDirty = false;
	// Lo primero es crear el buffer de v�rtices, si no lo estaba ya
	if (!ref_vb.isvalid()) {
		char nombreRecurso[N_MAXNAMELEN];
		sprintf(nombreRecurso, "nCSpriteVB%d", (int)this);
		int vbComponents = (N_VT_COORD | N_VT_UV0); // N_VT_NORM?
		int numVertex = 4;
		nVBufType vbType =  N_VBTYPE_STATIC; // N_VBTYPE_WRITEONLY;
		ref_vb = refGfx.get()->NewVertexBuffer(nombreRecurso, vbType, vbComponents, numVertex);
	}

	// Y ahora lo rellenamos.

	// Primero las coordenadas de textura. 
	int imageAncho = this->imageAncho;
	int imageAlto = this->imageAlto;

	if (ta && ta->GetTexture(0)) {
		imageAncho = ta->GetTexture(0)->GetWidth();
		imageAlto = ta->GetTexture(0)->GetHeight();
	}

	float u0, v0;
	float u1, v1;
	if (sourceIntUsed) {
		u0 = (float)sourceX / imageAncho;
		v0 = (float)sourceY / imageAlto;
		fSourceX = u0;
		fSourceY = v0;
	} else {
		u0 = fSourceX;
		v0 = fSourceY;
		sourceX = (int)(u0 * imageAncho);
		sourceY = (int)(v0 * imageAlto);
	}

	if (rectangleSourceIntUsed) {
		u1 = (float)(sourceX + rectangleSourceAncho) / imageAncho;
		v1 = (float)(sourceY + rectangleSourceAlto) / imageAlto;
		fRectangleSourceAncho = u1 - u0;
		fRectangleSourceAlto = v1 - v0;
	} else {
		u1 = u0 + fRectangleSourceAncho;
		v1 = v0 + fRectangleSourceAlto;
		rectangleSourceAncho = (int)((u1 - u0) * imageAncho);
		rectangleSourceAlto = (int)((v1 - v0) * imageAlto);
	}

	if ((u1 < u0) || (v1 < v0)) {
		// Las coordenadas de textura no est�n bien. El sprite
		// no es visible.
		notVisible = true;
		return;
	}

	// Ahora las coordenadas en la pantalla.
	float x0, y0;
	float x1, y1;

	if (screenIntUsed) {
		x0 = (float)screenX / screenAncho;
		y0 = (float)screenY / screenAlto;
		fScreenX = x0;
		fScreenY = y0;
	} else {
		x0 = fScreenX;
		y0 = fScreenY;
		screenX = (int)(fScreenX * screenAncho);
		screenY = (int)(fScreenY * screenAlto);
	}

	if (autoScale) {
		// El tama�o en la pantalla ser� el mismo que el de la textura.
//		n_printf("Relacion tama�os: %d %d %d %d\n", imageAncho, screenAncho, imageAlto, screenAlto);
		x1 = x0 + (u1 - u0)*(float)imageAncho / screenAncho;
		y1 = y0 + (v1 - v0)*(float)imageAlto / screenAlto;
	} else {
		// El tama�o en pantalla viene dado por los atributos.
		if (rectangleScreenIntUsed) {
			x1 = (float)(screenX + rectangleScreenAncho) / screenAncho;
			y1 = (float)(screenY + rectangleScreenAlto) / screenAlto;
			fRectangleScreenAncho = x1 - x0;
			fRectangleScreenAlto = y1 - y0;
		} else {
			x1 = x0 + fRectangleScreenAncho;
			y1 = y0 + fRectangleScreenAlto;
			rectangleScreenAncho = (int)((x1 - x0) * screenAncho);
			rectangleScreenAlto = (int)((y1 - y0) * screenAlto);
		}
	}

	if ((x1 < x0) || (y1 < y0) || (x0 > 1) || (y0 > 1) || (x1 < -1) || (y1 < -1)) {
		// El rectangulo no aparece en la pantalla.
		notVisible = true;
		return;
	}

	// Por �ltimo, damos la vuelta a las y's y v's, porque los c�lculos est�n hechos
	// con el valor peque�o arriba, y en realidad, est� abajo.
	y0 = 2 * (1-y0) - 1;
	y1 = 2 * (1-y1) - 1;
	x0 = 2 * x0 - 1;
	x1 = 2 * x1 - 1;

	// Ya tenemos los valores; los metemos en el buffer de v�rtices.
	ref_vb->LockVertices(); 
	
	// Arriba izquierda (vertice 0)
	ref_vb->Coord(0, vector3(x0, y0, 0));
	ref_vb->Uv(0, 0, vector2(u0, v0));

	// Arriba derecha (vertice 1)
	ref_vb->Coord(1, vector3(x1, y0, 0));
	ref_vb->Uv(1, 0, vector2(u1, v0));

	// Abajo derecha (vertice 2)
	ref_vb->Coord(2, vector3(x1, y1, 0));
	ref_vb->Uv(2, 0, vector2(u1, v1));

	// Abajo a la izquierda (vertice 3)
	ref_vb->Coord(3, vector3(x0, y1, 0));
	ref_vb->Uv(3, 0, vector2(u0, v1));

	ref_vb->UnlockVertices();

	notVisible = false;
}

bool nCSprite::UpdateScreenSize() {

	if ( !refGfx.isvalid() )
		return false;

	// get position coords
	int left, top, right, bottom;
	refGfx->GetDisplayDesc(left,top,right,bottom);
	if ((screenAlto != (bottom - top)) ||
		(screenAncho != (right - left))) {
		screenAlto = bottom - top;
		screenAncho = right - left;
		return true;
	}

	return false;
}

/**
Devuelve la posici�n del sprite en la pantalla.
*/
void nCSprite::getPosition(int &posX, int &posY) {
	if (vbDirty)
		calculateVertexBuffer();

	posX = screenX;
	posY = screenY;
}

/**
Devuelve la posici�n del sprite en la pantalla.
*/
void nCSprite::getPositionf(float &posX, float &posY) {
	if (vbDirty)
		calculateVertexBuffer();

	posX = fScreenX;
	posY = fScreenY;
}

/**
Devuelve el tama�o del sprite en la pantalla.
*/
void nCSprite::getScreenSize(int &width, int &height) {
	if (vbDirty)
		calculateVertexBuffer();

	if (autoScale) {
		width = rectangleSourceAncho;
		height = rectangleSourceAlto;
	} else {
		width = rectangleScreenAncho;
		height = rectangleScreenAlto;
	}
}

/**
Devuelve el tama�o del sprite en la pantalla.
*/
void nCSprite::getScreenSizef(float &width, float &height) {
	if (vbDirty)
		calculateVertexBuffer();

	if (autoScale) {
		width = fRectangleSourceAncho;
		height = fRectangleSourceAlto;
	} else {
		width = fRectangleScreenAncho;
		height = fRectangleScreenAlto;
	}
}


/**
Devuelve la posici�n de la esquina superior izquierda del sprite
dentro de la imagen origen (textura).
*/
void nCSprite::getSourcePosition(int &posX, int &posY) {
	if (vbDirty)
		calculateVertexBuffer();

	posX = sourceX;
	posY = sourceY;
}

/**
Devuelve la posici�n de la esquina superior izquierda del sprite
dentro de la imagen origen (textura).
*/
void nCSprite::getSourcePositionf(float &posX, float &posY) {
	if (vbDirty)
		calculateVertexBuffer();

	posX = fSourceX;
	posY = fSourceY;
}

/**
Devuelve el tama�o del rect�ngulo en la imagen origen.
*/
void nCSprite::getSourceSize(int &width, int &height) {
	width = rectangleScreenAncho;
	height = rectangleScreenAlto;
}

/**
Devuelve el tama�o del rect�ngulo en la imagen origen.
*/
void nCSprite::getSourceSizef(float &width, float &height) {
	width = fRectangleScreenAncho;
	height = fRectangleScreenAlto;
}

/**
Devuelve cierto si el tama�o de la textura del par�metro es distinta
de (sizeXExternalTexture, sizeYExternalTexture).
@note Si es distinta, iguala el tama�o de la del par�metro a las variables,
es decir, la siguiente llamada a esta funci�n, devolver�a false.
*/
bool nCSprite::UpdateSiblingTextureSize(nTextureArray *ta) {
	n_assert(ta);

	nTexture* tex = ta->GetTexture(0);
	n_assert(tex);

	int ancho = tex->GetWidth();
	int alto = tex->GetHeight();

	if ((sizeXExternalTexture != ancho) ||
		(sizeYExternalTexture != alto)) {
		sizeXExternalTexture = ancho;
		sizeYExternalTexture = alto;
		return true;
	}

	return false;
}